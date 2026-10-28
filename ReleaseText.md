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
