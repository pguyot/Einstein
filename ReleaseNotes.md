
Binary Release v2026.10.x
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

