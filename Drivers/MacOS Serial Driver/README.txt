
Einstein Serial Driver
======================

The Einstein Serial driver is a macOS kernel extension (KEXT) that will help
the Einstein emulator to present its serial port to the host machine. Apps
running on the host machine will not see a difference to a MessagePad connected
via a serial cable and a USB-to-serail converter.

This allows existing apps like NCX, or NCU or NTK via BasiliskII, to use
Einstein emulation just as if a physical machine was connected.


Prerequisites
-------------

The Einstein Serial Driver is implemented as a KEXT on macOS. Other platforms
can use much simpler BSD dirvers, 'socat' port redirection, or 'com0com' to
emulate a serial port. MacOS is quite a bit more demanding, requiring a rather
complex driver, signed with a special certificate that Apple may or may not
grant us.

The KEXT s based on a collection of USB-to-serial KEXTs found all over the net,
and the "AppleUSBCDCC" source code, also available on the net. At the time
of development, MacOS High Sierra is the current OS. Unfortunatley, High Sierra
is much more demanding to KEXT, plus a lot of the header files used in the
sample code vanished and was replaced by new concepts.

Since KEXTs run as part of the BSD kernel, any bug or error in the KEXT will
crash the entire system into a kernel panic. A second machine, preferably an
emulated virtual machine, for testing is an absolute must have. The emulated
machine must mave all security features disabled.


Implementation Concept
----------------------

The Serial Driver is a KEXT that forwards Einstein serial port communication
from a sytem socket to a BSD style device file that appears as a serial port
to the host machine.

 emulated              native
MessagePad <--------> Einstein
	     HW emualation

        user space       kernel space     /dev/cu.Einstein
         Einstein <--------> KEXT <--------> BSD device
               systems socket   serial port Nub

                                        native                emulated
				BSD device <--------> BasiliskII <--------> NTK, NCU, etc.
					 BSD read/write/ioctl       HW emulation

Even though this looks like a quite complicated way to acheive this line of
communication, it seems the only fully compatible way to do this. A virtual
Null-Modem cable ("com0com") or a working "socat" would make this simpler, but
the current safety concept of macOS don;t seem to allow that. Even this
implementation is not yet sanctioned by Apple.


Development Status
------------------

The driver skeleton is launching as a driver in an unprotected machine. It
creates the BSD device files for a durect serial connection and a terminal
connection. It receives commands to set the baud rate and other serial port
settings via "ioctl" to "/dev/cu.Einstein", and it tries to recive serial
data sent from an external app,

FIXME: but is not received by the KEXT yet.

TODO: before continuing the KEXT, I want to implement a little tool that
simulates communication on the BSD side, so I can trace the exact reaction of
the KEXT.

TODO: I want to implement the Einstein <--> KEXT path of communication next, so
I can reliably follow the KEXT communication:

https://developer.apple.com/documentation/kernel/1809176-ctl_register

This will also require a little tool that simulates the socket part of the
communication the the KEXT. This shall also replace the system level logging
which is rather complex and unneccessary.








