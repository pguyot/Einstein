

# TODO list for long-standing issues

## Assessment overview

Effort scale: **S** ≤ 1 day · **M** 2–5 days · **L** 1–3 weeks · **XL** a month or more.
Confidence is how sure the estimate is. Questions are numbered (Q1.1, …) so they
can be answered inline.

| Point                         | Effort                                    | Confidence | Depends on                     | Status |
|-------------------------------|-------------------------------------------|------------|--------------------------------|--------|
| SDL and Android               | XL                                        | low        | Fast start, Multithreading, Drivers | open |
| Sleep mode                    | S–M                                       | medium     | –                              | mostly covered by Fast start |
| Fast start                    | L–XL                                      | low        | Sleep mode, Multithreading, Drivers | done, PR #225 in review |
| Multithreading and atomics    | M to start, L to fix                      | medium     | –                              | started, 5 race groups deferred |
| Driver implementation         | L                                         | medium     | Multithreading                 | open |
| Multiple Configurations       | M as separate processes, XL in one process | medium    | –                              | open |
| IR emulation via UDP          | M–L, after a 2–3 day investigation        | low        | Multiple Configurations        | open |
| Event forwarding on slow machines | not assessed                          | –          | –                              | open |
| Fix CI testing                | –                                         | –          | –                              | done |

Suggested order (to be decided):
1. ~~Sleep mode, then a ThreadSanitizer pass (Multithreading).~~ Done as part of Fast start.
2. Wrap up Fast start (review findings, see *Next steps* there), then Drivers.
3. SDL and Android.
4. Multiple Configurations, then IR.

## SDL and Android

There are stubs to generate Einstein for Android using SDL3. This is a bigger
project that needs extensive planning. How can we cross compile comfortably?
How can users change settings (SDL does not offer a GUI)? Is a NewtonScript
setting app good enough? How can we handle the ROM and Flash files comfortably?
How will users install new software, synchronize, connect over Wifi or serial
port? How can they insert and remove virtual PCMCIA cards? How can we
handle the screen orientation and resolution?

What else do we need to make Einstein a good experience on Android and other
SDL devices?

**Assessment:** Effort XL, confidence low.

**Findings**
- `app/SDL` is about 1,000 lines with many TODOs (e.g. TSDLApp.cpp:632-638).
- `android-project/` is the Gradle project with the SDL activity.
- There is also an older native Android port with a Java UI
  (`_Build_/AndroidStudioNative`, `TAndroidNativeApp`), which may be worth
  mining for ideas.
- Android can kill an app at any time, so a good Android experience depends on
  Fast start. Fast start exists only in the FLTK app so far.

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

**Assessment:** Effort S–M, confidence medium. Mostly covered by Fast start.

**Findings**
- With Fast start on (the default), quitting the FLTK app puts the Newton to
  sleep first, waits up to 10 s, and saves the state (see Fast start). RAM
  survives, so the next launch continues instead of rebooting.
- With Fast start off, or when the Newton does not fall asleep in time, quit
  still just stops the CPU (`TFLApp::QuitNow`). It does not check whether
  NewtonOS is awake or asleep.
- Flash is a memory-mapped file that is synced on write and erase. If Einstein
  quits in the middle of a store operation, the store can be left inconsistent.
  Waiting for sleep makes that unlikely, but only with Fast start on.
- The power switch is `SendPowerSwitchEvent()`, and the platform manager knows
  the power state (`IsPowerOn()`, atomic since the Fast start work).

**To clarify**
- **Q3.1** On quit: put the Newton to sleep first and wait until it is asleep?
  How long until we force the quit? What if NewtonOS shows a dialog or hangs?
  **A** (partly): with Fast start on, sleep first, 10 s timeout, then quit
  without a state file. Open: also sleep first with Fast start off?
- **Q3.2** Should closing the window mean sleep instead of quit?
- **Q3.3** Should FLTK, Cocoa and SDL behave the same?
- **Q3.4** How do we test that the store survives a quit? For example, quit
  repeatedly during heavy store activity.

### Fast start

After falling asleep and quitting Einstein, restarting Einstein should bring
up the emulator where it left off. There is an API to save the current emulation
state, but I could never get that to run correctly. We need to find the bugs
and missing states and make a fast start the default.

**Status (2026-10-10):** Done and on by default in the FLTK app, on branch
`FastWakeup`, [PR #225](https://github.com/pguyot/Einstein/pull/225) against
pguyot/Einstein. The review (CodeRabbit, Greptile) found missing checks when
reading damaged state files, a crash when the screen size changes, and two
races. They are the first items under *Next steps*. The investigation notes
this work started from (bug list, missing-state table, step-by-step logs) are
in the history of this file: `git show d230e8a7:CLAUDE.md`.

**How it works**
- *State file* (`TEmulator::SaveState`/`LoadState`, file version 9): header
  (`EINI`, `SNAP`, version, kind), identity block, the inserted cards (tag and
  image path per socket), then the `TransferState` tree: memory (RAM,
  breakpoints, MMU, flash), CPU with native primitives, interrupts, DMA, serial
  DMA registers, PCMCIA controllers and cards, platform manager, screen and
  tablet, sound. Written to `<path>.tmp` and renamed when complete.
- *Two kinds:* fast start files store only a flash checksum (4.3 MB), debug
  files store the whole flash (12.7 MB). The ROM is never saved; ROM
  breakpoints are removed before loading and re-applied from the loaded list.
- *Identity check* before anything changes: ROM+REX checksum, RAM size, flash
  checksum (fast start files only), card tag and contents checksum per socket.
  Every mismatch is printed ("the flash changed", …).
- *One tree for save, load and reset:* a `TStream` reads, writes or resets
  (`TResetStream`). `TransferXxx(value, resetValue)` gives the power-on value;
  calls without one keep the current value (RAM, flash, configuration). The
  follow-ups after loading (JIT flush with `TJITCache::InvalidateAll()`, screen
  power, serial wake-up) also run after a reset. Card state is a tagged block
  with a length (`TMemoryStream`), so loading with a different card skips it.
- *Threads:* `TMonitor::RequestStateTransfer` does the transfer on the UI
  thread if the emulator is halted, otherwise it hands the request to the
  monitor thread and stops the emulator. Serial drivers are suspended during
  the walk, the interrupt manager holds its mutex.
- *Quit* (`TFLApp::UserActionQuit`): a timer presses power if the Newton is
  awake, waits up to 10 s for sleep, stops the emulator, saves
  `FastStart.state` in the user data folder, then quits normally (which
  flushes the card files). No sleep in time: no file.
- *Launch* (`TFLApp::LoadFastStartState`, before the emulator thread starts):
  read the cards from the header, insert them, load, wake the Newton 0.5 s
  after the emulator starts. If loading fails for any reason: clear RAM,
  `ResetState()`, normal boot with the cards left inserted. The file is deleted
  either way.
- *Setting:* "Fast start: continue where Einstein was quit" in the MessagePad
  tab of the Settings dialog (`System/FastStart`, default on).
- *Debug tools:* Monitor `save [fast] <path>`, `load <path>`,
  `checkstate [path]` (save A, load A, save B, compare; reports differing bytes
  per section). Platform menu items Save/Load/Check State (Cmd+Shift+K/L/R),
  hidden since `d230e8a7`.

**Decisions** (Matt)
- Save on quit only after the Newton is asleep. After a successful fast start,
  always wake the Newton. (2026-10-08)
- Fast start files store a flash checksum and only load if nothing changed.
  Debug files store the whole flash, because soups may change after the
  snapshot. (2026-10-07)
- PCMCIA: insert the cards from the header again; card kind and contents
  checksum must match, otherwise boot normally with the cards left inserted
  (a card may have been restored from a backup or used in another emulated
  Newton). (2026-10-07)
- Delete the fast start file after loading, so an unforeseen error cannot cause
  an endless loop. (2026-10-08)
- Fast start is a setting, checked on the first run. (2026-10-08)
- The temporary menu items stay in the code, hidden; later replaced by
  user-friendly checkpoints (test-install apps and return quickly). (2026-10-08)
- Serial: the driver type is a preference and not saved; the DMA registers are
  loaded into whatever driver runs. Revisit for drivers with more state or for
  licensee ports.
- Deliberately not saved: `TEmulator::mZAPMemory` (a pending Brain Wipe must
  not survive into a fast start) and watchpoints (debug only).

**Commits** on `FastWakeup`

| Step | Commit | What |
|------|--------|------|
| 0 | `cbd60874` | Thread safety first: atomic run-control flags (no longer saved) and pending interrupts, serial `Suspend()/Resume()`, TSan fixes |
| 1 | `7e243218` | Temporary Save State / Load State menu items |
| 2 | `511ac19f` | Full JIT flush after loading; `SaveState`/`LoadState` return errors, no leaks |
| 3 | `04294bf0` | `checkstate` round trip |
| 4 | `98645599` … `9c5ff5b8` | Missing state: platform manager, pen samples and sound, serial DMA registers, PCMCIA controllers and cards, serial number chip |
| 5 | `b97523dd` | Identity check, ROM no longer saved, fast start and debug kinds |
| 6 | – | TSan check of save, load and reset: no new races |
| 7 | `700ec75f`, `257bfa7d` | Reset mode; Hardware Reset and Brain Wipe through it |
| 8 | `9ebb41d0`, `f6473cac` | Fast start on quit and launch, setting; quit without a nested wait (macOS menu, Cmd-Q) |
| – | `d230e8a7` | Hide the temporary menu items |

**Testing notes**
- Tests used an lldb Python driver on a sandboxed debug build (save, load, tap,
  screen dump, quit through `Fl::awake`). Sandboxes use a private `$HOME`;
  FLTK wraps long preference values with `+` continuation lines, so check
  that the flash path really points into the sandbox.
- Quit Einstein normally before a test that loads (card pages are written to
  the image file later), and load before NewtonOS boots; otherwise the card and
  flash checksums differ, as they should.
- Without the card the state expects, NewtonOS shows "Sorry, a problem has
  occurred (-7338348)" after waking. That is why cards must match.
- The serial number chip's power-on read position is 64, not 0 (with 0,
  NewtonOS reports "This unit's serial number cannot be read").

**Next steps to wrap up state snapshots**

*Before merging PR #225* (review findings, all checked against `d230e8a7`):
1. **Screen size in the identity block** (Greptile). `TScreenManager::
   TransferState` loads the saved width and height and copies that many pixels
   into a buffer allocated for the current settings. Make the screen smaller,
   quit, relaunch: heap overflow on launch. The screen size is configuration:
   add it to `GetStateIdentity()`, refuse a mismatch, and don't load it.
2. **Damaged files must not crash or allocate gigabytes** (CodeRabbit,
   Greptile). Values from the file are used as sizes and indices without a
   check:
   - card image path lengths in `LoadState` and `ReadStateCards` (which runs at
     every launch, before the identity check);
   - the card state block size in `TPCMCIAController::TransferState`;
   - the ATA FIFO size (at most `kSectorSize`);
   - platform event and buffer counts (a count near 2³² wraps the new capacity,
     then the loop writes past the allocation), buffer sizes, unchecked
     `calloc`;
   - the tablet ring-buffer cursors (must be below `kTabletBufferSize`).

   Proposal: a small helper that throws when a value is out of range, so the
   existing `catch` reports a damaged file and fast start boots normally. Also
   store the payload length in the header and check it against the file size
   before changing anything. That refuses cut-off files up front, which
   matters for debug files: today a file cut off inside the flash section
   overwrites part of the user's flash. *Decision:* length only, or length
   and a CRC32 of the payload?
3. **Lost state requests in the Monitor** (CodeRabbit, Greptile). Two windows
   with the same cause:
   - `RunEmulator()` has left its loop (breakpoint, `stop`) but not yet set
     `mHalted`: the request is queued and never processed;
   - `RunEmulator()` has set `mHalted = false` but not yet entered
     `TEmulator::Run()`, which overwrites `Stop()` with `mRunning = true`.

   Either way `mStateRequest` stays set, and every later Save/Load/Check State
   and Hardware Reset is refused until the next `run`. The fast start save is
   not affected (it waits for `IsHalted()`). Fix: process a pending request
   after `mHalted` is published and before entering `Run()`, or make the stop
   a request that `Run()` does not clear.
4. **Suspend every serial worker** (Greptile). Only the TCP client implements
   `Suspend()/Resume()`. The PTY, Pipes and BasiliskII threads keep writing RAM
   and DMA registers during save, load and reset. Give them the TCP client's
   mutex pattern (or move it into a shared base, see Driver implementation),
   or answer Q8.2 and remove drivers nobody uses.
5. **Check the close before the rename** (Greptile). `TFileStream` ignores the
   result of `fclose()`, so a full disk can publish a cut-off file as a
   success. Flush and close with a result check, keep the old file on failure.
6. Typo "upgarde" in the `LoadState` message (CodeRabbit).
7. Answer the review comments on the PR.

*After merging*
8. Save the fast start file on the monitor thread instead of the UI thread
   (Greptile): the UI is frozen during the write. A 4.3 MB write is quick, so
   low priority.
9. Debug files: after a load that fails halfway, the machine runs on a
   half-loaded state. Reset and reboot instead, as the fast start path does.
   Step 2's length check makes this rare.
10. Debug files do not save card contents, so a changed card is refused. Save
    them like the flash if that is ever needed.
11. Fast start for the Cocoa and SDL front ends (Q3.3).
12. Checkpoints for users (Matt's idea), replacing the hidden menu items.
13. Open questions Q4.4, Q4.5 and Q4.7 below.

**To clarify**
- **Q4.1** Is "snapshot only when asleep" acceptable? **A:** Yes, for fast
  start (decided 2026-10-08). Debug snapshots are taken while the Monitor is
  halted.
- **Q4.2** When is a snapshot invalid? **A:** When the file version, the
  ROM+REX, the RAM size, the flash (fast start files) or a card changed.
  Einstein then boots normally and prints the reason on the console. The
  screen size is still missing (next step 1).
- **Q4.3** What happens with a corrupt snapshot? **A:** Fast start: the file
  is deleted and Einstein boots normally. Debug files: next steps 2 and 9.
- **Q4.4** Clock jump after a restore: is the host-time patch enough?
- **Q4.5** Should external connections (TCP serial, network) reconnect on their
  own?
- **Q4.6** Where is the snapshot stored, and is there one per configuration?
  **A:** `FastStart.state` in the user data folder, one per installation. One
  per configuration once Multiple Configurations exist.
- **Q4.7** Should a "cold boot" option stay available in the UI? Today: turn
  the setting off, or use the Reset menu after a fast start. Is a way to skip
  one fast start at launch needed?

### Hardware Reset (related to Fast start)

Suspected cause of the store being erased after a reset. The old theory (a ROM
checksum over the *patched* ROM) is unlikely: six patches change data that the
OS reads (`gDebuggerBits`, `gNewtConfig`, four time-base words), so a checksum
over the whole ROM would fail with or without a mirror of the original ROM.

**Status:** Rewritten in Fast start step 7 (`700ec75f`). The FLTK Reset menu
(`TFLApp::UserActionReset`) offers three resets:
- "NewtonScript Reboot" calls `Reboot()`, so NewtonOS shuts down cleanly.
- "Hardware Reset" used to call only `TARMProcessor::Reset()`, from the UI
  thread while the emulator ran, and left every device as it was. Now it goes
  through `TMonitor::RequestReset()`: the emulator is stopped,
  `TEmulator::ResetState()` walks the state tree with a `TResetStream`, then
  the CPU takes the reset exception. Reset: interrupt controller, MMU, DMA,
  serial DMA, PCMCIA interrupt registers, card state machines, pen samples,
  sound masks and buffers, platform manager queues (boot lock set again),
  serial number chip position. Kept: RAM (like the real reset button), flash,
  clock, volume, PCMCIA pin registers (they show the inserted card).
- "Brain Wipe" sets the ZAP flag, which the REX reports once through
  `ResetZAPStoreCheck` (TNativePrimitives.cpp:667), so NewtonOS erases the
  store on purpose. Also through `RequestReset()` now.

Tests: reset after boot (reboot, notes kept, taps work, round trip identical
before and after); reset with a card and the Dock app open (reboot, card
mounted again); Brain Wipe asks "Do you want to erase data completely?".

**To clarify**
- **Q4.8** Does Hardware Reset actually lose the store? **A4.8** (test,
  2026-10-07) Not in the idle case, before and after the rewrite. Not tested
  yet: a reset during store activity, sync or active DMA.
- **Q4.9** Should Hardware Reset keep RAM like the real reset button, or is a
  reset without RAM (closer to a fresh launch) good enough? Implemented as
  "keep RAM" (Claude's default); Matt to confirm.

## Multithreading and atomics

Verify that multithreading is implemented correctly and atomics are used where
needed and in a correct way. The original app had no atomics and those I put in
were my first step in this garden of joy.

**Assessment:** Effort M to start, L to fix, confidence medium.

**Findings**
- There are about 20 places that start threads:
  - the emulator thread and the UI thread;
  - the interrupt timer thread;
  - one thread per serial driver;
  - sound threads (each sound backend);
  - network, TCP, and the Monitor.
- `TInterruptManager` takes a lock when it changes `mIntRaised`, but its getters
  read it without the lock (TInterruptManager.h:298).
- Serial drivers write into emulated memory directly from their own threads
  (DMA) while the CPU thread runs. That interacts with the JIT cache when the
  code runs from RAM.

**ThreadSanitizer results** (2026-10-07/08)

How to run: FLTK build with `-fsanitize=thread`, RelWithDebInfo, in a private
`$HOME` (FLTK reads the preferences from `$HOME/Library/Preferences`) with the
717006 ROM, a copy of the flash file and no PCMCIA cards. Use it interactively;
calling functions through lldb in a TSan binary is unreliable.

Six runs: two minutes idle with the TCP serial driver on `extr`, then
interactive sessions (tapping, Monitor `stop`/`save`/`load`/`run`, power and
backlight buttons, Save/Load/Check State, Hardware Reset, normal quit). The
baseline had 13 distinct races; the first interactive run 77 reports; the last
run 12 reports, all in the deferred groups below.

Fixed (Fast start step 0, `cbd60874`):
- `TARMProcessor::mPendingInterrupts` is `std::atomic`. Before, `|=` and `&=`
  from the CPU and the timer thread could lose an interrupt.
- The run-control flags in `TEmulator` (`mRunning`, `mPaused`, `mInterrupted`,
  …), `TMonitor::mHalted` and `mCommand`, and `TPlatformManager::mPowerOn`
  are `std::atomic`.
- FLTK widgets changed from the emulator thread without `Fl::lock()`:
  `TFLMonitor::DrawScreen()`, `TFLScreenManager::PowerOnScreen()` and
  `PowerOffScreen()`. Now locked (FLTK's lock is recursive), then `Fl::awake()`.
- The UI drew the Monitor's halted view while the monitor thread restarted the
  emulator. UI callbacks now call `TFLMonitor::DrawScreenFromUI()`, which only
  draws while it holds the monitor mutex. `TMutex::TryLock()` used to return
  *false* when it got the lock; now it returns *true* as documented.
- `~TSerialPortDriverTcpClient` no longer calls `Disconnect()` while the worker
  thread may use the socket; the worker disconnects on `'q'`.

Deferred (still open):

| Threads | What races | Effect |
|---|---|---|
| CPU ↔ interrupt timer | `mCPSR_I`/`mCPSR_F` read by the timer thread | delays an interrupt; `SetCPSR` checks again |
| CPU ↔ interrupt timer | interrupt controller registers: `mFIQMask` written by the CPU, `mIntRaised` written by `FireTimersAndFindNext` and read by `GetIntRaised` without the lock | |
| CPU ↔ TCP serial | DMA registers (`WriteDMARegister` vs. `HandleDMA`) | |
| CPU ↔ CoreAudio | `TCircleBuffer` positions (`OutputIsRunning` vs. `Consume`) | |
| UI ↔ CPU | tablet state (`PenDown`/`PenUp` vs. `GetTabletState`/`GetSample`) | |

Not covered by any run: the other serial drivers, network, PCMCIA.

**To clarify**
- **Q7.1** Is "clean under ThreadSanitizer for scenarios X, Y, Z" the definition
  of done?
- **Q7.2** Rule to adopt: only the emulator thread changes emulated state, and
  other threads post events? That is cleaner, but changes the drivers.
- **Q7.3** Speed budget: no atomics or locks in the JIT's hot loop?
- **Q7.4** Windows: shared threading code, or keep a separate implementation?

## Driver implementation

A bunch of drivers run in separate threads. We need to verify the implementation
which feels clumsy under Linux/macOS. This also needs to be compatible with the
Fast Start issue from earlier in the list.

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
- For Fast start, every driver needs to pause and resume. Only the TCP client
  implements `Suspend()/Resume()` so far (Fast start next step 4).

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
  - the fast start file (`FastStart.state` in the user data folder);
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

If we have multiple configurations, this would just be a fun project, so
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
