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
