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
