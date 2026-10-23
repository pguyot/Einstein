
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

