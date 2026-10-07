

# TODO List for long standing issue

## Assessment overview

Effort scale: **S** ≤ 1 day · **M** 2–5 days · **L** 1–3 weeks · **XL** a month or more.
Confidence is how sure the estimate is. Questions are numbered (Q1.1, …) so they
can be answered inline.

| Point                         | Effort                                    | Confidence | Depends on                     |
|-------------------------------|-------------------------------------------|------------|--------------------------------|
| SDL and Android               | XL                                        | low        | Fast start, Multithreading, Drivers |
| Sleep mode                    | S–M                                       | medium     | –                              |
| Fast start                    | L–XL                                      | low        | Sleep mode, Multithreading, Drivers |
| Multithreading und atomic     | M to start, L to fix                      | medium     | –                              |
| Driver implementation         | L                                         | medium     | Multithreading                 |
| Multiple Configurations       | M as separate processes, XL in one process | medium    | –                              |
| IR emulation via UDP          | M–L, after a 2–3 day investigation        | low        | Multiple Configurations        |


Suggested order (to be decided):
1. Sleep mode, then a ThreadSanitizer pass (Multithreading).
2. Drivers and Fast start together.
3. SDL and Android.
4. Multiple Configurations, then IR.

## SDL and Android

There are stubs to generate Einstein for Android using SDL3. This is a bigger
project that needs extensive planning. How can we cross compile comfortably?
How can users change settings (SDL doe not offer a GUI)? Is a NewtonScript
setting app good enough? How can we handle the ROM and Flash files confortably?
Ho will user install new software, synchronize, connect over Wifi or serial
port? How can they insert and remove virtual PCMCIA cards? How can we
handle the screen orientation and resolution?

What else do we need to make Einstein agood experience on Android and other
SDL devices?

**Assessment:** Effort XL, confidence low.

**Findings**
- `app/SDL` is about 1,000 lines with many TODOs (e.g. TSDLApp.cpp:632-638).
- `android-project/` is the Gradle project with the SDL activity.
- There is also an older native Android port with a Java UI
  (`_Build_/AndroidStudioNative`, `TAndroidNativeApp`), which may be worth
  mining for ideas.
- Android can kill an app at any time, so a good Android experience depends on
  Fast start.

**To clarify** (in addition to the questions above)
- **Q2.1** Which targets: Android only, or also iOS and Linux handhelds through
  SDL? What minimum Android version and which CPU types (ABIs)?
- **Q2.2** Distribution: Play Store or sideloading? The ROM cannot be shipped, so
  how do users import it? (Android's system file picker can do this.)
- **Q2.3** Settings: a NewtonScript preferences app inside the emulated Newton, a
  native Android screen, or both? Some settings are needed before the Newton
  boots: ROM, RAM size, screen size.
- **Q2.4** Input: is stylus and touch precision good enough? Hardware keyboard
  support? A back-button mapping?
- **Q2.5** Screen: rotation and scaling, Newton portrait vs. landscape, integer
  scaling vs. stretching.
- **Q2.6** Connectivity: NE2000 networking, serial over TCP, how to install
  packages (drag-and-drop does not exist).
- **Q2.7** Who tests on real devices, and can CI build and sign the APK?

## Sleep mode

Verify that we go into sleep mode correctly and that the state is saved. What
happens if we exit the app? Does the emulation close gracefully?

**Assessment:** Effort S–M, confidence medium.

**Findings**
- The FLTK app quits by calling `mEmulator->Quit()`, which just stops the CPU
  (TFLApp.cpp:417-423). It does not check whether NewtonOS is awake or asleep.
- RAM is not saved, so the next launch is a reboot.
- Flash is a memory-mapped file that is synced on write and erase. If Einstein
  quits in the middle of a store operation, the store can be left inconsistent.
- The power switch is `SendPowerSwitchEvent()`, and the platform manager knows
  the power state (`IsPowerOn()`).

**To clarify**
- **Q3.1** On quit: put the Newton to sleep first and wait until it is asleep?
  How long until we force the quit? What if NewtonOS shows a dialog or hangs?
- **Q3.2** Should closing the window mean sleep instead of quit?
- **Q3.3** Should FLTK, Cocoa and SDL behave the same?
- **Q3.4** How do we test that the store survives a quit? For example, quit
  repeatedly during heavy store activity.

### Fast start

After falling asleep and quitting EInstein, restarting Einstein should bring
up the emulator where it left off. There is an API to save the current emulation
state, but I could never get that to run correctly. We need to find the bugs
and missing states and make a fast start the default.

**Assessment:** Effort L–XL, confidence low.

**Use cases** (Matt, brainstorming, not decided yet)
1. *Fast start:* on quit, the emulator is told to sleep. When it reaches the
   sleep state, it saves everything but ROM and flash, then Einstein quits.
   When Einstein is restarted with the same parameters (ROM, flash), it
   continues from there. "Everything" includes CPU registers, MMU, interrupt
   controller, timers, the platform manager's queues and the PCMCIA registers,
   because a sleeping Newton is not powered off.
2. *Debug snapshot:* save the state while the Monitor is at a breakpoint, and
   restore it later in the same session to repeat a debugging run. This was the
   original purpose of `SaveState`/`LoadState`, which probably explains why
   caches and pointers are not cleared: a restore was only expected within the
   same Einstein session.

**Design options** (brainstorming)
- Two independent systems for the two use cases.
- One `TransferState` tree with a `TStream` subclass that carries the extra
  information (for example "include ROM and flash" for debug snapshots).
- Only solve Fast start and leave debug snapshots for future Matt.
- Extra phases walked over the same tree, so the order is always the same:
  *prepare* (stop DMA, pause threads), *transfer* (write, read or reset),
  *resume* (restart after save or load). The tree is walked three times.
- Reset mode (see Hardware Reset below): `TStream` gets a third state,
  *resetting*. A `TResetStream` subclass moves no bytes; new overloads such as
  `TransferInt32BE(mTimer, 0)` set the power-on value. Calls without a reset
  value keep the current value (right for RAM, flash, configured screen size).
  One list per class then defines save, load and reset. A forgotten reset value
  silently keeps old state, so the coverage check (below) should list every
  field without one.
- Flash validity for Fast start: the last write time of a memory-mapped file is
  not always updated reliably, and copying a file can keep or change it. A
  checksum of the 4 MB flash file is cheap and more robust (decision open).

**Findings: the TStream system**
- `TStream` is abstract (`Read`, `Write`, `FlushOutput`, `PeekByte` are pure
  virtual). `TRandomAccessStream` is also abstract and only adds a cursor.
  `TFileStream` is the only concrete subclass; there is no memory stream.
- Six transfer methods: `TransferBoolean`, `TransferByte`, `TransferInt16BE`,
  `TransferInt32BE` (unsigned and signed), `TransferInt32ArrayBE`, and a raw
  `Transfer(buffer, count)`. The direction comes from `mIsReading` and
  `mIsWriting` (reading wins if both are set). No per-section versions, no
  error reporting.
- Callers: the Monitor commands `save`, `load`, `snap`, `revert` (only while
  halted) and the CLI app. The FLTK app always creates the Monitor
  (`TFLApp::InitMonitor`), so the halt logic is already there.
- `TEmulator::TransferState` (TEmulator.cpp:516) saves memory (ROM, RAM,
  breakpoints, MMU, flash), the CPU including `TNativePrimitives` (called from
  `TARMProcessor::TransferState`), interrupts, DMA and the screen.

**Findings: bugs in the existing code**
1. *Stale JIT code after a load.* `TMemory::TransferState` only calls
   `InvalidateTLB()`, which clears the virtual-address map. Translated pages
   are also indexed by physical address, and `TJITCache::GetPage()` reuses
   them, so code in RAM can keep running the old translation. The RAM block is
   also `realloc`ed, so translated pages may point into freed memory.
2. *Thread-control flags are saved as machine state:* `TEmulator`
   (`mInterrupted`, `mRunning`, `mPaused`, `mBPHalted`, `mBPID`) and
   `TInterruptManager` (`mRunning`, `mExiting`, `mWaiting`). Restoring them can
   stall a host thread.
3. *The whole 16 MB ROM image is saved and restored,* overwriting the current
   ROM and its patches. A snapshot from a different ROM silently replaces it.
4. *The flash is saved and restored.* Loading takes the user's store back to
   the time of the snapshot. An old snapshot can roll back newer data.
5. `LoadState` leaks the stream on its three error returns and never checks
   that the file was long enough.
6. No coordination with other threads: the interrupt timer thread and the
   serial DMA threads keep changing state during a save or load.

**Findings: state that is not saved** (checked against the compiler's field
layout of each class)

| Class | Missing |
|---|---|
| `TPlatformManager` | event queue and its read/write positions, buffer queue, `mBufferNextID`, `mPowerOn`, `mQueuePreLock`, `mQueueLockCount`, `mQueueBootLock` |
| `TScreenManager` | tablet sample buffer and its read/write positions |
| `TNativePrimitives` | `mSoundOutputBuffer1Addr`, `mSoundOutputBuffer2Addr` |
| `TSoundManager` | `mInputIntMask`, `mOutputIntMask`, `mOutputVolume` |
| `TMemory` | `mSerialNumberIx` (read position in the serial number chip); watchpoints (debug only) |
| `TPCMCIAController` ×2 | all 17 registers, and which card is in each slot |
| PCMCIA cards | `TLinearCard`: `mState`, `mStatusRegister`; `TATACard`: 11 task-file registers, FIFO and its position, `mState` |
| `TBasicSerialPortManager` ×4 | 12 DMA registers per port (buffer start, position, countdown, interrupt enable, event; transmit and receive) |
| `TEmulator` | `mZAPMemory` (minor) |

`TDMAManager` only has `mAssignmentReg`, which is saved; the DMA channel
registers live in the serial drivers.

**Verification**
1. *Round trip:* save A, load A, save B, compare the files byte for byte.
   Catches fields that are read and written in a different order. Cannot find
   fields that are missing entirely.
2. *Coverage check:* compare the compiler's field list
   (`-Xclang -fdump-record-layouts-complete`) with the `Transfer…` calls of
   each class. Every field is classified once: saved, derived (rebuilt after a
   load, e.g. the MMU cache), host-only (pointers, logs, mutexes, threads), or
   configuration (from the preferences). Turn this into a small tool.
3. *Load into a freshly started Einstein,* not only into the running one;
   loading into the same instance hides missing state because the right values
   are still in memory. Scenarios: idle, typing in Notes, sound playing, serial
   connected, PCMCIA card inserted, Newton asleep.
4. After the reset mode exists: the Hardware Reset test (Q4.8).

**Implementation plan** (each step a small, separately explained change)
0. Before starting (decided): a ThreadSanitizer baseline run (serial, sound,
   network), so races that existed before are known. Make the run-control
   flags (`mRunning`, `mPaused`, …) properly atomic or locked and build one
   clean way to pause host threads, because step 6 depends on it. All other
   threading fixes happen when Fast start touches that code.
   *Done 2026-10-07 (not committed):* run-control flags in `TEmulator` are
   `std::atomic` and no longer saved (state file version 2);
   `TARMProcessor::mPendingInterrupts` is `std::atomic`;
   `TSerialPortDriver::Suspend()/Resume()` (TCP client implements them with a
   mutex around its DMA work), `TSerialPorts::SuspendAll()/ResumeAll()` called
   from `TEmulator::TransferState`; `TInterruptManager::TransferState` holds
   its mutex. No sound hook: the audio thread touches no emulated memory and
   raises interrupts through the interrupt manager's lock.
1. Temporary GUI items "Save State" and "Load State" that tell the Monitor to
   stop, save or load a fixed file in Einstein's data folder, and run again.
   Changes: `TFLAppUI.fl` with its generated `.cpp`/`.h`, two handlers in
   `TFLApp`.
   *Done 2026-10-07 (not committed):* Platform menu "Save State" (Cmd+Shift+K)
   and "Load State" (Cmd+Shift+L), file `Einstein.state` in the user data
   folder (`Fl_Preferences::getUserdataPath()`). `TMonitor::RequestSaveState()`
   / `RequestLoadState()`: if the emulator is halted (the UI gets the monitor
   mutex), the UI thread saves/loads directly; if it runs, the request is
   handed to the monitor thread, the emulator is stopped, `RunEmulator()`
   does the transfer and continues running. TSan run: no new races. Also:
   committed fluid output is now excluded from the clang-format check
   (`EINSTEIN_FLUID_OUTPUT` source property).
2. Fix the existing bugs: full JIT flush after a load (new "invalidate
   everything" in `TJITCache`), stop saving thread-control flags, fix the leaks
   and length checks in `LoadState`, raise the file version to 2.
   *Done 2026-10-07 (not committed):* `TJITCache::InvalidateAll()` (unlinks
   every page from the physical map and clears the virtual map), called by
   `TMemory::TransferState` after loading. `TEmulator::SaveState/LoadState`
   return `Boolean`, catch exceptions (missing or truncated file), and free
   the stream on every path; the Monitor reports failures. Guards make sure
   the serial drivers are resumed and the interrupt manager's mutex is
   unlocked even when loading throws. A truncated file still leaves a
   partially loaded state (decision for later: message and reboot, so a bad
   RAM image never writes to flash).
   Tested with an lldb driver (scratchpad `drive.py`): boot, save, tap Names,
   Dates, Extras, load → screen identical to the saved one, Extras opens over
   Notes as expected; loading a half-size copy prints an error, no crash.
3. Round-trip check as a Monitor command and a temporary menu item.
   *Done 2026-10-07 (not committed):* Monitor command `checkstate [path]` and
   Platform menu "Check State Round Trip" (Cmd+Shift+R): save to `…A.state`,
   load it, save to `…B.state`, compare byte by byte. `TEmulator` records
   where each section starts while saving, so a failure reports the number
   of differing bytes per section. First result: A and B identical (29437311
   bytes). That only shows that what is saved is also loaded; missing state
   (step 4) needs the coverage check and loading into a fresh Einstein.
4. Add the missing state one class per change: platform manager, screen and
   tablet, sound, serial DMA registers, PCMCIA controllers and cards. Run
   verification 1–3 after each.
   *Progress (Matt left this to Claude on 2026-10-07; commits are local until
   Matt reviews them):*
   - Baseline: loading a state from another session into a freshly started
     Einstein already works while the Newton is awake and idle (screen
     identical, taps work).
   - Platform manager (`98645599`, file version 3): power state, pending
     events and buffers, queue locks. Before, a state saved while asleep and
     loaded into a fresh Einstein needed two power button presses to wake;
     now one. After loading, the screen is switched on or off to match the
     loaded power state. Round trip identical while awake and while asleep.
   - Pen samples and sound (`7c7f25c5`, version 4): pending tablet samples
     (ring buffer and positions), sound interrupt masks and volume (loaded
     through `OutputVolume()`, so the host follows), sound buffer addresses.
   - Serial ports (`e040d658`, version 5): the 12 DMA registers of each port
     (`TSerialPortDriver::TransferState`, implemented in
     `TBasicSerialPortManager`; the driver behind a port is a preference).
     The driver thread is woken after loading. Round trip identical with the
     Dock app open.
   - PCMCIA (`7467166c`, version 6): both controllers' registers; card state
     (linear: flash state machine; ATA: task file registers, FIFO, state;
     NE2000: none) saved with a card tag and length, using the new header-only
     `TMemoryStream`. Loading with a different card (or none) skips the card
     state with a warning. Tested with a copied 2 MB linear card: same card
     works; without the card, NewtonOS shows "Sorry, a problem has occurred
     (-7338348)" after waking, because its RAM expects the card. **Step 5 must
     make sure the same cards are inserted (refuse, or insert them).**
   - Serial number chip read position (`9c5ff5b8`, version 7).
   - Deliberately not saved: `TEmulator::mZAPMemory` (a pending Brain Wipe
     must not survive into a fast start), watchpoints (debug only).
   - Test sandboxes (scratchpad `drivehome`, `cardhome`): note that FLTK wraps
     long preference values with `+` continuation lines; the flash path in
     `drivehome` pointed at the TSan sandbox's flash until this was noticed.
     Loading restores the flash, so the cross-session results stand.
5. Stop saving the ROM: save ROM ID, REX checksum and RAM size instead, refuse
   mismatching snapshots, re-apply breakpoints from the saved list. Apply the
   flash decision.
   *Decisions (Matt, 2026-10-07):* fast start snapshots (quit: sleep, save,
   quit) store only a flash checksum; fast start only if nothing changed.
   Debug snapshots store the whole flash, because soups may change after the
   snapshot. PCMCIA: card kind and contents checksum must match (a card may
   have been restored from a backup or used in another emulated Newton);
   otherwise boot normally.
   *Done (`b97523dd`, file version 8, not pushed yet):* `TEmulator::SaveState(
   path, kind)` with `kFastStartState` / `kDebugState`; a check block after the
   header: ROM+REX checksum (computed when TMemory is created from a
   `TROMImage`, before any breakpoint), RAM size, flash checksum, card tag and
   contents checksum per socket. `LoadState` compares before changing
   anything and prints every mismatch ("the flash changed", "the contents of
   the card in socket 0 changed", ...). The flash is only checked for fast
   start files and only saved in debug files (`TStream::TransferFlags()`,
   `kStateIncludesFlash`). The ROM is no longer saved; ROM breakpoints are
   removed before loading and re-applied from the loaded list. Debug files
   12.7 MB, fast start files 4.3 MB (were 29.4 MB). Monitor: `save fast
   <path>`; `TMonitor::RequestSaveState(path, fastStart)`.
   Debug snapshots refuse a changed card too (card contents are not saved;
   saving them like the flash could come later).
   Tests (lldb driver): breakpoint at 0x1412FC survives save/load; fast start
   file refused with a fresh flash and with a missing card; saved, quit
   normally, and loaded 2 s after the next launch: accepted, breakpoint back,
   one power press wakes into Dates; debug file loads after a full boot with a
   fresh flash. Note: tests must quit Einstein normally (card pages are written
   to the image file later) and load before NewtonOS boots, or the card and
   flash checksums differ, as they should.
6. Thread safety: pause the timer, serial, sound and network threads during
   save and load (much simpler if snapshots are only taken while asleep).
   *Checked 2026-10-08 (TSan, interactive: Save/Load/Check State, Hardware
   Reset):* 21 reports, all in the deferred groups (`mCPSR_I`, interrupt
   controller registers, tablet, TCP DMA, CoreAudio), none in the save, load
   or reset paths. The timer thread is suspended while the emulator is
   stopped, serial drivers are suspended by `TEmulator::TransferState`, the
   sound thread only raises interrupts under the interrupt manager's lock,
   and the network thread was not active. Nothing more needed for now.
7. Reset mode: `TResetStream`, reset-value overloads, a `StateChanged()` hook
   per class shared by load and reset; switch Hardware Reset to it.
   *Done (`700ec75f`, not pushed yet):* `TStream::IsResetting()` and
   `TransferXxx(value, resetValue)` overloads (header inline); header-only
   `TResetStream`. No separate `StateChanged()` hook: the existing
   `if (IsReading())` follow-ups (JIT flush, screen power, serial wake-up)
   now also run when resetting. Reset values are the member initializers.
   Reset: interrupt controller, MMU, DMA, serial DMA, PCMCIA interrupt
   registers, card state machines, pen samples, sound masks and buffers,
   platform manager queues (boot lock set again), serial number chip
   position (power-on value 64, not 0: with 0 NewtonOS reports "This unit's
   serial number cannot be read"); then `TARMProcessor::Reset()`. Kept: RAM
   (like the real reset button, my default for Q4.9), flash, clock, volume,
   PCMCIA pin registers (they show the inserted card).
   Hardware Reset and Brain Wipe go through `TMonitor::RequestReset()`
   (emulator stopped, reset, continues) instead of calling
   `TARMProcessor::Reset()` from the UI thread.
   Tests: reset after boot (reboot, notes kept, taps work, round trip
   identical before and after); reset with a card and the Dock app open
   (reboot, card mounted again); Brain Wipe asks "Do you want to erase data
   completely?".
8. Fast start: on quit, sleep and save; on launch, load a matching snapshot or
   boot normally. Remove the temporary menu items.
   *Cards (Matt, 2026-10-07):* if the fast start state was saved with one or
   two PCMCIA cards inserted, insert the same cards again when loading, then
   run the checksum test. If it fails, boot normally, but leave the cards
   inserted so NewtonOS mounts them again. That needs the card identity
   (image path or card UUID from the card list) in the state file header, so
   the front end can insert the cards before the state is loaded.
   *Decisions (Matt, 2026-10-08):* after a successful fast start, always wake
   the Newton. Delete the fast start file after loading (saves users from
   endless loops on unforeseen errors). Fast start is a setting: a checkbox
   under "Fetch Date and Time" in the "MessagePad" tab of the Settings
   dialog, checked on the first run, then saved and loaded with the
   preferences. Keep the temporary Save/Load/Check menu items for now and
   hide them later; Matt has an idea for user-friendly checkpoints (test-
   install apps and return quickly), for later.
   *Done (`9ebb41d0`, file version 9, not pushed yet):*
   - Quit (`TFLApp::UserActionQuit` → `SaveFastStartState`): delete an old
     `FastStart.state` (data folder), press power if awake, wait up to 10 s
     for `IsPowerOn()` == false while the UI runs, stop the emulator, save
     through the monitor's halted path (fast start kind), then the normal
     quit (which flushes the card files). No sleep in time: no file.
   - Launch (`LoadFastStartState`, before the emulator thread starts): read
     the cards from the header (`TEmulator::ReadStateCards`), insert them
     (card list lookup by image path, network card by kind), load. Loaded:
     wake the Newton 0.5 s after the emulator thread starts. Not matching:
     clear RAM, `ResetState()`, normal boot with the cards left inserted.
     The file is deleted either way. Empty slots get the kept cards.
   - Setting "Fast start: continue where Einstein was quit" under "Fetch
     date and time" in the MessagePad tab (`System/FastStart`, default 1).
   - `SaveState` writes `<path>.tmp` and renames it when complete, so a
     state file is never cut off.
   - Tests (lldb driver, quit through the event loop): quit in Dates with a
     card → 4.3 MB file, exit 0; plain launch → awake in Dates after 15 s,
     taps work, file deleted; with a changed flash → "the flash changed",
     normal boot, card mounted again; setting off → no file.
   - Not done yet: hide the temporary menu items (later, with Matt's
     checkpoint idea); Cocoa and SDL front ends have no fast start.
   - Fix (2026-10-08): quitting through the macOS menu bar or Cmd-Q waited
     until the timeout, because the quit waited in a nested `Fl::wait()` loop
     inside the menu callback, where the emulator thread could not get the
     FLTK lock to switch off the screen. Now `UserActionQuit()` only starts
     the process; a timer (`SaveFastStartStateTimer`) steps through sleep,
     halt and save, and calls `QuitNow()` through `Fl::awake()`, because on
     macOS a timer fires inside the event wait and closing the windows from
     there would leave `Fl::run()` waiting.

**To clarify**
- **Q4.1** Is "snapshot only when asleep" acceptable? (It is much simpler.)
- **Q4.2** When is a snapshot invalid: a different Einstein version, a changed
  ROM, REX or flash file, a different RAM size? Should it fall back to a normal
  boot silently?
- **Q4.3** What happens with a corrupt snapshot? Delete it and boot normally?
- **Q4.4** Clock jump after a restore: is the host-time patch enough?
- **Q4.5** Should external connections (TCP serial, network) reconnect on their
  own?
- **Q4.6** Where is the snapshot stored, and is there one per configuration?
- **Q4.7** Should a "cold boot" option stay available in the UI?

### Hardware Reset (related to Fast start)

Suspected cause of the store being erased after a reset. The old theory (a ROM
checksum over the *patched* ROM) is unlikely: six patches change data that the
OS reads (`gDebuggerBits`, `gNewtConfig`, four time-base words), so a checksum
over the whole ROM would fail with or without a mirror of the original ROM.

**Findings**
- The FLTK menu offers three resets (`TFLApp::UserActionReset`, TFLApp.cpp:620):
  - "NewtonScript Reboot" calls `Reboot()`, so NewtonOS shuts down cleanly.
    This should be safe.
  - "Hardware Reset" only calls `TARMProcessor::Reset()`, which resets the CPU
    registers and mode. The MMU, the JIT cache, pending interrupts and timers in
    `TInterruptManager`, active DMA channels, the serial driver threads,
    PCMCIA and the platform manager's event queue all keep running as they were.
    A real MessagePad's reset button resets the Voyager chip and all
    peripherals together; only RAM survives. NewtonOS boots against hardware in
    a state it never sees on a real machine.
  - "Brain Wipe" sets the ZAP flag, which the REX reports once through
    `ResetZAPStoreCheck` (TNativePrimitives.cpp:667). NewtonOS then erases the
    store on purpose. That is intended.
- The code overlaps with Fast start: a correct reset needs every device to
  return to its power-on state, and Fast start needs every device to save and
  restore its state. Both need the same complete list of emulated state.

**Test to confirm it:** create a few notes, use Hardware Reset several times
(also while the Newton is busy, e.g. syncing or writing to the store), check
whether the store survives, and compare with NewtonScript Reboot.

**Possible fixes**
- Rebuild the emulator the way a fresh launch does, keeping only the flash file
  (most reliable).
- Or give every device a `Reset()`, covering the same state list as Fast start.

**To clarify**
- **Q4.8** Does Hardware Reset actually lose the store? (Run the test above.)
  **A4.8** (test, 2026-10-07) Not in the idle case: booted, Hardware Reset via
  `UserActionReset(0)`, NewtonOS reboots and the note is still there. A loss
  may still need store activity, active DMA or cards during the reset. The
  current reset also calls `TARMProcessor::Reset()` from the UI thread while
  the emulator runs (a race), and leaves all other devices as they were.
- **Q4.9** Should Hardware Reset keep RAM like the real reset button, or is a
  reset without RAM (closer to a fresh launch) good enough?

## Multithreading und atomic

Verify that multithreading is implemented coreectly and atomics are used where
needed and in a correct way. The original app had no atomics and thos I put in
were my first step in this garden of joy.

**Assessment:** Effort M to start, L to fix, confidence medium.

**Findings**
- There are about 20 places that start threads:
  - the emulator thread and the UI thread;
  - the interrupt timer thread;
  - one thread per serial driver;
  - sound threads (each sound backend);
  - network, TCP, and the Monitor.
- `TEmulator::mRunning`, `mPaused` and `mInterrupted` are plain integers. The UI
  thread writes them in `Stop()` while the emulator thread reads them. Only
  `mSignal` is atomic.
- `TInterruptManager` takes a lock when it changes `mIntRaised`, but its getters
  read it without the lock (TInterruptManager.h:298).
- Serial drivers write into emulated memory directly from their own threads
  (DMA) while the CPU thread runs. That interacts with the JIT cache when the
  code runs from RAM.
- Cheapest first step: a ThreadSanitizer build and a session that exercises
  serial, sound and network. That turns guesswork into a concrete list.

**ThreadSanitizer baseline (2026-10-07)**

How it was run: FLTK build with `-fsanitize=thread`, RelWithDebInfo, in a
private `$HOME` (FLTK reads the preferences from `$HOME/Library/Preferences`)
with the 717006 ROM, a copy of the flash file and no PCMCIA cards. Booted for
two minutes, TCP serial driver on `extr`, no network card. 13 distinct races:

| Threads | Reports | What races |
|---|---|---|
| CPU ↔ interrupt timer thread | 7 | `TARMProcessor::mPendingInterrupts` (`IRQInterrupt`/`FIQInterrupt` use `\|=`, `Clear…` uses `&=`, from the timer thread), `mCPSR_I`/`mCPSR_F` read by the timer thread, `TEmulator::mInterrupted` (`TJITGeneric.cpp:165-168`) |
| CPU ↔ interrupt timer thread | 2 | interrupt controller registers: `mFIQMask` written by the CPU (`TMemory::WriteP` → `SetFIQMask`), `mIntRaised` written by the timer (`FireTimersAndFindNext`) and read by the CPU (`GetIntRaised`) without the lock |
| CPU ↔ TCP serial thread | 1 | DMA registers (`TBasicSerialPortManager::WriteDMARegister` vs. `TSerialPortDriverTcpClient::HandleDMA`) |
| CPU ↔ CoreAudio render thread | 1 | `TCircleBuffer` positions (`OutputIsRunning` vs. `Consume`) |
| UI thread ↔ CPU | 2 | tablet state (`TScreenManager::PenDown`/`PenUp` vs. `GetTabletState`/`GetSample`) |

The read-modify-write on `mPendingInterrupts` from two threads can lose an
interrupt. Not covered by this run: other serial drivers, network, PCMCIA,
`TEmulator::Stop()` (never called, the process was killed).

**After step 0 of the Fast start plan (2026-10-07, not committed yet):** 9
reports. Fixed: `mPendingInterrupts` (now `std::atomic`, `|=`/`&=` are atomic)
and `TEmulator::mInterrupted` (all run-control flags are now `std::atomic`).
Still open, as decided: `mCPSR_I` read by the timer thread (5 reports, only
delays an interrupt because `SetCPSR` checks again), TCP DMA registers,
interrupt controller registers, CoreAudio ring buffer. The tablet races were
not exercised in the second run.

**Third run (interactive: tapping, Monitor `stop`/`save`/`load`/`run`, power
button, normal quit):** 77 reports. Fixed right after (not committed):
- `TFLMonitor::DrawScreen()` changed FLTK widgets from the emulator thread
  without `Fl::lock()` (~45 reports). Now locked (FLTK's lock is recursive)
  and followed by `Fl::awake()`.
- `TMonitor::mHalted` and `mCommand` are `std::atomic` (UI ↔ monitor thread).
- `TPlatformManager::mPowerOn` is `std::atomic` (CPU ↔ UI; Fast start waits
  for the Newton to sleep using it).
- `~TSerialPortDriverTcpClient` no longer calls `Disconnect()` while the
  worker thread may use the socket; the worker disconnects on `'q'`.
The Monitor `load` of a snapshot taken a moment earlier worked (emulator kept
running).

**Fourth run (same scenario, with the fixes):** 38 reports, the FLTK widget,
`mHalted`/`mCommand`, `mPowerOn` and TCP destructor races are gone. Remaining:
the deferred ones (`mCPSR_I` 12, interrupt controller registers 3, TCP DMA 1,
CoreAudio 2, tablet 7) and one new pattern (13): after `run`, the Monitor's
buttons and command line call `DrawScreen()` from the UI thread
(TFLMonitor.cpp:911-1007) while `mHalted` is still true, so the UI reads CPU
registers and timers for the halted view while the monitor thread already
starts the emulator. Read-only, a display glitch at worst. Possible fix: let
only the monitor thread redraw after a command (it does so anyway at the top
of its loop).

**Fifth run (with the Monitor fix, not committed):** 11 reports. The Monitor
halted-view races are gone: UI callbacks and `Show()` now call
`TFLMonitor::DrawScreenFromUI()`, which only draws the halted view while it
holds the monitor mutex (the monitor thread holds it whenever it is not waiting
for a command; `TMutex::TryLock()` used to return *false* when it got the
lock, now fixed to return *true* as documented),
and `TMonitor::RunEmulator()` draws the running view itself. Remaining: the
deferred ones, plus `TFLScreenManager::PowerOnScreen()`/`PowerOffScreen()`
changing the screen widget's label from the emulator thread without
`Fl::lock()` (on the sleep path Fast start will use). Fixed afterwards: both
now take `Fl::lock()` and call `Fl::awake()`. Step 0 of the Fast start plan is
complete.

**Sixth run (toolbar power/backlight buttons, tapping, normal quit):** 12
reports, all in the deferred groups (`mCPSR_I`, interrupt controller registers,
TCP DMA, CoreAudio, tablet). No FLTK, Monitor or power-state races left.

**To clarify**
- **Q7.1** Is "clean under ThreadSanitizer for scenarios X, Y, Z" the definition
  of done?
- **Q7.2** Rule to adopt: only the emulator thread changes emulated state, and
  other threads post events? That is cleaner, but changes the drivers.
- **Q7.3** Speed budget: no atomics or locks in the JIT's hot loop?
- **Q7.4** Windows: shared threading code, or keep a separate implementation?

## Driver implementation

A bunch of drivers run in separate threads. We need to verify the implementation
which feels clumsy under Linux/macOS. This also need to be compatible with the
Fast Start issue form earlier in the list.

**Assessment:** Effort L, confidence medium.

**Findings**
- Four Voyager-style serial drivers (Pipes, PTY, BasiliskII, TCP) are largely
  copy-and-paste of each other. Each has its own thread, a pipe, a `select()`
  loop and `usleep()` throttling, and each is 300–750 lines.
- Two serial systems exist side by side:
  - Voyager register emulation (`TBasicSerialPortManager` …);
  - the licensee-driver system (`TSerialHostPort` with `TSerialChipEinstein` in
    the REX).

  The TODO at the top of TSerialPorts.cpp:24-63 already says one should go.
- For Fast start, every driver needs to be able to pause, resume and save its
  state.

**To clarify**
- **Q8.1** Which serial system stays?
  **A8.1** (Research, decision still open.) Matt wrote the original hardware
  register (Voyager) system; the licensee system was added by Eckhart Koeppen
  in 2020 (`e311a865`) and is assumed to be cleaner.

  *Active in the FLTK build today: the Voyager system.*
  - `TMainPlatformDriver::New()` (Drivers/TMainPlatformDriver.cpp:88-95)
    unregisters the ROM's serial chips at `infr`, `tblt` and `mdem`, and keeps
    the one at `extr` for Einstein's port emulation.
  - That chip is the ROM's own `TSerialChipVoyager` (around 0x001D67xx). It
    drives the emulated Voyager registers and DMA in `TBasicSerialPortManager`
    and its subclasses.
  - `TFLApp::InitSerialPorts()` picks the host driver behind the registers:
    the saved driver for `extr` (TCP client by default), null for the rest.
  - Einstein Prefs only talks to this system
    (`Get/SetSerialPortDriverAndOptions` work on `TSerialPorts::mDriver[]`).

  *The licensee system is in the REX but dormant.*
  - It only becomes active when the user installs `portenabler.pkg`
    (Drivers/EinsteinPortEnabler) and calls `Enable(location, type, config)`.
  - That unregisters the ROM chip at the location, creates a
    `TSerialChipEinstein`, and passes an `'eloc'` option. The emulator then
    calls `SerialPorts.SetDriver()` (Emulator/TNativePrimitives.cpp:2066-2101),
    which creates the host port and puts the Voyager driver at that port on null.
  - Only two host drivers exist: `TSerialHostPortDirect` (real host serial
    device) and `TSerialHostPortPTY`, both macOS/Linux only. FLTK defaults them
    to `/tmp/einstein-*.pty`.

  *What a switch to the licensee system would take*
  - Install `TSerialChipEinstein` at boot from the REX, as the port enabler does.
  - Port the TCP client, Pipes and BasiliskII drivers to `TSerialHostPort`;
    today they only exist on the Voyager side.
  - Add a Windows host port (Windows serial is only the Voyager TCP client now).
  - Make Einstein Prefs talk to the licensee system.
  - For IR (point 10): check whether NewtonOS beaming calls Voyager-specific
    chip functions that `TSerialChipEinstein` would also have to provide.

  *Probably dead code:* the REX package `EinsteinSerialVoyager`
  (Drivers/TSerialChipVoyager.cpp, Paul Guyot 2013). Nothing creates it, and
  the emulator ignores its native calls (below 0x30).
- **Q8.2** Which drivers are still needed? Is anyone using Pipes or BasiliskII?
- **Q8.3** Which drivers must work on Windows?
- **Q8.4** Are sound and network in scope, or only serial?
- **Q8.5** What interface should every driver share: start, stop, pause, save
  state?

## Multiple Configurations

It would be neat to be able to store multiple configurations, so the user
has access to multiple devices simultaneously.

**Assessment:** Effort M as separate processes, XL in one process, confidence
medium.

**Findings**
- Running several emulators in one process is blocked by process-wide globals:
  `TNewt::mEmulator`, `mMemory`, `mCPU`, the static patch list, and `gApp`.
- Separate processes, one per configuration, are realistic.
- Today, two instances would collide on:
  - the single preferences group (`robowerk.com/einstein`);
  - the PTY paths `/tmp/einstein-*.pty`;
  - the flash file, which has no lock;
  - the Newton ID.

**To clarify**
- **Q9.1** Does "simultaneously" mean several windows of one app, or is one
  process per configuration fine?
- **Q9.2** What belongs to a configuration? ROM, REX, flash file, RAM size,
  machine model; screen size, serial settings, PCMCIA cards; window position;
  the snapshot.
- **Q9.3** How does the user pick one: a launcher window, a menu,
  `--config <name>` on the command line?
- **Q9.4** Does each instance need a unique Newton ID? (Probably, for Dock and
  beaming.)
- **Q9.5** Migration: the current settings become a "Default" configuration?
- **Q9.6** FLTK only, or Cocoa and SDL as well?

## IR emulation via UDP broadcasting

If we have multiple configurations, this would just be a func project, so
emulators can beam data between them.

**Assessment:** Effort M–L after a 2–3 day investigation, confidence low.

**Findings**
- The `infr` port gets the null driver by default.
- The REX registers an `infr` serial chip (Drivers/TMainPlatformDriver.cpp:94).
- No IR-specific behaviour was found in the emulation: carrier detection,
  half-duplex turnaround, Sharp-IR mode.
- The licensee-driver path (`TSerialChipEinstein`) looks like the cleaner place
  to add it. Sending UDP itself is the easy part.

**To clarify**
- **Q10.1** Which protocols: NewtonOS Beam (Sharp IR), IrDA, or both? Does Beam
  actually work through the emulated chip today? That is what the investigation
  would answer.
- **Q10.2** Range: broadcast to all instances on the LAN, loopback only, or
  "point at" one chosen peer?
- **Q10.3** Should we simulate half-duplex, collisions and range, or deliver
  every packet perfectly?
- **Q10.4** Privacy: is broadcasting beam data on a shared LAN acceptable, or
  opt-in only?
- **Q10.5** Should a real Newton be reachable through a host IR adapter?
  (Probably out of scope.)

## Event forwarding on slow machines

(Matt, 2026-10-08, for after the Fast start work.) On slow machines, or on a
fast machine with ASan, the forwarding of host events to NewtonOS can break:
keyboard events and package installer events stop working. A first place to
look: the platform event queue in `TPlatformManager` with its locks
(`mQueuePreLock`, `mQueueLockCount`, `mQueueBootLock`) and the platform
interrupt that tells NewtonOS about a new event. A timing-dependent lock that
is never released would block all later events.

## Fix CI testing

We do some rather involved testing on some platforms which fails because
it is unmaintained. This is worse than no testing at all. I suggest
we build a Workflow purely for testing and run that at some strategic
times (before a release, or as mandatory part of a release)

Done: EinsteinTests are only configured with `-D EINSTEIN_BUILD_TESTS=ON`
(default OFF, googletest is not downloaded otherwise). The regular CI workflows
build only Einstein. `.github/workflows/tests.yml` builds and runs the tests on
Linux x64/ARM64, macOS and Windows, by hand or via `workflow_call` from another
workflow (e.g. release.yml). The Cocoa workflow (macos.yml) still runs the
Xcode project's own tests.

## fixups

Serial port settings:

Switching back to TCP resets the address: Einstein builds a brand-new TCP driver, which starts at 127.0.0.1:3679. The panel only sends the new driver ID, so that default replaces your saved server and port. Getting your last address back would mean changing TSerialPorts::ReplaceDriver to apply the saved values to a new TCP driver

- AppConstants.f says kTcpServerSerialDriverTag := 5, but in the emulator 5 means kDirectDriver (TSerialPorts.h:59-66).

- kSerialDriverMax is 5, but GetSerialPortDriverNames only returns 5 names (indices 0–4). Picking driver 5 would read past the end of the list.