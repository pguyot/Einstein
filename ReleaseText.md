
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

