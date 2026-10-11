# Einstein User Guide

Einstein emulates an Apple Newton MessagePad 2000/2100 (and the eMate 300) on
your computer. The Newton was a pen-based handheld computer, sold by Apple
from 1993 to 1998. With Einstein you can run NewtonOS and Newton software on
macOS, Windows and Linux.

This guide describes the FLTK version of Einstein, which is the version on the
[release page](https://github.com/pguyot/Einstein/releases). The older native
macOS version (Cocoa) has fewer features.

**Contents**

1. [Before you start](#1-before-you-start)
2. [First launch](#2-first-launch)
3. [Essential software](#3-essential-software)
4. [The main window](#4-the-main-window)
5. [Installing packages](#5-installing-packages)
6. [Fast start, saving and restoring](#6-fast-start-saving-and-restoring)
7. [Resetting the Newton](#7-resetting-the-newton)
8. [PC Cards](#8-pc-cards)
9. [Serial port and Dock](#9-serial-port-and-dock)
10. [Networking](#10-networking)
11. [For developers: Toolkit and Monitor](#11-for-developers-toolkit-and-monitor)
12. [What Einstein can't do yet](#12-what-einstein-cant-do-yet)
13. [More resources](#13-more-resources)


## 1. Before you start

Einstein needs two files:

- **A ROM image.** The ROM holds NewtonOS. We can't distribute it, because it
  is Apple's property. If you own a Newton, you can copy the ROM from it; see
  [Dumping The Rom](https://github.com/pguyot/Einstein/wiki/Dumping-The-Rom)
  in the wiki.
- **A flash file.** The flash is the Newton's internal storage: your notes,
  names and dates, and the software you install. Einstein creates a new,
  empty flash file for you, or you can use one from an earlier Einstein
  installation.

Keep a copy of your flash file. It holds all your Newton data.


## 2. First launch

When you start Einstein, the Settings window opens.

1. In the **MessagePad** tab, click **Choose...** next to *Newton ROM* and
   select your ROM image. Einstein shows "ROM file found" when it recognizes
   the file.
2. Next to *System Flash*, click **Create...** for a new flash file, or
   **Choose...** for an existing one.
3. Choose the *System RAM*: 1 MB (like a MessagePad 2000 or an eMate 300) or
   4 MB (like a MessagePad 2100).
4. Leave *Fetch date and time from host* on, so the Newton shows your
   computer's date and time.
5. Leave *Fast start: continue where Einstein was quit* on (see
   [section 6](#6-fast-start-saving-and-restoring)).
6. If you check **Don't show at startup**, Einstein starts the Newton right away
   next time. You can open the Settings any time with **Window › Settings...**.
7. Click **Start**.

The Newton starts. The first start takes a little longer, while NewtonOS sets
up the new flash.

To quit, choose **File › Quit** or close the window.

In this guide, *Cmd* means the Command key on macOS and the Ctrl key on
Windows and Linux. For example, **Window › Settings...** is Cmd+Shift+U.


## 3. Essential software

**Help › Install Essentials...** opens a window with software that most
Einstein users want:

- **NewtonOS Y2K10 Fix.** NewtonOS has a bug with dates after January 5,
  2010. Choose the patch that matches your ROM (German MP2x00 or eMate 300).
  Read the warning in the window before you install a patch.
  The US MP2x00 ROM (717006) is patched by Einstein itself, so you do not
  need to install a patch for it.
- **NIE, the Newton Internet Enabler**, and the **Einstein Network Card
  driver**, for networking (see [section 10](#10-networking)).
- **Courier**, a web browser, and **ViewFrame**, a tool for programmers.

Einstein downloads these packages from the Internet and installs them on the
Newton.

<!-- TODO(Matt): the outline mentions the built-in NewtTest. What is it for? -->
<!-- NewtTest is a program that tests some Newton functionality and outputs
     some statistics, including emulation speed -->

## 4. The main window

The main window shows the Newton screen. Below the menu bar is a toolbar.

- **The pen** is your mouse: click to tap, drag to write or draw.
- **The keyboard**: what you type on your computer goes to the Newton.
- **Power** (toolbar, or **Platform › Power Button**) puts the Newton to
  sleep or wakes it up, like the power switch of a real Newton.
- **Backlight** (toolbar, or **Platform › Backlight**) switches the screen
  light on or off.
- **PCCards** shows the PC Card menu of the upper slot when you click its
  upper half, and of the lower slot when you click its lower half (see
  [section 8](#8-pc-cards)).
- **Install Package** installs Newton software (see
  [section 5](#5-installing-packages)).

The **Window** menu changes the window:

- **Initial Size** returns the window to the size of the Newton screen.
- **Fullscreen** (Cmd+Shift+F) fills your screen.

In the **User Interface** tab of the Settings, you can choose the screen size
of the Newton in pixels, a scale for the window (100% to 200%), whether the
menu bar and the toolbar are shown, and whether the mouse pointer is hidden
over the Newton screen. A new screen size takes effect at the next start.

**File › Print...** (Cmd+Shift+P) prints the Newton screen.
**File › Screenshot...** (Cmd+Shift+J) saves the Newton screen as a PNG
image.


## 5. Installing packages

Newton software comes as package files ending in `.pkg`. There are three ways
to install a package:

1. Choose **Platform › Install Package...** (Cmd+Shift+I), or click **Install
   Package** in the toolbar, and select the file.
2. Drag the package file from your computer onto the Newton screen.
3. Drag a link to a package (`http://...pkg`) from your web browser onto the
   Newton screen. Einstein downloads and installs it.

Before you try out a new package, you can save the state of your Newton and
return to it later (see the next section).


## 6. Fast start, saving and restoring

### Fast start

When you quit Einstein, the Newton goes to sleep and Einstein saves its state.
The next time you start Einstein, the Newton wakes up where you left off, like
a real Newton that you switch on again.

Einstein starts the Newton normally (from its flash) instead if something
changed in the meantime: the ROM, the RAM size, the screen size, the flash
file, or a PC Card. Your data is safe either way, because it is in the flash.

You can turn fast start off in the **MessagePad** tab of the Settings
(*Fast start: continue where Einstein was quit*).

### Saving and restoring a state

You can save the complete state of the Newton and return to it later. For
example, save the state before you try out a package, and restore it if you
don't like the package.

- **File › Save State** (Cmd+Shift+S) saves the state to the current state
  file.
- **File › Save State As...** saves the state to a file of your choice. This
  file becomes the current state file.
- **File › Restore Last** returns to the state in the current state file.
- **File › Restore From...** returns to the state in a file of your choice.

The current state file is the one you saved or restored last. At first, it is
`Einstein.state` in Einstein's data folder.

A state file holds everything: the Newton's memory, its internal storage
(flash), and the contents of the inserted PC Cards. **Restoring replaces all
of it.** Everything you did after saving the state is lost, including new
notes and installed packages. Einstein asks before it restores, and names the
PC Cards whose contents would be replaced.

You can only restore a state with the same ROM, RAM size, screen size and the
same kind of PC Cards as when it was saved. If that is not the case, Einstein
tells you what changed.


## 7. Resetting the Newton

**Platform › Reset** has three choices:

- **System Reboot** restarts NewtonOS, like choosing Restart on a Newton. Your
  data stays.
- **Hardware Reset** is like the reset button on the back of a real Newton.
  The emulated hardware starts over; the memory and the flash stay. Use it if
  the Newton doesn't react any more.
- **Brain Wipe** resets the Newton, and NewtonOS then offers to erase all
  data. Only use it if you really want to start over with an empty Newton.


## 8. PC Cards

The Newton has two PC Card slots. Einstein emulates three kinds of cards:

- **Linear flash cards** (2 MB to 32 MB), like the Newton memory cards.
- **ATA flash cards** (4 MB to 64 MB), like CompactFlash cards in an adapter.
  ATA card emulation requires Paul Guyot's ATA card driver installed on the Newton.
- **The Einstein Network Card**, for networking (see
  [section 10](#10-networking)).

### Creating cards

Each memory card is an image file on your computer. In the **PC Cards** tab
of the Settings:

1. Click **+**.
2. Choose a new card (for example *New 4MB Linear Flash Card...*), or *Add
   existing Linear or ATA Image...* for an image file you already have.
3. Give the image file a name and a place.

The **...** menu lets you change the name, tag and image path of a card. The
**-** button removes a card from the list (the image file stays).

### Inserting and removing cards

Use **Platform › Upper PCCard** and **Platform › Lower PCCard**, or the
upper or lower half of the **PCCards** toolbar button:

- **Insert Card:** inserts a card from your list.
- **Replace Card:** removes the inserted card and inserts another one.
- **Eject Card** removes the card. Close the documents on the card first.
- **Keep in Slot** keeps the card inserted the next time you start Einstein.

### Backing up cards

To back up a card, quit Einstein and copy its image file. Einstein writes
changes to the image file a few seconds after the Newton changes the card,
and completely when you quit.

A card image can be used by more than one emulated Newton, but not by two at
the same time.

<!-- TODO(Matt): how to make an image from a physical card ("dump physical cards") -->

Einstein ATA card files use the same format as Newton's ATA cards. You can
dump the snapshot of a physical card, mount it in Einstein (tested, working),
and write the image back to the physical card (untested).


## 9. Serial port and Dock

The Newton's serial port connects it to a desktop computer, for example to
back up and synchronize data ("Dock"). In Einstein, the serial port is
connected to a program on your computer instead of a cable.

To set it up, open **Einstein Prefs** on the Newton (the *Einstein* icon in
the Extras drawer) and tap
**Serial Port Driver**:

- **Driver** chooses how the serial port is connected:
  - *Network Client* connects to a program on your computer, or on another
    computer, over TCP/IP. This is the default, and the only choice on
    Windows.
  - *Pseudoterminal*, *Named Pipes* and *BasiliskII* (macOS and Linux) are
    for special setups.
  - *None* disconnects the port.
- **Server** and **Port** tell the Network Client where the program runs.
  The default is `127.0.0.1` (your own computer), port `3679`.

<!-- TODO(Matt): which Dock programs work with Einstein (and the Network Client), and how to set them up -->


## 10. Networking

Einstein can connect the Newton to the Internet through an emulated network
card:

1. Install **NIE** and the **Einstein Network Card driver** with
   **Help › Install Essentials...**.
2. Insert the **Einstein Network Card** into a PC Card slot (it is in the card
   list in the Settings).
3. Set up the connection in NewtonOS (*Internet Setup*; Install Essentials
   has a button to open it).

<!-- TODO(Matt): what to enter in Internet Setup; browser, e-mail, Network Dock -->


## 11. For developers: Toolkit and Monitor

### Toolkit

**Window › Toolkit** (Cmd+Shift+T) opens a NewtonScript development
environment. You can write, build and run Newton programs without a Newton
or Newton Toolkit.

### Monitor

**Window › Monitor** (Cmd+Shift+M) opens a low-level debugger. It shows the
CPU registers and lets you stop the emulated CPU, step through code, set
breakpoints and look at memory. Type `help` in the Monitor for all commands.

Commands for state files (the Newton must be stopped with `stop` first):

| Command | What it does |
|---|---|
| `save <path>` | save the state, like File › Save State As... |
| `load <path>` | load a state; refused if a PC Card changed since |
| `load overwrite <path>` | load a state and replace the contents of changed PC Cards |
| `save fast <path>` | save a fast start file (holds only a checksum of the flash) |
| `checkstate [path]` | save, load and save again, then compare the files |

*Launch Monitor at Boot* in the **User Interface** tab of the Settings opens
the Monitor when Einstein starts.


## 12. What Einstein can't do yet

- Infrared beaming between Newtons is not emulated.
- Alarms set in Dates don't go off, and an alarm can't wake the Newton.
- Multiple configurations (several emulated Newtons with their own settings)
  are planned, but not available yet.
- The Edit menu (Cut, Copy, Paste) doesn't work yet. However, moving selected
  text to the left or right screen edge makes the text available in the host's
  paste buffer.


## 13. More resources

- [Einstein on GitHub](https://github.com/pguyot/Einstein): downloads,
  source code, and bug reports ("Issues").
- [The Einstein wiki](https://github.com/pguyot/Einstein/wiki)
- [UNNA](http://www.unna.org/), the archive of Newton software
- [messagepad.org](http://www.messagepad.org/)

Einstein is released under the GNU General Public License, version 2. **Help ›
About Einstein...** lists the licenses of the software that Einstein uses.

The old user manual (2018) is in [Historic](Historic/UserManual.pdf).
