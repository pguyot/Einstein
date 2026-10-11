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
