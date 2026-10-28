

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
| Multiple Configurations       | M–L (8–9 days, parallel processes included) | medium | –                       | done (steps 1–7, 2026-10-24); later items open |
| IR emulation via UDP          | M–L, after a 2–3 day investigation        | low        | Multiple Configurations        | open |
| Event forwarding on slow machines | not assessed                          | –          | –                              | open |
| Usermode network              | M (about 4 days, plus 1–2 for the driver) | medium     | –                              | N1, A1, B1, N2 done (2026-10-28); next A2 |
| Fix CI testing                | –                                         | –          | –                              | done |

Suggested order (to be decided):
1. ~~Sleep mode, then a ThreadSanitizer pass (Multithreading).~~ Done as part of Fast start.
2. Wrap up Fast start (review findings, see *Next steps* there), then Drivers.
3. SDL and Android.
4. Multiple Configurations, then IR.

Before Multiple Configurations (Matt, 2026-10-23): the time patches for the
other ROMs (*Time base*, open points), then the serial fixups (*fixups*).

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
- Every quit puts the Newton to sleep first (since 2026-10-23, also with
  Fast start off). Only when the Newton does not fall asleep in 10 s, quit
  just stops the CPU (`TFLApp::QuitNow`).
- Flash is a memory-mapped file that is synced on write and erase. If Einstein
  quits in the middle of a store operation, the store can be left inconsistent.
  Waiting for sleep makes that unlikely, but only with Fast start on.
- The power switch is `SendPowerSwitchEvent()`, and the platform manager knows
  the power state (`IsPowerOn()`, atomic since the Fast start work).

**To clarify**
- **Q3.1** On quit: put the Newton to sleep first and wait until it is asleep?
  How long until we force the quit? What if NewtonOS shows a dialog or hangs?
  **A** (partly): with Fast start on, sleep first, 10 s timeout, then quit
  without a state file. Also with Fast start off (2026-10-23): Matt found
  that a quit without sleep loses edits on the MP2x00 too, not only on the
  eMate, so every quit sleeps first; only the state file depends on the
  setting.
  **eMate** (Matt, 2026-10-23): with Fast start off, changes made on an
  eMate are lost after quitting, and it sometimes asks to calibrate the
  screen. Findings (Claude): `TFlashStore::Sleep` and `Idle` are empty in
  the MP2x00 ROM, so the store writes flash at once, and the flash file is
  a shared memory map that survives a quit. More likely, apps keep edited
  entries in memory and save them at sleep or when they close, so a quit
  without sleep loses them, on a MessagePad too (Matt: yes, the MP2x00
  loses them as well). Fixed: sleep first on every quit, the state file
  only with Fast start on (`TFLApp::StartSavingFastStartState`). The
  calibration question is skipped on the German, eMate and Watson ROMs too
  now (`CheckTabletCalibration`, German 0x00142E4C, eMate 0x00133760,
  Watson 0x00141D1C, found by matching the instruction words).
  Matt, after the fix: text in Notes survives sleep and a restart, text in
  Works does not; pressing Overview twice saves it. Not important for now.
- **Q3.2** Should closing the window mean sleep instead of quit?
- **Q3.3** Should FLTK, Cocoa and SDL behave the same?
- **Q3.4** How do we test that the store survives a quit? For example, quit
  repeatedly during heavy store activity.

### Fast start

After falling asleep and quitting Einstein, restarting Einstein should bring
up the emulator where it left off. There is an API to save the current emulation
state, but I could never get that to run correctly. We need to find the bugs
and missing states and make a fast start the default.

**Status (2026-10-11):** Done and on by default in the FLTK app, on branch
`FastWakeup`, [PR #225](https://github.com/pguyot/Einstein/pull/225) against
pguyot/Einstein. Four review rounds (CodeRabbit, Greptile), all fixed
(commits below): missing checks when reading damaged state files, a crash
when the screen size changes, two races, serial workers that were not
suspended; then oversized files at launch, a refused value left behind,
blocking serial writes under the DMA mutex; then a failed restore that had
already replaced flash and cards, ATA write errors, the Brain Wipe flag;
then linear card write errors. The last review found nothing more. Testing also
found that no platform event reached NewtonOS after a Hardware Reset or a
NewtonScript Reboot (fixed). The investigation notes this work
started from (bug list, missing-state table, step-by-step logs) are in the
history of this file: `git show d230e8a7:CLAUDE.md`.

**Done: a rotated screen (Matt, 2026-10-18; done 2026-10-23, `0235c869`).** When the emulated NewtonOS
rotates the screen, a fast start doesn't remember the rotation and starts
un-rotated: the screen shows the image's lines interleaved. Not unexpected:
Einstein has code for rotating the screen, but the FLTK app has no setting
for it, and it was never tested. To do: keep the rotation in the state file
(the screen's part of `TransferState`), restore it before the first frame,
and give the FLTK app what rotation needs (window size, tablet
coordinates), then test it with a rotated NewtonOS.

Matt, 2026-10-23: users rotate the screen with a NewtonOS menu, and Einstein
must keep that wish: every later start of that machine (fast start or not)
comes up in the same rotation. The default rotation for an eMate is
landscape.

*Findings* (Claude, 2026-10-23):
- The rotation *is* in the state file (`mScreenOrientation`,
  `mTabletOrientation` in `TScreenManager::TransferState`), but loading sets
  the value without calling `ScreenOrientationChanged()`. The FLTK window and
  its RGB buffer stay portrait (320 wide) while `UpdateScreenRect()` copies
  rows of `GetScreenWidth()` (480): the interleaved lines. "Restore From..."
  a landscape checkpoint while portrait has the same bug.
- A normal boot only works by luck: `TNotebook::InitToolbox` (0x00146B28)
  reads the user preference `screenOrientation` and calls
  `SetOrientation()` (0x00202B3C), which reaches Einstein as display driver
  `SetFeature(4)` and resizes the window after the boot started in portrait.
  Without that preference (new or wiped store) NewtonOS takes
  `GetGrafInfo(4)` = display driver `GetFeature(4)` = Einstein's
  `mScreenOrientation`, which is always `kOrientation_AppleBottom` (2) at
  launch.
- `GetScreenInfo` already reports the size from the orientation, so starting
  the emulator in landscape gives NewtonOS a landscape screen from the first
  instruction.
- Orientation values (TCocoaScreenGlue.mm): 0 AppleTop (portrait), 1
  AppleRight (landscape, the eMate's normal), 2 AppleBottom (portrait flip,
  the MP2x00's normal), 3 AppleLeft (landscape flip).
- `TFLScreenManager::ScreenOrientationChanged()` frees and reallocates the
  RGB buffer on the emulator thread while the UI thread may draw it (no
  lock), and passes the new size through `static` variables.
- The FLTK app ignores `TabletOrientationChanged()`; windowed pen
  coordinates seem to work today, not checked in all four rotations.

*Plan* (proposed):
1. After loading a state, call `ScreenOrientationChanged()` if the loaded
   rotation differs from the current one. Fixes fast start and Restore From.
2. Remember the rotation in the preferences (`Screen/Orientation`, per
   machine once machines exist), updated whenever NewtonOS rotates, saved at
   quit.
3. Launch in the remembered rotation: `TScreenManager` starts with it,
   `InitScreen()` opens the window with width and height swapped, the RGB
   buffer and `mScreenWasLandscape` match. A store with its own
   `screenOrientation` preference still wins, as on a real Newton.
4. eMate: without a remembered rotation, an eMate ROM starts in landscape
   (1). Matt: today an eMate ends in chaos with 320x480 and with 480x320
   alike. Proposed: the Settings show "MP2x00: 320 x 480 (portrait)" and
   "eMate 300: 480 x 320 (landscape)" under the size fields; a size wider
   than high means "start in landscape", and Einstein keeps the portrait
   size internally (`TScreenManager`, the state file's identity check), so
   both entries give an eMate the same screen. Test first what the eMate
   ROM does with `GetFeature(4)` = 1.
5. Resize the RGB buffer under `Fl::lock()`, without the `static` size
   variables.
6. Test: rotate, quit, fast start; the same with fast start off; restore a
   landscape checkpoint while portrait; a new eMate flash; taps in all four
   rotations.

Decisions (Matt, 2026-10-23): remember the rotation silently, no setting;
a fresh eMate starts in landscape; explain the screen size per model under
the size fields.

Tested (Matt, 2026-10-23): step 1 works, a rotated MP2x00 fast-starts
rotated. The eMate ROM started portrait and rotated to landscape itself
before booting. Steps 2–4 are in the code (untested): `Screen/Orientation`
and `Screen/OrientationROM` are saved at quit, the next start with the same
ROM uses them (`TFLApp::InitScreen()`, `TFLScreenManager` takes the
orientation); otherwise an eMate ROM or a size wider than high starts in
landscape (AppleRight), with the portrait size given to NewtonOS. Matt:
"everything else about rotation works well". Then: the size hint under the
fields ("MessagePad: 320 x 480 (portrait)", "eMate 300: 480 x 320
(landscape)"; the checkboxes below moved down 30 px), and step 5:
`TFLScreenManager::ScreenOrientationChanged()` replaces the image buffer
under `Fl::lock()` and hands the new widget size to the UI thread in
members instead of `static` variables. Tested (Matt, 2026-10-23): taps
work in all four rotations and in full screen. Rotation per machine
follows with Multiple Configurations.

**How it works**
- *State file* (`TEmulator::SaveState`/`LoadState`, file version 14): header
  (`EINI`, `SNAP`, version, kind), identity block, the inserted cards (tag and
  image path per socket), then the `TransferState` tree: memory (RAM,
  breakpoints, MMU, flash), CPU with native primitives, interrupts, DMA, serial
  DMA registers, PCMCIA controllers and cards, platform manager, screen and
  tablet, sound; last the storage, only in debug files (PC Card contents,
  then flash), so a file that fails is refused before the user's storage is
  replaced. The state is collected in a `TMemoryStream` and written with
  a CRC-32C at the end to `<path>.tmp` (every write and the close checked),
  then renamed.
- *Reading* reads the whole file and checks its size and CRC before anything
  changes, so a damaged or cut-off file is refused. Values that become sizes,
  counts or indices are checked as well (`TStream::CheckLimit()`,
  `CheckDataSize()`), in case a file was edited by hand: paths 2048 bytes, card
  state blocks 64 MB + 4 KB, ATA FIFO one sector, 65536 events, 1024 buffers of
  at most 64 MB, 65536 breakpoints. The CRC costs about 8 ms per fast start
  file and 24 ms per debug file.
- *Two kinds:* fast start files store only a flash checksum (4.3 MB), debug
  files store the whole flash (12.7 MB). The ROM is never saved; ROM
  breakpoints are removed before loading and re-applied from the loaded list.
  Saving takes about 0.1 s (fast start) and 0.2 s (debug) in a debug build.
- *Identity check* before anything changes: ROM+REX checksum, RAM size, flash
  checksum (fast start files only), screen size, card tag, contents size and
  contents checksum per socket, and a CRC of the Einstein version
  (`PROJECT_VER`; fast start files only, so a new Einstein boots normally
  even if the file format did not change, while checkpoints still load into
  another version); a read-only card whose contents would change
  is refused too. Every mismatch is printed ("the flash changed", …). The
  screen size and the RAM size are configuration and are never loaded.
- *One tree for save, load and reset:* a `TStream` reads, writes or resets
  (`TResetStream`). `TransferXxx(value, resetValue)` gives the power-on value;
  calls without one keep the current value (RAM, flash, configuration). The
  follow-ups after loading (JIT flush with `TJITCache::InvalidateAll()`, screen
  power, serial wake-up) also run after a reset. Card state is a tagged block
  with a length (`TMemoryStream`), so loading with a different card skips it.
- *Threads:* `TMonitor::RequestStateTransfer` does the transfer on the UI
  thread if the emulator is halted, otherwise it hands the request to the
  monitor thread and stops the emulator. A small mutex around the request and
  `mHalted`, and a stop counter in `TEmulator` (`Run(inStopCount)`), make sure
  no request gets lost while the monitor starts or stops the emulator. All
  serial workers are suspended during the walk (`mDMAMutex` in
  `TBasicSerialPortManager`), the interrupt manager holds its mutex.
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
| Review | `5da9e480` | Screen size in the identity block (version 10) |
| Review | `aa89a161` | CRC-32C, checked writes and close, whole file checked before loading (version 11) |
| Review | `064df478` | Limits for sizes, counts and indices read from a file |
| Review | `5127ddb9` | No lost state requests while the monitor starts or stops the emulator |
| – | `15504e8c` | Platform events delivered again after a Hardware Reset |
| Review | `f2308217` | Suspend the PTY, Pipes and BasiliskII workers too |
| Review | `8c6a1473` | Message for a state file of another version |
| – | `63aaed80` | Platform events delivered again after a NewtonScript Reboot |
| Review 2 | `fee76444` | Files over 256 MB or without `EINISNAP` refused before allocating |
| Review 2 | `6f4f966c` | Values checked before they replace the current ones (breakpoints, pen positions, event and buffer sizes) |
| Review 2 | `a5f867bd` | Non-blocking serial writes for BasiliskII and TCP; TCP connects outside the DMA mutex |
| Review 3 | `58a1fcf3` | Storage (card contents, flash) last in the file; card size and writability checked up front (version 13) |
| Review 3 | `1554a7ce` | Brain Wipe through the reset request |
| Review 4 | `d5bdbbba` | Linear cards write restored contents at once and refuse read-only images |
| – | `6916503a` | Einstein version in the identity block, fast start files only load into the same version (version 14) |

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
- The file CRC is CRC-32C (Castagnoli, `K/Misc/CRC32`), not zlib's CRC-32.
  A script that edits a state file must recompute it the same way.
- Races between threads were forced with temporary delays (`sleep_for`
  behind an environment variable) in the window, not with lldb breakpoints:
  a thread stopped at an lldb breakpoint stayed suspended after an expression
  ran on another thread.

**Next steps to wrap up state snapshots**

*Before merging PR #225*
1. ~~Review findings 1–6~~ done 2026-10-10 (commits above): screen size in
   the identity block; CRC and limits for damaged or edited files (Matt:
   paths ≤ 2048 bytes, linear cards ≤ 64 MB + CIS, CRC if it is fast enough);
   no lost Monitor requests (both windows reproduced with delays, then fixed);
   all serial workers suspended; checked close; the version message.
2. ~~Push and answer the reviews~~ done 2026-10-11 (four rounds, last push
   `d5bdbbba`). Codacy reports markdownlint style issues in the Markdown
   files and one generic flawfinder `strlen` warning (a NUL-terminated path);
   not changed.

*After merging*
3. ~~NewtonScript Reboot blocks all later platform events~~ fixed
   2026-10-10 (`63aaed80`, older than this branch). `Reboot()` arrives as a
   platform event, and NewtonOS reboots before it answers, so
   `mQueueLockCount` stayed at 1. The native `TMainPlatformDriver::Init`,
   called once per boot before `PowerOnDeviceCheck`, now resets the queue
   locks as at launch. No REX change was needed.
4. ~~Save the fast start file on the monitor thread~~ done (`b5b9fc5e`):
   `RequestSaveState(path, fastStart, haltAfter)`; the monitor stops, saves
   and stays halted, the UI waits for `IsStateRequestPending()` to clear.
5. ~~Reset after a load that fails halfway~~ done (`542301af`): RAM cleared,
   `ResetState()`, NewtonOS boots again. Refused files still change nothing.
6. ~~Card contents in debug files~~ done (`e97f9d2d`, `06e97e16`, `98fee640`,
   file version 12). Matt: yes, but ask before overwriting, because cards may
   be shared between emulated Newtons. Changed contents are only replaced
   with `LoadState(path, true)`; the Monitor has `load overwrite <path>`,
   Load State asks (Cancel / Overwrite Card).
7. **Other front ends** (Matt, 2026-10-11, Q3.3):
   - Cocoa is not maintained by Matt: it must still compile, but does not get
     Fast start or checkpoints (it can't easily be debugged). Checked
     2026-10-11 after `ba2826ff`: builds, Xcode tests pass.
   - SDL is work in progress: add Fast start when SDL development restarts;
     it would improve usability a lot (Android can kill the app any time).
8. ~~Checkpoints~~ done (`9bbbff7c`, `ba2826ff`, Matt's idea). File menu:
   "Save State" (Cmd+Shift+S), "Save State As...", "Restore Last",
   "Restore From...". A checkpoint is a debug state file (memory, flash, PC
   Card contents); the last one saved or restored is remembered
   (`System/StatePath`, default `Einstein.state` in the user data folder).
   Restoring asks first (file, date, "memory and internal storage (flash)
   are replaced", PC Cards named); errors show their reason
   (`TEmulator::GetLastStateError()`). Wording chosen by Claude; Matt to
   review. Not tested by hand yet: the dialogs and the file choosers.
13. **Einstein Prefs** is built into the REx since `54927697`
    (`Drivers/EinsteinPrefs`, precompiled package; source to follow). Matt
    changed the author part of the app symbol, so the built-in app and a copy
    installed in the flash can coexist (with the same symbol, neither
    appeared, and only a Brain Wipe brought the app back; a Hardware Reset
    was not enough). The package name also had to change
    (`EinsteinPrefs:EINSTEIN`, was `EinsteinPrefs:MATTWM`, `010c1323`):
    with the same package name, NewtonOS reported a conflict at every start.
    Next version (Matt): not a separate app, but a new item in the built-in
    Newton preferences.
12. **Documentation** (Matt, 2026-10-11): write new release notes next, and
    refresh the documentation, maybe with a help page. Release notes drafted
    in `ReleaseNotes.md` and `ReleaseText.md` (version 2026.10.11, tag
    `v2026.10.11rc1`); also added the v2026.10.6 text that was only on
    GitHub. User guide: `Documentation/UserGuide.md` (Matt chose Markdown in
    the repository; `4c16c77a`), following Matt's outline in TFLApp.cpp;
    Help › User Manual opens it on GitHub (`94a12134`). The old LaTeX manual
    is in `Documentation/Historic`. Open points, marked `TODO(Matt)` in the
    guide: Y2K10 patch for the US ROM, NewtTest, Dock programs, Internet
    Setup, dumping physical cards. Screenshots would help.
9. ~~Block-wise Int32 arrays~~ done (`5506122b`), same bytes. Less gain than
   expected: debug file save 0.18 → 0.13 s, load 0.13 s, fast start 0.07 s
   (debug build); the rest is checksums and file I/O.
10. ~~Open questions~~ answered (Matt, 2026-10-11). Q4.5 and Q4.7 need no
    change. Q4.4: findings below.
11. ~~**The RTC alarm interrupt**~~ Done 2026-10-23, tested by Matt.
    `TInterruptManager::FireTimersAndFindNext()` raises 0x4 once when the
    RTC reaches the alarm register. It is armed by a write of a future
    time (NewtonOS also writes past times and fires those itself), and
    after loading a state (not saved). The timer thread checks the clock
    when the alarm is due by its tick count, at most a minute apart; it
    only waits with a timeout for the alarm while the emulator waits for an
    interrupt (a timed wait on every turn made the emulation slow), and it
    resumes a sleeping Newton if the alarm bit is in the wake-up mask
    (0x0F184800). Needed the RTC in UTC first (see *Time base*).
    **Was not emulated** (found 2026-10-11, older than
    this branch). NewtonOS's `TRealTimeClock::SetAlarm` ends in
    `SetRealTimeClockAlarm`, which writes the alarm register (0x0F181400) and
    enables interrupt 0x4. Einstein stores the value (`mAlarmRegister`) but
    never compares it with the clock. A real Newton uses this alarm to wake
    from sleep (`CyclePower` checks `TRealTimeClock::SleepingCheckFire`).
    Idea: fire once when the clock reaches the register, and save an "armed"
    flag, so an alarm that passed while Einstein was quit fires right after a
    fast start, as a sleeping Newton would have woken for it. **A** (Matt,
    2026-10-11): there are no alarms in Einstein FLTK today (Cocoa not
    checked, none in SDL); nice to have later. The alarm would be checked by
    the emulator's timer thread (`TInterruptManager`), not by an FLTK timer,
    so long alarms work as long as Einstein runs.

**To clarify**
- **Q4.1** Is "snapshot only when asleep" acceptable? **A:** Yes, for fast
  start (decided 2026-10-08). Debug snapshots are taken while the Monitor is
  halted.
- **Q4.2** When is a snapshot invalid? **A:** When the file version, the
  ROM+REX, the RAM size, the screen size, the flash (fast start files), the
  Einstein version (fast start files) or a card changed. Einstein then boots normally and prints the reason on the
  console.
- **Q4.3** What happens with a corrupt snapshot? **A:** The CRC refuses it
  before anything changes. Fast start: the file is deleted and Einstein boots
  normally. Debug files: the Monitor reports it, the emulator keeps running.
- **Q4.4** Clock jump after a restore: is the host-time patch enough?
  **A:** I have to test that. I wonder how the original ROM handles that. I
  doubt that the CPU registers are kept alive when the device is in the deepest
  sleep, so after waking up, the CPU jumps to address 0 and - I assume -
  recognizes that the RAM contents is still valid and continues with the stored
  registers instead of cold booting. I assume that's where the real time clock
  and the alarm are adjusted. Can we check that and do it the same way?
  **Findings** (Claude, 2026-10-11, ROM disassembly):
  - The guess is right for the CPU: on a StrongARM, `TVoyagerPlatform::
    PowerOffSystem` calls `SaveCPUStateAndStopSystem` (0x00018F48). It saves
    all banked registers, the MMU registers and some Voyager registers
    (including the interrupt controller) at `gParamBlockFromImagePhysical`,
    with a checksum and a valid flag, and stops. After the reset the boot code
    checks the checksum, restores everything and returns into
    `PowerOffSystem`.
  - Nothing adjusts the clock, neither there nor in
    `TVoyagerPlatform::PowerOnSystem` (it only finds the wake reason) nor in
    `CyclePower()` (0x00192764), which loops `PowerOffSystem`/`PowerOnSystem`
    and goes back to sleep after an RTC alarm wake-up unless
    `TRealTimeClock::SleepingCheckFire` says an alarm is due. Time simply
    continues because the RTC hardware keeps counting during sleep.
  - Einstein does the same in effect: its REX platform driver replaces
    `TVoyagerPlatform`, and `PowerOffSystem` waits in `PauseSystem` instead
    of resetting, so no CPU save is needed. The time of day comes from the
    host (patched `RealClockSeconds` and friends), so it is right after a
    fast start. Tested: quit, 3.5 minutes later fast start, the Newton shows
    the current time.
  - The 3.6864 MHz timer does not jump: whenever the timer thread resumes, it
    sets `mTimerDelta = ticks - mTimer`, so the Newton timer continues where
    it stopped, after a pause and after a fast start alike (corrected
    2026-10-11; an earlier note here said it jumps by the time Einstein was
    quit).
  - What does not happen the same way: the RTC alarm (next step 11).
- **Q4.5** Should external connections (TCP serial, network) reconnect on their
  own?
  **A:** No. We are emulating that a person switches off their device and
  restarts it much later, maybe even after moving the device. Serial port
  and Network connection would be lost anyway.
- **Q4.6** Where is the snapshot stored, and is there one per configuration?
  **A:** `FastStart.state` in the user data folder, one per installation. One
  per configuration once Multiple Configurations exist.
- **Q4.7** Should a "cold boot" option stay available in the UI? Today: turn
  the setting off, or use the Reset menu after a fast start. Is a way to skip
  one fast start at launch needed?
  **A:** What we currently have is sufficient.

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

Fixed 2026-10-10 (`15504e8c`): the reset locks the platform event queue until
NewtonOS has booted, as at launch, but only the first `PowerOnDeviceCheck`
per process released that lock (a `static` counter). After a Hardware Reset
or Brain Wipe no platform event reached NewtonOS any more (power button,
keyboard, packages). Now every `PowerOnDeviceCheck` releases it.

**To clarify**
- **Q4.8** Does Hardware Reset actually lose the store? **A4.8** (test,
  2026-10-07) Not in the idle case, before and after the rewrite. Not tested
  yet: a reset during store activity, sync or active DMA.
  **Found 2026-10-12:** a Monitor breakpoint in ROM, then a Hardware Reset
  (or any reboot), erased the store ("The internal store was erased because
  a different ROM has been installed"). NewtonOS keeps the ROM and REx
  checksums in the flash; `TFlash` writes those of the patched image at
  launch (so adding or removing patches is safe), but breakpoints change ROM
  words later. Fixed in `21db6589`/`886da2e7`: a native patch on
  `OSCalibrationParameters::CalculateROMREXCheckSums` returns the launch-time
  checksums of the ROM and REx segments, on all four ROMs.
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
- For Fast start, every driver needs to pause and resume. Since `f2308217`,
  `TBasicSerialPortManager` implements `Suspend()/Resume()` with `mDMAMutex`,
  and all four worker threads hold it while they touch DMA. The PTY driver
  only starts if `/tmp/BBridge.Einstein` exists (hard-coded path).

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
- Seen with Newton Connection (Matt, 2026-10-18): switching one installation
  between a MessagePad 2000 setup and an eMate 300 setup keeps the one Newton
  ID, so the desktop takes both for the same device. Newton Connection then
  backs up both into one file (`0000-4E65-7774-6F6E.newtondevice`), soups of
  the two mixed; the eMate's clock read 1996 (its NewtonOS not patched for
  the date bug), the MessagePad's 2026, so even the backup times of the
  soups mix. Each configuration needs its own Newton ID (Q9.4: yes), as
  well as its own flash file and fast start file.

**Decisions** (Matt, 2026-10-23)
- **Q9.1** Machines run as separate processes, one per machine. Several
  machines may run at the same time, each with its own Dock icon and menu
  bar ("that's actually pretty great"). No several emulators in one process.
- **Q9.2** One configuration per machine (no separate "MessagePad" and "User
  Interface" configurations; the disabled placeholder widgets on the
  MessagePad tab go away). Per machine: ROM, built-in REx, flash, RAM, screen
  size, scale, rotation, full screen, window position, fast start on/off,
  state path, serial driver and TCP server, cards kept in a slot, Newton ID.
  Shared: "don't show at startup", menubar and toolbar, the Resources tab,
  the list of known PC Cards (only the slot assignment is per machine).
- **Q9.3** A machine list in the Settings dialog (below), and
  `--machine <uuid>` on the command line.
- **Q9.4** Yes, each machine gets its own Newton ID.
- **Q9.5** Yes. Storage: one folder per machine, with a fallback for the
  default machine: the first machine in the list stays in the old
  preferences keys and file locations, so an older Einstein still launches
  it.
- **Q9.6** FLTK only for now (as for Fast start, Q3.3).
- ~~Switching to another machine relaunches the process~~ Superseded: play
  starts the other machine in a new process (round 2 below). No in-process
  restart of the emulator.
- Port paths that two processes would share (`/tmp/einstein-*.pty`,
  `/tmp/BBridge.Einstein`, `/tmp/pty.BasiliskII`, TCP ports): the first
  process wins, the other one can't open it.
- Only one Settings window may be open at a time, also across processes.
  That also protects the preferences from being written by two processes.
- Nice to have: a custom Dock icon per machine (and the equivalent on Windows
  and Linux, if FLTK can do it; `Fl_Window::icon()` covers window icons on
  Windows and X11, macOS probably needs `[NSApp setApplicationIconImage:]`).

**Decisions, round 2** (Matt, 2026-10-23, after a UTM screenshot)
- Layout like UTM: the machine list on the left (icon, name in bold, one
  line of description, a play button per row, a context menu per row, "+"
  to add), the existing tabs on the right for the selected machine.
- Start-up: the Settings window with the list; play starts the machine in
  this process and closes the window. With "Don't show at startup", the
  last used machine starts at once. In a running Einstein, play on another
  machine starts it in a new process (own window and Dock icon).
- The first machine in the list is always also stored in the old global
  preference keys, so an older Einstein launches it (downgrade). When the
  first machine is moved or deleted, the new first one is copied there.
  So no machine is special: any machine can be deleted, at least one stays.
- The sidebar is collapsible, with a [<<] button at the bottom left under
  the tabs; the choice is remembered.
- Icons are chosen automatically from the ROM: MessagePad and eMate for
  now, as SVG (Matt can draw them if needed).
- Context menu: Start, Duplicate, Rename, Show in Finder, Delete (asks:
  only the entry, or the files too). No presets: the ROM sets the
  rotation; a new machine gets 4 MB RAM and 4 MB flash.
- The list shows machines that run in other processes; it can't stop them.
  The button must go back to play when that process quits.
- A PC Card image is locked while inserted; another machine can't insert
  it and names the machine that has it. The network card needs no lock.
- If the Settings are open in another process, a message says so. A pipe
  or broadcast to bring that window forward can come later.
- IPC: lock files for now (below). Sockets (`AF_UNIX`, all three
  platforms) only when messages between processes are needed: bring a
  window forward, quit, a setting changed (about 1–2 days).

**Plan** (Claude, 2026-10-23, revised)

*Today:* `TFLApp::Run()` creates exactly one emulator per process. The
preferences are one flat group, opened in `TFLSettings::loadPreferences()`
and in `DataFilePath()` (TFLApp.cpp, the paths of `FastStart.state` and
`Einstein.state`). The Newton ID is the constant `0x4E65 77746F6E`
("Newton", TEmulator.cpp:74); FLTK never calls `TEmulator::SetNewtonID()`.
The user-mode network has a fixed MAC (`58:b0:35:77:d7:22`,
TUsermodeNetwork.cpp), private to each process, so no collision; a TAP
network would need one per machine (could come from the Newton ID).

*Storage:*
```
<user data>/Machines/<uuid>/
    Machine.prefs       per-machine settings, Newton ID, window position
    Machine.lock        held by the process that runs the machine
    internal.flash
    FastStart.state
    Einstein.state      (default checkpoint)
<user data>/Settings.lock   held while a Settings window is open
```
The main preferences hold the machine list (order, last used, sidebar open)
and the shared settings, plus the first machine's settings in the old keys.
The migrated machine keeps its files where they are today (the paths are
stored) and its Newton ID `4E65-77746F6E`, so existing backups in Newton
Connection and NCX still match it. New and duplicated machines get a random
48-bit Newton ID.

*Locks:* `flock()` (macOS, Linux) or `LockFileEx()` (Windows); the OS
releases a lock when its process ends, also after a crash. The list tries
each machine's lock about once a second (`Fl::add_timeout`, non-blocking)
to show which machines run. A machine that runs in another process is
read-only in the Settings ("running in another window"), because it writes
its settings at quit (Matt, 2026-10-23). No IPC for now; file locks are
enough. Any machine can be deleted, at least one stays.

*Steps:*

| Step | What | Days |
|---|---|---|
| 1 | ~~Split `TFLSettings` into app settings and `TFLMachineSettings`~~ done, `d6b4d459`; Matt tested it, also with an older Einstein | 1 |
| 2 | ~~Machine folders, machine list, the first machine mirrored in the old keys~~ done, `4d6d08ae`, tested by Matt ( `LoadMachines()`, `SaveMachines()`; the first machine is also read from the old keys, so changes made by an older Einstein win) | 1 |
| 3 | ~~Per-machine paths (flash, fast start, checkpoint), Newton ID via `SetNewtonID()`, `--machine <uuid>`~~ done, `0dd9352a`, tested by Matt ( `FastStart.state`, `Einstein.state` and `EinsteinCheck` in the machine's folder; `System/NewtonID` as 16 hex digits, the migrated machine keeps `00004E6577746F6E`; `--machine` takes the UUID or the name and skips the Settings) | 1 |
| 4 | Locks: machine lock, Settings lock, PC Card image lock (4a–c in the code, untested: `TFLFileLock`, `Machine.lock` per machine folder, `Settings.lock` in the user data folder; a quitting Einstein saves only its machine, `SaveRunningMachine()`; the Settings skip machines that run elsewhere. 4a–c tested by Matt, `a8b2a7ea`, except the Settings lock across two Einsteins. 4d in the code, untested: `Cards/<card uuid>.lock` with the machine's name while a card is inserted, not for the network card; the slots per machine in `PCMCIA/KeepInSlot0/1` of the machine file; the first machine's slots also in each card's old `keepInSlot`, written and read like the old keys) | 1 |
| 5 | The list: rows with icon, name, description, play button; context menu; [<<] (done, `ee463d6f` and `5e6bc6ff`, tested by Matt: the dialog edits `mEditMachine`; `TFLMachineList`, an `Fl_Scroll` with an `Fl_Pack` of row groups (Matt's suggestion); list changes are saved at once, tab changes with Save or Start; `ROM/ID` per machine for the icon and description. 5d in the code, untested: a timer shows running machines (green dot, back to play when that Einstein quits) and reloads a machine that stopped; a machine that runs elsewhere is read-only (`updateEditable()`); opening the Settings while running reads the list again, `RefreshMachines()`) | 2 |
| 6 | Start a machine in a new process (macOS bundle, Windows, Linux); machine name in window and Monitor titles (done, `740d0408`, tested by Matt on macOS: `TFLApp::LaunchMachine()`, double `fork()`/`execl()` with all inherited files closed, `CreateProcessW()` without inherited handles on Windows; lock files with `O_CLOEXEC`; the Settings save before starting) | 1–1.5 |
| 7 | Tests (old Einstein on the new preferences, two machines at once, fast start per machine), user guide (user guide section 12 and release notes written; the tests were done by Matt along the steps) | 1 |

About 8–9 days. `--machine` also makes scripted tests easier.

*Later* (Matt, 2026-10-24, after testing 5a–c), done 2026-10-26
(`75fd6920` … `b1847f60`, steps 1–6 below):
- ~~The same flash file must not be used by two machines~~ The flash file
  is locked at launch (`Flash/<hash of the canonical path>.lock`); the
  Settings warn when another machine uses the same file and gray out Brain
  Wipe and Create while another Einstein has it.
- ~~A machine's default flash file is in its folder~~ ROM and flash each
  have a file in the machine folder (stored by name) and one on the host
  (full path); radio buttons "Keep (a copy) in the machine folder" / "Use
  the file where it is" choose, both are kept (Matt: no hidden copies or
  deletes). The old keys get the full path of the file in use. The machine
  made from the old settings keeps its files on the host (Matt, Q5).
- ~~A GUI element to go back to the default flash file~~ the radio button.
  In the folder, a machine has one flash (Create/Choose only for host
  files); Brain Wipe... deletes it and `FastStart.state`. ROM lights: red
  (none, missing), yellow (unknown), green (known). Choose... in folder
  mode offers Copy to Machine Folder or Use Where It Is.
- "+" › Add Existing Machine... adds a machine folder (copied into
  `Machines/<new uuid>/` unless it is there already; a new Newton ID if one
  in the list has the same). PC Cards stay shared (Matt, Q6).
- `System/StatePath` is stored by name for files in the machine folder.

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

## eMate Special Keys bar

(Matt, 2026-10-23.) The eMate 300 has 13 special keys above its keyboard, in
five groups: close, extras | dock, send, receive | scroll up, overview,
scroll down | undo, find, assist | light, power. Like the Toolbar, a "Keys"
option in the Settings (Show: Menubar, Toolbar, Keys) shows this bar under
the Newton screen. Look: dark green keys, darker green separators between
the groups, white icons and labels (scan: `~/dev/Newton/eMate Special
Keys.jpg`). Einstein maps most of them to Esc and the function keys of the
host keyboard.

Key codes (Matt, 2026-10-23): Esc close, F1 extras, F2 dock, F3 send, F4
receive, F5 scroll up, F6 overview, F7 scroll down, F8 undo, F9 find, F10
assist. Light and power are no keys for NewtonOS: the host's F11 and F12
become `UserActionToggleBacklight()` and `UserActionTogglePower()` (today
they are passed on as key codes 0x67 and 0x6F). The bar is clickable and
resizes with the window above it.

*Plan* (proposed):
1. ~~Find out which key code each special key sends.~~ Above. Check whether
   the MP2x00 ROM understands them too.
2. A widget `TFLEMateKeys` (one `Fl_Group`, keys drawn by hand, not 13
   fluid buttons): it scales with the window width (about 22 px per key at
   320 px, 33 px at 480 px), draws the icons as small built-in SVGs
   (`Fl_SVG_Image`, FLTK 1.4.5 has it) and sends `KeyDown`/`KeyUp` to the
   screen manager on press and release.
3. Layout: the bar is a fixed height below the screen; `InitScreen()`,
   `ResizeFromNewton()` (rotation), full screen and the window size range
   take it into account, as they do for the toolbar.
4. Setting `Screen/ShowSpecialKeys` (per machine once machines exist), in
   the Settings next to Menubar and Toolbar.

Decisions (Matt, 2026-10-23): the eMate docs call them "Special Keys"; on
by default for the eMate; the MP2x00 ROMs don't understand them, so the
checkbox is cleared and grayed out for other ROMs.

Status (2026-10-23, `ea834f0f`; Matt: "works very well", he changed the
key box style and colors): `TFLSpecialKeys`
(app/FLTK/TFLSpecialKeys.cpp) draws the keys itself with built-in SVG
icons, spreads them over its width (labels when they fit, icons only
otherwise), sends `KeyDown`/`KeyUp` on press and release (dragging off a
key lets it go), and calls the app for light and power. `InitScreen()`
puts it under the screen if the setting is on and the ROM is an eMate's;
its height is 40 px times the screen scale, its width follows the window.
The host's F11 and F12 toggle the backlight and the power, once per press.
Settings: "Special Keys (eMate)" under Toolbar; `updateSpecialKeysCheckbox()`
follows the ROM chosen in the dialog (`mDialogROMId`, set by
`GetROMDetails()`), and a grayed-out checkbox keeps the saved setting.

## Usermode network (TUsermodeNetwork)

(Matt, 2026-10-27, after the serial fixes.) Check the user-mode network for
bottlenecks, bad throttling, deadlocks, missing timeouts and other bugs.
The FLTK app only uses `TUsermodeNetwork` (or `TNullNetworkManager`);
`TTapNetwork` is only in the Cocoa app.

**How it works**
- Single-threaded and polled: every call comes from the NE2000 driver
  package (not part of the REx) (`NE2K.pkg`, source
  `Drivers/NE2000Driver/NE2000.cpp`) through the natives 0x0a00…0x0a15
  (`TNativePrimitives.cpp` around line 2970), on the emulator thread. A
  blocking socket call freezes the Newton.
- The driver's timer runs every 100 ms (`kTimerTime`). Each tick calls
  `EinsteinNetTimerExpired` (all handlers poll their sockets), then
  `EinsteinNetDataAvailable` and receives the waiting packets, at most 16
  (`kMaxRxPerTick`; one before B1). The card
  interrupt is not used for receiving (`InterruptHandler` only calls a
  native that does nothing).
- Handlers: DHCP and ARP (answered at once, fixed addresses), DNS to
  127.0.0.53 (answered with `gethostbyname`), TCP and UDP (one host socket
  per connection). Packets for the Newton wait in a FIFO.

**Findings** (Claude, 2026-10-27)

*Throttling*
- ~~`DataAvailable()` hands out a packet only every 0.25 s~~ (A1); with 100 ms ticks
  one packet per 300 ms, ACKs included. Added in 2021 (`cdab4332`). Matt: a
  proof of concept, when the emulation was slow and NewtonOS was easily
  overwhelmed, probably also because of handshake bugs; never redone.
- ~~The driver takes at most one packet per tick: 10 packets/s, about
  14 KB/s, at best.~~ (B1)
- TCP `timer()` reads the socket only when the whole FIFO is empty and then
  waits for the Newton's ACK of that one segment; every Newton segment gets
  its own ACK packet, which waits for its slot. Download and upload are
  about 4–5 KB/s today ("Courier is somewhat working now, but incredibly
  slow", Matt 2010, `dddd9e4e`).

*Blocking calls (freezes)*
- `TCPPacketHandler::connect()` waits in `select()` up to 10 s, again for
  every SYN the Newton retries.
- DNS: `gethostbyname()` blocks for the whole lookup (deprecated, not
  thread-safe).
- `TNetworkManager::Run()` holds `mSelectMutex` during a `select()` without
  timeout; `TTapNetwork::ReceiveData()` then blocks the emulator until the
  next packet (Cocoa only).

*TCP*
- `::send()` result ignored: on the non-blocking socket a partial send or
  `EAGAIN` loses data that the Newton was already ACKed for. No
  `MSG_NOSIGNAL`/`SO_NOSIGPIPE`: a send to a closed socket kills Einstein,
  unless the TCP serial driver happens to have set `SIGPIPE` to a handler.
- Sequence numbers are never checked: a Newton retransmission (likely with
  300 ms per packet) reaches the server twice.
- `recv()` errors other than "no data" and RST from the Newton are
  ignored; the connection stays forever.
- Any TCP packet that matches no handler (a stray ACK or RST) creates one
  and opens an outgoing connection (with the 10 s wait).
- A failed connect leaks its socket and keeps the handler; no RST to the
  Newton. No timeouts in the closing states.

*Parsing*
- TCP and UDP payload sizes come from the frame size, not the IP total
  length (padding would become data).
- No length checks before reading fields (DHCP reads offset 0x116 of any
  UDP packet to port 67, the DNS name parser reads past the packet).
- The DNS name parser can overflow its 128-byte stack buffer (the counter
  wraps, only the inner loop stops); the type check uses `&&` instead of
  `||`, so AAAA queries get an A answer.
- Natives: `TimerExpired` (0x11) has no null check; `GetDeviceAddress`
  copies `size` bytes from a 6-byte array.

**Plan** (proposed)

| Step | What | Days |
|---|---|---|
| A1 | ~~Remove the 0.25 s throttle; a newer pure ACK of a connection replaces a queued one~~ done 2026-10-28 | 0.5 |
| A2 | Non-blocking connect: "connecting" state checked in `timer()`, SYN-ACK when connected, RST on failure or after 10 s | 0.5 |
| A3 | Send buffer per connection: keep what `send()` did not take, flush on the next ticks, ACK only what the host accepted; `MSG_NOSIGNAL`/`SO_NOSIGPIPE` | 0.5–1 |
| A4 | Sequence checks (re-ACK retransmissions, drop out-of-order), RST both ways, RST for unknown packets instead of connecting, timeouts for the closing states | 1 |
| A5 | DNS in a worker thread (`getaddrinfo`), replies queued and delivered in `TimerExpired()`; fix the name parser | 0.5–1 |
| A6 | Length checks, payload sizes from the IP header, the two natives | 0.5 |
| N1 | NE2K.pkg built by the `Drivers` CMake (gcc, `ELFtoPKG`, the NewtonScript part added by `NE2000Driver/AddNSPart.py`), installed to `_Data_/NE2K.pkg` | 1 |
| N2 | ~~NE2K.pkg embedded in Einstein; Install Essentials installs it from there instead of the server~~ done 2026-10-28 | 0.5 |
| B1 | ~~NE2K.pkg receives all queued packets per tick~~ done 2026-10-28 | 0.5 |
| B2 | NE2K.pkg also receives on the card interrupt; first find out whether NewtonOS passes the card's IREQ to Lantern's `InterruptHandler` (`TPCMCIAController::RaiseInterrupt()` exists) | 0.5–1 |
| N3 | Last: NE2K.pkg in the REx? First test what it does to an installed NE2K.pkg and to a NewtonOS without NIE (Matt, 2026-10-27) | ? |

A1–A6 change only the emulator and can be tested one by one (Courier,
Newt's Cape, Dock over TCP with NCX).

NE2K.pkg was built with its own MPW Makefile and NTK (1997/2010), never
with the `Drivers` CMake, which is why it was never maintained. Matt
(2026-10-27): build it with the `Drivers` CMake, and embed it in Einstein,
so the right version of the driver is always available, even without the
web site. Whether it belongs in the REx is tested last.

*N1 (Claude, 2026-10-27; tested by Matt: installs, DHCP, DNS, Courier loads a page):*
- `NE2000Driver/NE2000.cpp` (line endings converted, otherwise Apple's
  template with Einstein's natives), `NE2000.impl.s` (class info and
  branch table, written like `PMainSoundDriver.impl.s`), `NE2000Prims.s`
  (natives 0x0a00…0x0a15).
- Apple's `LanternDriver.proto.h` and `LanternDriverAPI.proto.h` are not in
  the public NCT_Projects (only in Matt's NewtonDev archive), so
  `NE2000Driver/LanternDriver.h` declares the two protocols, and
  `LanternDriverAPI.cpp` has the call glue as naked functions (`ldr r0,
  [r0,#4]; ldr r12, [r0,#8]; add pc, r12, #offset`), the same offsets
  as the glue in the NE2K.pkg of 1997 (checked). `fDriverAPI` is at offset
  24, as in the old code.
- Event handlers: cfront's `(DriverProcPtr) &TNE2000Sample::Method` is a
  gcc bound member function, `EVENT_HANDLER(method)`, with
  `-Wno-pmf-conversions`.
- The package: the NewtonScript part of 1997 (`NE2K.auto`, extracted from
  the old package; its source is `NE2000Driver/Install` and `Project
  Data`), then the protocol part from `ELFtoPKG`, put together by
  `NE2000Driver/AddNSPart.py`. Everything in a package counts from the
  start of the package: the references in a frame part, the offsets in the
  relocation data, and the values of the relocated words (here the GOT
  entries of the event handlers). So the NewtonScript part stays at offset
  408, where NTK put it (the directory is padded), and the script moves the
  relocation offsets and the relocated values of the protocol part. (Each of
  the three showed up as its own failure: "a problem has occurred",
  "Part 1, Type auto", a driver that stopped after `Enable`.)
- The MPW build files (`Makefile`, `Makeout`, `FilesInBuild`,
  `NE2000Prims.a`), the real card's `NE2000.h` and the old `NE2K.pkg` are
  removed. The new package is `_Data_/NE2K.pkg` (target `NE2K.pkg`,
  installed with the REx).

*A1 and B1 (Claude, 2026-10-28; tested by Matt: DHCP, DNS, Courier loads a page):*
- The driver hands Lantern all waiting packets per tick, at most 16
  (`kMaxRxPerTick`); a packet that gets no buffer from `NewPacketPtr()`
  waits for the next tick (the old code would have written to address 0).
- `DataAvailable()` has no throttle any more.
  `TUsermodeNetwork::EnqueueAck()`: the ACK for a Newton segment replaces a
  pure ACK of the same connection that still waits in the FIFO (ACK numbers
  are cumulative). Courier sends a request in three segments and gets one
  ACK.
- The trace showed every reply handed over in the tick it was queued; from
  DHCP to the answer of the server took about two seconds. The three
  seconds before the Newton's FIN are Courier's own, nothing waited.
- Still limited: TCP sends one segment and waits for its ACK, so a download
  gets one segment per tick (at most about 14 KB/s). Next ideas: a shorter
  tick (one line in the driver), several segments in flight (with A3/A4).
- Seen for A4: Einstein answers the Newton's FIN with an RST instead of an
  ACK (`kStatePeerDiscWaitForFIN`).

*N2 (Claude, 2026-10-28; tested by Matt):* CMake embeds `_Data_/NE2K.pkg`
as `gNE2KPackage` (`app/FLTK/TFLNE2KImage.cpp/.h`, generated like the REx
image, not in git). Install Essentials links starting with `E` are built
into Einstein (`expand()` in TFLSettingsUI.fl makes them `einstein:NE2K.pkg`),
and `TFLApp::InstallPackagesFromURI()` installs them from memory. The
messagepad.org download of NE2K.pkg is no longer used.

*Trace:* `EINSTEIN_NET_TRACE` set prints to stderr, with the time in seconds
(`TNetworkManager::Trace()`): every NE2000 native except `DataAvailable`
without a packet, every 10th timer tick, the packet sizes; in
`TUsermodeNetwork` the type of each Newton packet, DHCP request types,
queued replies and replaced ACKs. It stays (Matt, 2026-10-28), like
`EINSTEIN_SOUND_TRACE`.

**To clarify**
- ~~**Q11.1** Build NE2K.pkg with the MPW Makefile through mosrun, or move it
  into the CMake build?~~ CMake (Matt, 2026-10-27).
- **Q11.2** Test setup: which apps and servers (Courier, Newt's Cape, NCX
  Dock over TCP, a local HTTP server)? Matt's emulator has NIE and Courier
  (Install Essentials); he can add more apps.

## Event forwarding on slow machines

(Matt, 2026-10-08, for after the Fast start work.) On slow machines, or on a
fast machine with ASan, the forwarding of host events to NewtonOS can break:
keyboard events and package installer events stop working. A first place to
look: the platform event queue in `TPlatformManager` with its locks
(`mQueuePreLock`, `mQueueLockCount`, `mQueueBootLock`) and the platform
interrupt that tells NewtonOS about a new event. A timing-dependent lock that
is never released would block all later events.

A concrete case of such a lock (2026-10-10, fixed in `63aaed80`): after
"NewtonScript Reboot", `mQueueLockCount` stayed at 1 because NewtonOS reboots
before it answers the event that carried `Reboot()`, and all later events
waited. An event whose answer gets lost on a slow machine would look the
same, but would not be cured by the fix, which only runs at boot. To see the
state, read `mQueueBootLock`, `mQueueLockCount`,
`mQueuePreLock` and `mEventQueueCCrsr`/`PCrsr` of `TPlatformManager` in a
debugger.

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

## Time base (Y2K25) and the 2040 limit

Found 2026-10-17: the Newton did not go to sleep (Windows first, every
platform affected). The Y2K10 patch counted NewtonScript seconds from 2008,
which wrapped on 2025-01-04 (30-bit integers). `SetSysAlarm()` alarms landed
in 1993, fired at once and were set again every few ms; `CyclePower()` found
the alarm message on `gNewtPort` and cancelled the sleep. Fixed: base moved to
2024 (good until 2041-01-04), and the 150-day Windows offset in the RTC is
gone. Details, ROM addresses, how to patch the other ROMs and physical
Newtons, and the plan for 2040: `Documentation/NewtonTime.md`.

Open:
- ~~The time patches exist for the US ROM only.~~ All four ROMs since
  2026-10-23 (addresses found by matching instruction words, table in
  `Documentation/NewtonTime.md`). Matt tested them: they work on all
  four ROMs. The 40hz Y2K10 packages (Patch_D.pkg "74J185", Patch_eMate.pkg
  "73J186") do interfere: they bring the 2008 base and code of
  `TimeInSeconds`, `DateFromSeconds`, `SetTimeInSeconds`, `SetSysAlarm`,
  which then runs instead of the ROM code that Einstein patches. Apple's
  updates contain none of it. Install Essentials now offers 717260, 747260
  and 737246; there is no French 2x00 update (UNNA, checked on the MIT
  mirror `web.mit.edu/unna`; www.unna.org was down on 2026-10-23 for a PHP
  security fix). No warning at launch about a Y2K10 package in the flash
  (Matt, 2026-10-23: users who play around don't care and should not be
  bothered by dialogs; may change after complaints).
- ~~`AdjustRealTimeClock` resets the RTC to 1996 at every boot.~~ Patched on
  all four ROMs (2026-10-23, untested): the upper bound (2010) is gone, so
  the RTC keeps host local time; before 1996 it is still reset. Matt: an
  alarm 1 minute ahead, quit, restart later: the expired alarm popped up.
  But with an alarm pending, quit ran into the 10 s timeout instead of
  sleeping, and the emulation got slow. Cause (2026-10-23): NewtonOS keeps
  the RTC and alarms in UTC (`SetSysAlarm` subtracts `GMTOffset()` and
  `DaylightSavingsOffset()`, the home city), Einstein's RTC counted host
  local time. Matt's Newton is set to Tokyo, the host to Berlin, so alarms
  landed 9 hours early, fired at once and were set again in a loop. Fixed
  (option 1, Matt): the RTC counts host UTC, `GMTOffset()` and
  `DaylightSavingsOffset()` return the host's time zone on all four ROMs,
  `SetRealClockSeconds()` is ignored. The RTC alarm interrupt is emulated
  too (Fast start, next step 11). Matt tested it: alarms on time, waking
  from sleep, sleep with a timer pending, normal speed.
- Time Zones' "Second City" goes back to Tokyo after every reboot, even
  after closing the app; it survives a fast start (Matt, 2026-10-23).
  Probably NewtonOS keeps it in memory only; would show on a real Newton
  after a reset. Not looked into.
- The setting "Fetch date and time from host" (`System/FetchDateAndTime`)
  was saved but not used anywhere; the time always comes from the host.
  Removed (Matt, 2026-10-23); old preference files keep the unused key.
- 2040-02-06: the 32-bit seconds since 1904 overflow (RTC, `RealClock()`,
  `Time()`). Idea: run the clock 28 years behind (same calendar).
- Ctrl-Q forwards a Ctrl key-down to NewtonOS before the quit presses power;
  a key event in that window can cancel the sleep too. On the Mac, any key
  (also modifiers) wakes the emulator.
- ~~`THostInfo::GetHostTimeZone()` on Windows multiplies seconds by 3600, and
  the sign differs between platforms.~~ Fixed 2026-10-23 (untested): it
  returns the host's standard offset, east positive, without daylight
  saving, on every platform (`TInterruptManager::GetHostTimeZone()`). Used
  by the NS Runtime's Setup patch (`kGetLocationWithProperGMTFn`,
  Drivers/NSRuntime/UserInfo.f), which picks the home city whose `gmt`
  matches when the Setup Assistant finishes. On macOS it included daylight
  saving, so a Berlin host got a UTC+2 city in summer. Tested by Matt: Setup now picks a UTC+1
  city, Albania (the first one in the soup with that offset). Idea for
  later: prefer the host's city (macOS knows the zone name, e.g.
  "Europe/Berlin").

## fixups

To do before Multiple Configurations, after the time patches (Matt,
2026-10-23). The Einstein Prefs source code is on Matt's BasiliskII setup
(the NTK project), for the fixes on the Newton side.

Serial port settings:

Status (Matt, 2026-10-23): all three fixed. Matt set kSerialDriverMax to 4
and removed the unused kTcpServerSerialDriverTag in the Einstein Prefs
source; tested on macOS: port and TCP server stay the same across driver
changes and reboots.

- ~~Switching back to TCP resets the address~~ Fixed 2026-10-23:
  `TSerialPorts` keeps the TCP server and port per port
  (`SetTcpClientSettings()`): FLTK passes the saved settings before the
  drivers start, and `ReplaceDriver()` keeps those of a TCP driver it
  replaces. A new TCP client driver starts with them instead of
  127.0.0.1:3679.
- AppConstants.f says kTcpServerSerialDriverTag := 5, but in the emulator 5
  means kDirectDriver (TSerialPorts.h:59-66). Probably copied from the Cocoa
  app, where tag 5 is a TCP *server* that was never implemented and the
  client is 4 (`app/TCocoaUserDefaults.h`); switching to TCP works, so
  Einstein Prefs must send 4. To do in the Einstein Prefs source (Matt):
  check where the constant is used, then remove it or rename it.
- kSerialDriverMax is 5, but GetSerialPortDriverNames only returned 5 names
  (0–4). Einstein side fixed 2026-10-23 (untested): a sixth name, "Host
  Serial Port", for kDirectDriver (assert in the constructor), and
  `NSSetDriverAndOptions()` refuses drivers that are not in
  `ValidDriversByPort` for the platform (for example Pipes on Windows, or
  5); the current driver keeps running. Einstein Prefs could still offer
  the drivers from `GetSerialPortDriverList(port)` (the valid IDs for the
  platform) instead of 0 to kSerialDriverMax.

Later (Matt, 2026-10-23):
- The PTY and Pipes drivers need their own settings, like the TCP client's
  server and port (today: hard-coded `/tmp/BBridge.Einstein` for the PTY,
  fixed pipe names). Also per machine, see Multiple Configurations.
- The BasiliskII driver should show the path it uses on the current
  platform (`/tmp/pty.BasiliskII`).

Next (Matt, 2026-10-23): **Einstein Prefs as a Preferences item.** Instead
of its own app in Extras, Einstein Prefs becomes an entry in the Newton's
built-in Preferences (NewtonOS 2.x `RegPrefs(sym, template)` in an
install script, `UnRegPrefs(sym)` in the remove script; the ROM has the
`prefsRegistry`). See *Next steps to wrap up state snapshots*, item 13. Matt plans to build the
package with `newtc` instead of NEWT/0 (closer to the original NTK, with
source level debugging); not started yet.
