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
