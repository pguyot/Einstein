Binary Release v2026.10.28
==========================

User Release Notes
------------------
 - Network: downloads are more than 20 times faster. A 67 KB package from
   unna.org came in at about 90 KB/s instead of 4 KB/s. Web pages load
   faster too; with Courier, drawing the page now takes most of the time.
 - Network: the Newton no longer freezes while Einstein connects to a server
   or looks up a name. A server that does not answer used to freeze it for
   10 seconds per try; now NewtonOS gets an error after 10 seconds.
 - Network: connections close cleanly, and a server that closes or resets a
   connection no longer leaves it hanging. On macOS and Linux, Einstein
   could quit without a message when it sent to a connection that the
   server had closed; that can no longer happen.
 - Docking over the network: Dock with NCX works over the Einstein Network
   Card, using DockTCP by Paul Guyot on the Newton
   (http://www.kallisys.com/files/newton/DCL/Dock_TCP-1.3-en.pkg). Before, the
   connection stopped right after the password.
 - The Einstein Network Card driver is built into Einstein: Help › Install
   Essentials... installs the version that belongs to this Einstein, without
   a download. Install it again after updating Einstein; the faster
   transfers need the new driver.
 - Sound: short sounds, like the tap on the screen, play completely, and
   longer sounds no longer have short gaps or get cut off at the end. The
   faster the computer, the worse it was.
 - macOS: the end of a sound no longer repeats briefly.

Developer Release Notes
-----------------------
 - NE2K.pkg is built by the `Drivers` CMake (target `NE2K.pkg`, installed to
   `_Data_/NE2K.pkg`): the driver from `Drivers/NE2000Driver` with gcc and
   `ELFtoPKG`, then `NE2000Driver/AddNSPart.py` adds the NewtonScript part of
   1997 (`NE2K.auto`). Frame part references, relocation offsets and
   relocated values all count from the start of the package, so the script
   keeps the NewtonScript part at its old offset and moves the protocol
   part's relocations. The MPW build files are gone.
 - Einstein embeds `_Data_/NE2K.pkg` (`gNE2KPackage`, like the REx);
   Install Essentials links starting with `E` are built-in packages
   (`einstein:NE2K.pkg`).
 - The driver hands Lantern all waiting packets (at most 16) per tick and on
   the card interrupt: Einstein raises the IREQ of the socket that holds the
   network card for every packet it queues. The TimerExpired native returns
   the time to the next tick: 20 ms while packets flow, 100 ms otherwise.
 - `TUsermodeNetwork`: no 0.25 s throttle any more; TCP sends as many
   segments as the Newton's window takes (4096 bytes in NIE) and fills it
   again as soon as an ACK opens it; a newer pure ACK replaces a queued one.
   Connects run in the background (RST after 10 s or on an error); Newton
   data the server does not take waits in a 16 KB buffer per connection,
   whose free space is the window; `SO_NOSIGPIPE`/`MSG_NOSIGNAL`; resets in
   both directions; packets for no connection get an RST instead of opening
   a new one; closing connections end after 30 s at the latest. DNS names
   are looked up with `getaddrinfo()` in a worker thread; queries other than
   A/IN are answered "not implemented".
 - Dock_TCP takes only one Dock command per TCP segment. On a connection
   whose first Newton data starts with `newtdock`, Einstein sends each Dock
   command from the desktop in a segment of its own.
 - Frames from the Newton are checked before the handlers read them
   (`Packet::CheckHeaders()`); TCP and UDP payloads end at the IP length, so
   the padding of short frames is no longer data. The network natives check
   their sizes.
 - `EINSTEIN_NET_TRACE` prints the driver's natives and every packet, TCP
   packets with flags, sequence numbers, window and the first payload bytes.
 - `TCircleBuffer` (the sound queue of all sound managers but waveOut, and
   the read buffer of the PTY and direct serial host ports) lost data: when
   the producer was behind the consumer, the free space underflowed and new
   data overwrote unread data, and a buffer filled exactly looked empty.
   `Produce()` now keeps one byte free and grows the buffer when needed.
   Unit tests in `_Tests_/CircleBufferTests.cpp`.
 - CoreAudio: `StopOutput()` lets the queued samples play out (NewtonOS
   stops the output while the last buffer is still queued, because the
   output interrupt comes as soon as there is room for the next buffer). The
   output unit keeps running for a second after the last sound, so the next
   one starts at once. The render callback zeroes the rest of its buffer,
   also in the PortAudio manager. `EINSTEIN_SOUND_TRACE` prints the calls of
   NewtonOS's sound driver with their times.

Binary Release v2026.10.26
==========================

User Release Notes
------------------
 - Several machines: the Settings window has a list of emulated Newtons on
   the left (open it with the << button at the bottom left). Each machine
   has its own ROM, internal storage, memory, screen, serial port, saved
   states and PC Cards, and its own Newton ID. Add, duplicate, rename,
   reorder and delete machines from the list; start one with its play
   button.
 - Several machines can run at the same time, each in its own Einstein
   window, with its name in the window title. A machine can only run once,
   a PC Card can only be in one running machine, and two machines can't run
   on the same internal storage file. While a machine runs in another
   window, its settings can't be changed.
 - Your settings so far become the first machine. It keeps its ROM and its
   internal storage where they were, and its Newton ID, so existing backups
   in Newton Connection and NCX still match it. An older Einstein starts the
   first machine in the list.
 - The ROM and the internal storage can be kept in the machine's folder or
   used from anywhere on your computer; Einstein remembers both, so you can
   switch back and forth. A machine whose files are in its folder can be
   taken to another computer: copy the folder, and add it there with
   + › Add Existing Machine...
 - A light shows whether Einstein knows the ROM (green), doesn't know the
   file (yellow) or has none (red), and whether the internal storage exists.
   Brain Wipe... in the Settings erases the internal storage, after asking.
 - The Settings save every change at once; Start is the only button. While
   the machine list is shown, the Settings window stays open after Start,
   so you can start more machines. "Don't show at startup" is now "Show at
   startup".
 - File › Close (Cmd+Shift+W, so Cmd+W stays with NewtonOS) and the window's
   close button end the Newton like Quit, but keep the Settings window open
   if it is. Quit always quits.
 - Windows: Einstein no longer crashes when you close its window with fast
   start on. The state was saved before the crash, so no data was lost.
 - Windows: the Newton no longer freezes, and Einstein no longer hangs on
   quit, when NewtonOS stops a sound (for example, quitting a few seconds
   after launch).
 - Install Essentials no longer crashes when a download server does not
   answer; it shows "File can't be downloaded" instead.

Developer Release Notes
-----------------------
 - `--machine <name or id>` starts a machine without the Settings window.
   An unknown machine is an error (exit code 1).
 - Each machine has a folder `Machines/<uuid>/` in the user data folder:
   `Machine.prefs` (its settings and Newton ID, `System/NewtonID`), its
   flash, `FastStart.state`, `Einstein.state` and, if chosen, its ROM. The
   main preferences hold the list (`Machines/Order`, `LastUsed`,
   `SidebarOpen`) and the shared settings; the first machine is also
   written to and read from the old keys.
 - `Machine.prefs` stores the ROM and the flash as `inFolder`, `name` (the
   file in the machine folder) and `path` (the file on the host). Files in
   the machine folder, and the state file there, are stored by name only,
   so the folder can be moved; the old keys get the full path of the file
   in use.
 - Lock files, released by the OS when a process ends: `Machine.lock` per
   machine, `Settings.lock` for the Settings window, `Cards/<uuid>.lock`
   while a PC Card is inserted, `Flash/<hash of the path>.lock` for the
   flash file of a running machine.
 - Close ends the emulation in the running process (`TFLApp::CloseNow()`:
   the Newton sleeps, the fast start state is saved, the emulator thread is
   joined and the locks are released); the process ends when the Settings
   window closes. Machines started from the list run in a new process.
 - macOS: Quit in the application menu calls `UserActionQuit()`
   (`app/FLTK/TFLAppMac.mm`). FLTK's own Quit closes every window, which
   would only be a Close now.
 - The Settings dialog identifies a ROM without applying the ROM patches,
   so "Applying patch at ..." appears only once.
 - Windows sound: the waveOut device signals an event (`CALLBACK_EVENT`)
   and a thread of Einstein's handles the played buffers. The callback
   used to call waveOut functions, which deadlocked with `waveOutReset()`.
 - `TBufferLog` writes to its refresh socket only if it is >= 0; the FLTK
   app leaves it at -1, and `write(-1)` ends the program on Windows.

Binary Release v2026.10.23
==========================

User Release Notes
------------------
 - The Newton goes to sleep again. Since January 2025, alarms that NewtonOS
   sets for itself were set to a date in 1993, went off at once, and woke
   the Newton every time it tried to sleep. Einstein now counts the
   NewtonScript time from 2024, which is good until January 2041.
 - The date fix (Y2K10) now works for all four ROMs: U.S. and German
   MessagePads, eMate 300 and Watson. Before, only the U.S. ROM showed the
   right date; the others showed 1996.
 - Help › Install Essentials offers Apple's System Update for each ROM
   (U.S. 717260, German 747260, eMate 737246; there is none for Watson)
   instead of the Y2K10 patch packages. Don't install a Y2K10 patch
   package: it replaces Einstein's fix with an older one, and the Newton
   no longer goes to sleep. If you installed one, back up your Newton and
   start with a new flash file.
 - Windows: the Newton's clock is no longer off by 150 days and one hour.
 - Windows: Einstein.exe no longer needs the Visual C++ Redistributable.
   With an older version of it installed, Einstein could crash while the
   Newton was booting.
 - Serial port: connections over TCP (for example to NCX or Newton
   Connection), named pipes, pseudo terminals and BasiliskII are much more
   reliable. Data that arrived faster than NewtonOS could read it
   overwrote earlier bytes, so the Newton asked for the same data again and
   again; NCX backups stopped halfway. Einstein now passes data on at about
   the speed of a fast serial line.
 - Switching to another serial driver in the Settings or in Einstein Prefs
   now stops the old driver cleanly.
 - Switching the serial port back to the Network Client keeps your server
   address and port; before, it went back to 127.0.0.1:3679.
 - Fast start: a new version of Einstein starts the Newton normally instead
   of continuing a state that an older version saved. Saved states (File ›
   Save State) still load into a new version.
 - Quitting always puts the Newton to sleep first, also with Fast start
   off, so apps save what you are editing. Before, a quit could lose the
   last changes in an open note. (Works on the eMate still saves a document
   only when you leave it, for example with Overview.)
 - Screen rotation: when you rotate the Newton's screen, Einstein starts in
   the same rotation next time. After a fast start or when restoring a saved
   state, a rotated screen no longer shows garbled lines.
 - eMate 300: the eMate starts in landscape. The screen size in the
   Settings is the size at the start: 320 x 480 for a MessagePad, 480 x 320
   for an eMate; both give an eMate the same screen.
 - eMate 300: the Special Keys (close, extras, dock, send, receive, scroll,
   overview, undo, find, assist, light, power) appear as a row of buttons
   under the screen. Turn them off with "Special Keys (eMate)" in the User
   Interface tab of the Settings. On your keyboard, Esc and F1 to F10 do
   the same.
 - F11 switches the backlight on or off, F12 presses the power button.
 - The German MessagePad, eMate 300 and Watson ROMs no longer ask to
   calibrate the screen.
 - Alarms work: alarms and timers go off on time, also while the Newton
   sleeps, which wakes it up. An alarm that came due while Einstein was
   quit shows up at the next start.
 - The Newton always uses your computer's time zone, whatever home city
   is set in Time Zones.
 - On a new Newton, the Setup Assistant picks a home city in your
   computer's time zone. Before, the city was off by an hour in summer on
   macOS, and in the wrong direction on Windows and Linux.
 - macOS: Einstein no longer asks for access to your contacts at every
   launch. It asks when the Newton first wants your name and address, in
   the Setup Assistant.
 - Einstein Prefs only offers serial drivers that exist.
 - The setting "Fetch date and time from host" is gone; it had no effect.
   The Newton always shows your computer's date and time.

Developer Release Notes
-----------------------
 - The emulated real-time clock counts UTC, as on a real Newton, and no
   longer goes back to 1996 at every boot (all four ROMs). `GMTOffset()` and
   `DaylightSavingsOffset()` return the host's time zone, and setting the
   time in NewtonOS no longer changes the clock. Before, the clock ran on
   local time, so alarms were off by the home city's offset and could fire
   in an endless loop.
 - The RTC alarm interrupt is emulated: it fires once when the clock
   reaches the alarm register, and wakes a sleeping Newton if the wake-up
   mask (0x0F184800) allows it.
 - `Documentation/NewtonTime.md` describes how NewtonOS keeps the time,
   the ROM addresses of the time patches, how to patch other ROMs and
   physical Newtons, and the 2040 limit of the 32-bit clock.
 - The TCP client socket never blocks, also not while connecting, so a quit
   or a driver change is answered at once. A server that does not answer is
   given up after 5 seconds, and Einstein tries again after 3 seconds.

Maintenance Release Notes
-------------------------
 - Windows ARM64 builds again with the static C runtime: FLTK 1.4.5's
   `rint()` fallback is patched for old MSVC only
   (`cmake/PatchFLTKMath.cmake`, until FLTK 1.5).
 - The unit tests pass on Linux x64, Linux ARM64, Windows and macOS. On
   macOS they hung before `main()`, waiting for access to the contacts.
 - The Cocoa CI workflow builds, tests and archives the app unsigned.

Binary Release v2026.10.13
==========================

User Release Notes
------------------
 - Network card: NIE and the Einstein Network Card need System Update 2.1
   (717260) on U.S. MessagePads. Without it, inserting the card fails with
   "Sorry, a problem has occurred (-48807)". Help › Install Essentials now
   offers the update, and lists what to enter in Internet Setup. Back up
   your Newton first: installing the update can erase its memory.
 - More memory: the Settings offer 8 MB and 16 MB of RAM (MessagePad tab).
   A RAM size chosen in the Settings dialog at launch is used right away.
 - File › Next Screenshot (Shift+Cmd+K) saves another screenshot without
   asking, under the last name with the next number (Shot_001.png,
   Shot_002.png, ...). The first time, it asks for the file like
   Screenshot....

Developer Release Notes
-----------------------
 - Breakpoints in ROM no longer make NewtonOS erase the internal store at
   the next Hardware Reset or Reboot ("The internal store was erased
   because a different ROM has been installed"). Einstein now reports the
   ROM checksums of the image as it was loaded, on all four supported ROMs.

Binary Release v2026.10.11
==========================

User Release Notes
------------------
 - Fast start: when you quit Einstein, the Newton goes to sleep and Einstein
   saves its state. The next launch continues where you left off, like
   waking up a real Newton. Turn it on or off with "Fast start: continue
   where Einstein was quit" in the MessagePad tab of the Settings (on by
   default). If the ROM, the RAM size, the screen size, the flash file or a
   PC Card changed in the meantime, Einstein starts the Newton normally.
 - Save and restore states: the File menu has "Save State", "Save State
   As...", "Restore Last" and "Restore From...". Save the state before you
   try out a package, and return to it later. Restoring replaces the
   Newton's memory, its internal storage (flash) and the contents of the
   inserted PC Cards, so Einstein asks before it does that.
 - Hardware Reset and Brain Wipe now reset all emulated hardware, like the
   reset button of a real Newton (the memory is kept).
 - After "NewtonScript Reboot", the keyboard, the power button and package
   installation work again.
 - Einstein Prefs, the app that sets up the serial port from inside the
   Newton, is now built in (Extras › Einstein). You don't need to install it
   any more.
 - Fast start and states are available in the FLTK version (macOS,
   Windows, Linux), not in the older native macOS (Cocoa) version.

Developer Release Notes
-----------------------
 - Monitor: "save <path>" and "load <path>" now work reliably and include
   the emulated hardware, the PC Card state and the card contents. "save fast
   <path>" writes a fast start file, "load overwrite <path>" replaces card
   contents that changed, "checkstate [path]" checks that saving and
   loading give the same result.
 - State files have a new format with a checksum. Damaged or edited files
   are refused before anything is loaded.
 - Several fixes for threads that changed the same data at the same time,
   found with ThreadSanitizer (for example a lost interrupt).

Maintenance Release Notes
-------------------------
 - The unit tests run in their own workflow and are only built with
   `-D EINSTEIN_BUILD_TESTS=ON`.

Binary Release v2026.10.6
==========================

User Release Notes
------------------
 - add ARM64 support for Windows and Linux builds

Binary Release v2026.9.21
==========================

User Release Notes
------------------
 - ATA PC-Card support added

Binary Release v2025.12.19
==========================

User Release Notes
------------------
 - new menu item "Snapshot..." to take screenshots

Binary Release v2025.9.26
==========================

User Release Notes
------------------
 - various maintenance updates and fixes

Binary Release v2024.12.26
==========================

User Release Notes
------------------
 - various maintenance fixes

Binary Release v2024.4.21
=========================

Maintenance Release Notes
------------------
 - updates to the CMake build system (modern CMake)
 - update to FLTK 1.4.0
 - fixes for MS Windows version on ARM64 CPUs

Binary Release v2022.4.20
=========================

User Release Notes
------------------
 - printer support ported to Linux and MSWindows in addition to the already
   working macOS implementation
 - fixed settings dialog to directly enter ROM and Flash file names
   in input fields


Binary Release v2022.4.19
=========================

User Release Notes
------------------
 - printer support, printer driver is built into REX

Toolkit Release Notes
---------------------
 - NewtonScript host file access


Binary Release v2022.4.18
=========================

User Release Notes
------------------
 - new setting to scale the Newton screen at startup

Toolkit Release Notes
---------------------
 - NewtonScript strings can now have a tab character in them
 - new NS command `MakeBinaryFromARM` translates ARM assembler into binary data
 - new Script command `PatchFileFromARM` to patch ROM files using ARM assembly code
 - Toolkit remembers the recent eight Script files for quick access


Binary Release v2022.4.17
=========================

This is our first automated binary release for Einstein.

Release 4.17 contains a few unfinished features for early
testing. After collecting user somments, a full featured
set will be implemented in v2022.5.0.

User Release Notes
------------------
 - Y10k problem is always fixed, time and date is set automatically
 - Newtwork setup now emulates DHCP, many more network emulation fixes
 - Added Credits and Licenses section to the About panel
 - Added Essentials panel for easy install of common tools and apps
 - print screenshots (FLTK)

Developer Release Notes
-----------------------
 - serial port drivers changed internally
 - many fixes to building, bug finding, and deployment
 - many fixes to formatting and ease of maintenance
 - REx can now compile on modern machines
 - the REx is now part of the app (Cocoa)

Releases come for different CPUs:
 - x64 stands for Intel 64 bit CPUs
 - arm64 binaries run on 64 bit ARM systems
 - universal binaries run on Apple Intel and M1 Apple silicon

Releases use different User Interface libraries:
 - Cocoa is Paul's original version running on MacOS without additional software
 - FLTK adds PCMCIA and Toolkit support and runs on all desktop platforms
 - the Androis version uses NDK directly and is not yet part of this release yet
 - due to restriction in the Apple Stroe, the iOS version needs to be built from source as explained in BUILDING.md

And as always, enjoy, and give us feedback, no matter if good or bad.

