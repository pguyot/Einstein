// ==============================
// File:			TUsermodeNetwork.cp
// Project:			Einstein
//
// Copyright 2010 by Matthias Melcher (mm@matthiasm.com).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

/*
 USERMODE NETWORK HANDLER
 
 The user mode Network handler works by emulating an ethernet card on a 
 pretty low level for the Newton. To use the network, the NIE2.0 and the 
 Einstein NE2000 driver must be installed on the Newton. By inserting
 the NE2000 card, the driver will be activated and all network calls
 will be forwarded to the Network Handler that was loaded at startup.
 
 All packats from the Newton are at the lowest network level. It is up to
 the handler to simulate or forward packets to the host or host network.
 The Newton expects to receive packets on the same low level, so we must
 generate the network header, IPv4 header, and TCP or UDP header ourselves.
 
 CURRENT STATUS
 
 At the current stage the driver is loaded correctly and all call forwarding 
 works. We can send and receive packets. A simuated ARP packet and a DNS
 request via UDP are accepted by the Newton.
 
 NEXT ACTION
 
 Next we need to implement handlers for all types of packages that we want
 to support. Forwarding packages that the Newton sends should be relatively
 easy. Generating the correct reply is a bit more complex.
 
 A working TCP protocol will allow web browsing and possibly reading mail.
 
 The goal is to set up a connection to NCX, Newtsync or Escale running locally. 
 
 */

/*
 tcpdump: listening on en1, link-type EN10MB (Ethernet), capture size 65535 bytes
 16:21:39.943756 IP (tos 0x0, ttl 64, id 10316, offset 0, flags [DF], proto TCP (6), length 64)
 192.168.0.200.58982 > borg.org.http: Flags [S], cksum 0x8065 (correct), seq 2730474047, win 65535, options [mss 1460,nop,wscale 3,nop,nop,TS val 774403430 ecr 0,sackOK,eol], length 0
 0x0000:  0024 36a2 a7e4 58b0 3577 d723 0800 4500  .$6...X.5w.#..E.
 0x0010:  0040 284c 4000 4006 4317 c0a8 00c8 4069  .@(L@.@.C.....@i
 0x0020:  cd7b e666 0050 a2bf ba3f 0000 0000 b002  .{.f.P...?......
 0x0030:  ffff 8065 0000 0204 05b4 0103 0303 0101  ...e............
 0x0040:  080a 2e28 7566 0000 0000 0402 0000       ...(uf........
 16:21:40.181068 IP (tos 0x0, ttl 48, id 0, offset 0, flags [DF], proto TCP (6), length 60)
 borg.org.http > 192.168.0.200.58982: Flags [S.], cksum 0x19ec (correct), seq 1704004503, ack 2730474048, win 5792, options [mss 1400,sackOK,TS val 3757379566 ecr 774403430,nop,wscale 0], length 0
 0x0000:  58b0 3577 d723 0024 36a2 a7e4 0800 4500  X.5w.#.$6.....E.
 0x0010:  003c 0000 4000 3006 7b67 4069 cd7b c0a8  .<..@.0.{g@i.{..
 0x0020:  00c8 0050 e666 6591 0b97 a2bf ba40 a012  ...P.fe......@..
 0x0030:  16a0 19ec 0000 0204 0578 0402 080a dff5  .........x......
 0x0040:  0fee 2e28 7566 0103 0300                 ...(uf....
 16:21:40.181180 IP (tos 0x0, ttl 64, id 35650, offset 0, flags [DF], proto TCP (6), length 52)
 192.168.0.200.58982 > borg.org.http: Flags [.], cksum 0x5f12 (correct), seq 1, ack 1, win 65535, options [nop,nop,TS val 774403433 ecr 3757379566], length 0
 0x0000:  0024 36a2 a7e4 58b0 3577 d723 0800 4500  .$6...X.5w.#..E.
 0x0010:  0034 8b42 4000 4006 e02c c0a8 00c8 4069  .4.B@.@..,....@i
 0x0020:  cd7b e666 0050 a2bf ba40 6591 0b98 8010  .{.f.P...@e.....
 0x0030:  ffff 5f12 0000 0101 080a 2e28 7569 dff5  .._........(ui..
 0x0040:  0fee                                     ..
 16:21:40.181672 IP (tos 0x0, ttl 64, id 2678, offset 0, flags [DF], proto TCP (6), length 391)
 192.168.0.200.58982 > borg.org.http: Flags [P.], cksum 0x02d8 (correct), seq 1:340, ack 1, win 65535, options [nop,nop,TS val 774403433 ecr 3757379566], length 339
 0x0000:  0024 36a2 a7e4 58b0 3577 d723 0800 4500  .$6...X.5w.#..E.
 0x0010:  0187 0a76 4000 4006 5fa6 c0a8 00c8 4069  ...v@.@._.....@i
 0x0020:  cd7b e666 0050 a2bf ba40 6591 0b98 8018  .{.f.P...@e.....
 0x0030:  ffff 02d8 0000 0101 080a 2e28 7569 dff5  ...........(ui..
 0x0040:  0fee 4745 5420 2f20 4854 5450 2f31 2e30  ..GET./.HTTP/1.0
 0x0050:  0d0a 486f 7374 3a20 626f 7267 2e6f 7267  ..Host:.borg.org
 0x0060:  0d0a 4163 6365 7074 3a20 7465 7874 2f68  ..Accept:.text/h
 0x0070:  746d 6c2c 2074 6578 742f 706c 6169 6e2c  tml,.text/plain,
 0x0080:  2074 6578 742f 7367 6d6c 2c20 7669 6465  .text/sgml,.vide
 0x0090:  6f2f 6d70 6567 2c20 696d 6167 652f 6a70  o/mpeg,.image/jp
 0x00a0:  6567 2c20 696d 6167 652f 7469 6666 2c20  eg,.image/tiff,.
 0x00b0:  696d 6167 652f 782d 7267 622c 2069 6d61  image/x-rgb,.ima
 0x00c0:  6765 2f70 6e67 2c20 696d 6167 652f 782d  ge/png,.image/x-
 0x00d0:  7862 6974 6d61 702c 2069 6d61 6765 2f78  xbitmap,.image/x
 0x00e0:  2d78 626d 2c20 696d 6167 652f 6769 662c  -xbm,.image/gif,
 0x00f0:  2061 7070 6c69 6361 7469 6f6e 2f70 6f73  .application/pos
 0x0100:  7473 6372 6970 742c 202a 2f2a 3b71 3d30  tscript,../.;q=0
 0x0110:  2e30 310d 0a41 6363 6570 742d 456e 636f  .01..Accept-Enco
 0x0120:  6469 6e67 3a20 677a 6970 2c20 636f 6d70  ding:.gzip,.comp
 0x0130:  7265 7373 0d0a 4163 6365 7074 2d4c 616e  ress..Accept-Lan
 0x0140:  6775 6167 653a 2065 6e0d 0a55 7365 722d  guage:.en..User-
 0x0150:  4167 656e 743a 204c 796e 782f 322e 382e  Agent:.Lynx/2.8.
 0x0160:  3572 656c 2e31 206c 6962 7777 772d 464d  5rel.1.libwww-FM
 0x0170:  2f32 2e31 3420 5353 4c2d 4d4d 2f31 2e34  /2.14.SSL-MM/1.4
 0x0180:  2e31 204f 7065 6e53 534c 2f30 2e39 2e37  .1.OpenSSL/0.9.7
 0x0190:  6c0d 0a0d 0a                             l....
 16:21:40.465171 IP (tos 0x0, ttl 48, id 7070, offset 0, flags [DF], proto TCP (6), length 52)
 borg.org.http > 192.168.0.200.58982: Flags [.], cksum 0x4485 (correct), seq 1, ack 340, win 6432, options [nop,nop,TS val 3757379592 ecr 774403433], length 0
 0x0000:  58b0 3577 d723 0024 36a2 a7e4 0800 4500  X.5w.#.$6.....E.
 0x0010:  0034 1b9e 4000 3006 5fd1 4069 cd7b c0a8  .4..@.0._.@i.{..
 0x0020:  00c8 0050 e666 6591 0b98 a2bf bb93 8010  ...P.fe.........
 0x0030:  1920 4485 0000 0101 080a dff5 1008 2e28  ..D............(
 0x0040:  7569                                     ui
 16:21:40.473217 IP (tos 0x0, ttl 48, id 7071, offset 0, flags [DF], proto TCP (6), length 1069)
 borg.org.http > 192.168.0.200.58982: Flags [P.], cksum 0x0d75 (correct), seq 1:1018, ack 340, win 6432, options [nop,nop,TS val 3757379592 ecr 774403433], length 1017
 0x0000:  58b0 3577 d723 0024 36a2 a7e4 0800 4500  X.5w.#.$6.....E.
 0x0010:  042d 1b9f 4000 3006 5bd7 4069 cd7b c0a8  .-..@.0.[.@i.{..
 0x0020:  00c8 0050 e666 6591 0b98 a2bf bb93 8018  ...P.fe.........
 0x0030:  1920 0d75 0000 0101 080a dff5 1008 2e28  ...u...........(
 0x0040:  7569 4854 5450 2f31 2e31 2032 3030 204f  uiHTTP/1.1.200.O
 0x0050:  4b0d 0a44 6174 653a 2053 6174 2c20 3035  K..Date:.Sat,.05
 0x0060:  204a 756e 2032 3031 3020 3134 3a32 313a  .Jun.2010.14:21:
 0x0070:  3430 2047 4d54 0d0a 5365 7276 6572 3a20  40.GMT..Server:.
 0x0080:  4170 6163 6865 2f31 2e33 2e32 3720 2855  Apache/1.3.27.(U
 0x0090:  6e69 7829 2020 2852 6564 2d48 6174 2f4c  nix)..(Red-Hat/L
 0x00a0:  696e 7578 2920 6d6f 645f 7079 7468 6f6e  inux).mod_python
 0x00b0:  2f32 2e37 2e38 2050 7974 686f 6e2f 312e  /2.7.8.Python/1.
 0x00c0:  352e 3220 6d6f 645f 7373 6c2f 322e 382e  5.2.mod_ssl/2.8.
 0x00d0:  3132 204f 7065 6e53 534c 2f30 2e39 2e36  12.OpenSSL/0.9.6
 0x00e0:  6220 4441 562f 312e 302e 3220 5048 502f  b.DAV/1.0.2.PHP/
 0x00f0:  342e 312e 3220 6d6f 645f 7065 726c 2f31  4.1.2.mod_perl/1
 0x0100:  2e32 3620 6d6f 645f 7468 726f 7474 6c65  .26.mod_throttle
 0x0110:  2f33 2e31 2e32 0d0a 4c61 7374 2d4d 6f64  /3.1.2..Last-Mod
 0x0120:  6966 6965 643a 2046 7269 2c20 3036 2044  ified:.Fri,.06.D
 0x0130:  6563 2032 3030 3220 3231 3a35 363a 3538  ec.2002.21:56:58
 0x0140:  2047 4d54 0d0a 4554 6167 3a20 2265 6638  .GMT..ETag:."ef8
 0x0150:  3832 2d32 3830 2d33 6466 3131 6432 6122  82-280-3df11d2a"
 0x0160:  0d0a 4163 6365 7074 2d52 616e 6765 733a  ..Accept-Ranges:
 0x0170:  2062 7974 6573 0d0a 436f 6e74 656e 742d  .bytes..Content-
 0x0180:  4c65 6e67 7468 3a20 3634 300d 0a43 6f6e  Length:.640..Con
 0x0190:  6e65 6374 696f 6e3a 2063 6c6f 7365 0d0a  nection:.close..
 0x01a0:  436f 6e74 656e 742d 5479 7065 3a20 7465  Content-Type:.te
 0x01b0:  7874 2f68 746d 6c0d 0a0d 0a3c 2144 4f43  xt/html....<!DOC
 0x01c0:  5459 5045 2048 544d 4c20 5055 424c 4943  TYPE.HTML.PUBLIC
 0x01d0:  2022 2d2f 2f57 3343 2f2f 4454 4420 4854  ."-//W3C//DTD.HT
 0x01e0:  4d4c 2033 2e32 2046 696e 616c 2f2f 454e  ML.3.2.Final//EN
 0x01f0:  223e 0a3c 4854 4d4c 3e0a 203c 4845 4144  ">.<HTML>..<HEAD
 0x0200:  3e0a 2020 3c54 4954 4c45 3e4d 696e 696d  >...<TITLE>Minim
 0x0210:  616c 2057 6562 2050 6167 6520 666f 7220  al.Web.Page.for.
 0x0220:  626f 7267 2e6f 7267 3c2f 5449 544c 453e  borg.org</TITLE>
 0x0230:  0a20 3c2f 4845 4144 3e0a 3c21 2d2d 2042  ..</HEAD>.<!--.B
 0x0240:  6163 6b67 726f 756e 6420 7768 6974 652c  ackground.white,
 0x0250:  206c 696e 6b73 2062 6c75 6520 2875 6e76  .links.blue.(unv
 0x0260:  6973 6974 6564 292c 206e 6176 7920 2876  isited),.navy.(v
 0x0270:  6973 6974 6564 292c 2072 6564 2028 6163  isited),.red.(ac
 0x0280:  7469 7665 2920 2d2d 3e0a 203c 424f 4459  tive).-->..<BODY
 0x0290:  2042 4743 4f4c 4f52 3d22 2346 4646 4646  .BGCOLOR="#FFFFF
 0x02a0:  4622 3e0a 0a20 203c 4831 2041 4c49 474e  F">....<H1.ALIGN
 0x02b0:  3d22 4345 4e54 4552 223e 626f 7267 2e6f  ="CENTER">borg.o
 0x02c0:  7267 3c2f 4831 3e0a 2020 3c48 5220 5749  rg</H1>...<HR.WI
 0x02d0:  4454 483d 2235 3025 223e 0a0a 2020 3c48  DTH="50%">....<H
 0x02e0:  3220 414c 4947 4e3d 2243 454e 5445 5222  2.ALIGN="CENTER"
 0x02f0:  3e54 6865 7265 2069 7320 6e6f 7468 696e  >There.is.nothin
 0x0300:  6720 6865 7265 2e20 2049 6e63 6c75 6469  g.here...Includi
 0x0310:  6e67 206e 6f74 6869 6e67 2074 6861 740a  ng.nothing.that.
 0x0320:  2020 6861 7320 616e 7974 6869 6e67 2074  ..has.anything.t
 0x0330:  6f20 646f 2077 6974 6820 2254 6865 2042  o.do.with."The.B
 0x0340:  6f72 6722 2e20 3c62 723e 3c62 723e 2062  org"..<br><br>.b
 0x0350:  6f72 672e 6f72 6720 6973 2073 696d 706c  org.org.is.simpl
 0x0360:  7920 7468 650a 2020 7065 7273 6f6e 616c  y.the...personal
 0x0370:  2064 6f6d 6169 6e20 6f66 2073 6f6d 656f  .domain.of.someo
 0x0380:  6e65 2077 686f 7365 206c 6173 7420 6e61  ne.whose.last.na
 0x0390:  6d65 2069 7320 426f 7267 2c20 616e 6420  me.is.Borg,.and.
 0x03a0:  7768 6f73 6520 6e61 6d65 0a20 2068 6173  whose.name...has
 0x03b0:  203c 693e 6265 656e 3c2f 693e 2042 6f72  .<i>been</i>.Bor
 0x03c0:  6720 7369 6e63 6520 6c6f 6e67 2062 6566  g.since.long.bef
 0x03d0:  6f72 6520 4361 7074 6169 6e20 4b69 726b  ore.Captain.Kirk
 0x03e0:  2065 7665 7220 746f 6f6b 2061 0a20 2068  .ever.took.a...h
 0x03f0:  656c 6d2e 2020 536f 7272 7920 746f 2064  elm...Sorry.to.d
 0x0400:  6973 6170 706f 696e 7420 616e 7920 7363  isappoint.any.sc
 0x0410:  6965 6e63 6520 6669 6374 696f 6e20 6661  ience.fiction.fa
 0x0420:  6e73 2e3c 2f48 323e 0a0a 203c 2f42 4f44  ns.</H2>...</BOD
 0x0430:  593e 0a3c 2f48 544d 4c3e 0a              Y>.</HTML>.
 16:21:40.473223 IP (tos 0x0, ttl 48, id 7072, offset 0, flags [DF], proto TCP (6), length 52)
 borg.org.http > 192.168.0.200.58982: Flags [F.], cksum 0x408b (correct), seq 1018, ack 340, win 6432, options [nop,nop,TS val 3757379592 ecr 774403433], length 0
 0x0000:  58b0 3577 d723 0024 36a2 a7e4 0800 4500  X.5w.#.$6.....E.
 0x0010:  0034 1ba0 4000 3006 5fcf 4069 cd7b c0a8  .4..@.0._.@i.{..
 0x0020:  00c8 0050 e666 6591 0f91 a2bf bb93 8011  ...P.fe.........
 0x0030:  1920 408b 0000 0101 080a dff5 1008 2e28  ..@............(
 0x0040:  7569                                     ui
 16:21:40.473329 IP (tos 0x0, ttl 64, id 85, offset 0, flags [DF], proto TCP (6), length 52)
 192.168.0.200.58982 > borg.org.http: Flags [.], cksum 0x59fa (correct), seq 340, ack 1018, win 65455, options [nop,nop,TS val 774403435 ecr 3757379592], length 0
 0x0000:  0024 36a2 a7e4 58b0 3577 d723 0800 4500  .$6...X.5w.#..E.
 0x0010:  0034 0055 4000 4006 6b1a c0a8 00c8 4069  .4.U@.@.k.....@i
 0x0020:  cd7b e666 0050 a2bf bb93 6591 0f91 8010  .{.f.P....e.....
 0x0030:  ffaf 59fa 0000 0101 080a 2e28 756b dff5  ..Y........(uk..
 0x0040:  1008                                     ..
 16:21:40.473385 IP (tos 0x0, ttl 64, id 112, offset 0, flags [DF], proto TCP (6), length 52)
 192.168.0.200.58982 > borg.org.http: Flags [.], cksum 0x59f9 (correct), seq 340, ack 1019, win 65455, options [nop,nop,TS val 774403435 ecr 3757379592], length 0
 0x0000:  0024 36a2 a7e4 58b0 3577 d723 0800 4500  .$6...X.5w.#..E.
 0x0010:  0034 0070 4000 4006 6aff c0a8 00c8 4069  .4.p@.@.j.....@i
 0x0020:  cd7b e666 0050 a2bf bb93 6591 0f92 8010  .{.f.P....e.....
 0x0030:  ffaf 59f9 0000 0101 080a 2e28 756b dff5  ..Y........(uk..
 0x0040:  1008                                     ..
 16:21:40.474607 IP (tos 0x0, ttl 64, id 24360, offset 0, flags [DF], proto TCP (6), length 52)
 192.168.0.200.58982 > borg.org.http: Flags [F.], cksum 0x59a8 (correct), seq 340, ack 1019, win 65535, options [nop,nop,TS val 774403435 ecr 3757379592], length 0
 0x0000:  0024 36a2 a7e4 58b0 3577 d723 0800 4500  .$6...X.5w.#..E.
 0x0010:  0034 5f28 4000 4006 0c47 c0a8 00c8 4069  .4_(@.@..G....@i
 0x0020:  cd7b e666 0050 a2bf bb93 6591 0f92 8011  .{.f.P....e.....
 0x0030:  ffff 59a8 0000 0101 080a 2e28 756b dff5  ..Y........(uk..
 0x0040:  1008                                     ..
 16:21:40.772356 IP (tos 0x0, ttl 48, id 7073, offset 0, flags [DF], proto TCP (6), length 52)
 borg.org.http > 192.168.0.200.58982: Flags [.], cksum 0x406d (correct), seq 1019, ack 341, win 6432, options [nop,nop,TS val 3757379619 ecr 774403435], length 0
 0x0000:  58b0 3577 d723 0024 36a2 a7e4 0800 4500  X.5w.#.$6.....E.
 0x0010:  0034 1ba1 4000 3006 5fce 4069 cd7b c0a8  .4..@.0._.@i.{..
 0x0020:  00c8 0050 e666 6591 0f92 a2bf bb94 8010  ...P.fe.........
 0x0030:  1920 406d 0000 0101 080a dff5 1023 2e28  ..@m.........#.(
 0x0040:  756b                                     uk
 */
#include <K/Defines/KDefinitions.h>
#include "TUsermodeNetwork.h"
#include "Emulator/Log/TLog.h"

//
// Handle all kinds of network packages
//

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>


// TODO: ARP
// TODO:  +--- send
// TODO:  +--- receive
// TODO: IPv4
// TODO:  +--- TCP
// TODO:  |		+--- send
// TODO:  |		+--- receive
// TODO:  +--- UDP
// TODO:		+--- send
// TODO:		+--- receive

class Packet
{
public:
	Packet(KUInt8 *data, KUInt32 size) {
		mData = (KUInt8*)malloc(size);
		memcpy(mData, data, size);
		mSize = size;
	}
	~Packet() {
		if (mData) 
			free(mData);
	}
	KUInt8 *Data() { return mData; }
	KUInt32 Size() { return mSize; }
	Packet *prev, *next;
private:
	KUInt8 *mData;
	KUInt32 mSize;
};


class PacketHandler 
{
public:
	PacketHandler(TUsermodeNetwork *h) :
		net(h)
	{
	}
	virtual ~PacketHandler() {
	}
	virtual int send(KUInt8 *data, KUInt32 size) = 0;
	virtual int timer() { return 0; }
	static int canHandle(KUInt8 *data, KUInt32 size) { return 0; }
	PacketHandler *prev, *next;
	TUsermodeNetwork *net;
};


class ARPPacketHandler : public PacketHandler 
{
public:
	ARPPacketHandler(TUsermodeNetwork *h) :
		PacketHandler(h)
	{
	}
	~ARPPacketHandler() {
	}
	virtual int send(KUInt8 *d, KUInt32 n) {
		if ( (n!=42) || ((d[12]<<8)|d[13])!=0x0806 )
			return -1;
		// build the reply package from scratch
		return 0;
	}
};


class TCPPacketHandler : public PacketHandler 
{
public:
	TCPPacketHandler(TUsermodeNetwork *h) :
		PacketHandler(h)
	{
	}
	~TCPPacketHandler() {
	}
	virtual int send(KUInt8 *d, KUInt32 n) {
		if ( ((d[12]<<8)|d[13])!=0x0800 ) return -1;
		if ( d[23]!=6 ) return -1;
		if ( (d[47]&0x3f)==0x02) { // start the connection
			printf("-----> tell Newton that we connected to the other side.\n");
				net->LogPacket(d,n);
			KUInt8 r[58];
			memcpy(r, d, 58);		// original package
			memcpy(r+0, d+6, 6);	// MAC
			memcpy(r+6, d+0, 6);	// MAC
			memcpy(r+26, d+30, 4);	// IP
			memcpy(r+30, d+26, 4);	// IP
			memcpy(r+34, d+36, 2);	// port
			memcpy(r+36, d+34, 2);	// port
			KUInt32 n = 1000;
			*((KUInt32*)(r+38)) = htonl(n); // 38 seq# (4 bytes)
			n = ntohl(*((KUInt32*)(d+38)));
			*((KUInt32*)(r+42)) = htonl(n+1); // 42 ack# (4 bytes)
			r[47] = 0x12;			// flags
			net->SetIPv4Checksum(r, 58);
			net->SetTCPChecksum(r, 58);
				net->LogPacket(r,58);
			net->Enqueue(new Packet(r, 58));
			return 0;
		}
		if ( (d[47]&0x3f)==0x10) { // start the connection
			printf("-----> Connection established!\n");
			net->LogBuffer(d, n);
			net->LogPacket(d, n);
			// if we have a payload, send the packet
			if (n>54) {
				KUInt8 r[] = {
					0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x08, 0x00, 0x45, 0x00, 
					0x04, 0x2d, 0x25, 0x05, 0x40, 0x00, 0x2f, 0x06, 0x53, 0x71, 0x40, 0x69, 0xcd, 0x7b, 0xc0, 0xa8, 
					0x00, 0xc8, 0x00, 0x50, 0xdd, 0x43, 0xa5, 0xf8, 0x45, 0xac, 0x63, 0xe1, 0x9b, 0x87, 0x80, 0x18, 
					0x19, 0x20, 0x16, 0x05, 0x00, 0x00, 0x01, 0x01, 0x08, 0x0a, 0xdf, 0xf0, 0x60, 0xac, 0x2e, 0x28, 
					0x0b, 0xcc, 0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x20, 0x32, 0x30, 0x30, 0x20, 0x4f, 
					0x4b, 0x0d, 0x0a, 0x44, 0x61, 0x74, 0x65, 0x3a, 0x20, 0x53, 0x61, 0x74, 0x2c, 0x20, 0x30, 0x35, 
					0x20, 0x4a, 0x75, 0x6e, 0x20, 0x32, 0x30, 0x31, 0x30, 0x20, 0x31, 0x33, 0x3a, 0x33, 0x30, 0x3a, 
					0x33, 0x30, 0x20, 0x47, 0x4d, 0x54, 0x0d, 0x0a, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x3a, 0x20, 
					0x41, 0x70, 0x61, 0x63, 0x68, 0x65, 0x2f, 0x31, 0x2e, 0x33, 0x2e, 0x32, 0x37, 0x20, 0x28, 0x55, 
					0x6e, 0x69, 0x78, 0x29, 0x20, 0x20, 0x28, 0x52, 0x65, 0x64, 0x2d, 0x48, 0x61, 0x74, 0x2f, 0x4c, 
					0x69, 0x6e, 0x75, 0x78, 0x29, 0x20, 0x6d, 0x6f, 0x64, 0x5f, 0x70, 0x79, 0x74, 0x68, 0x6f, 0x6e, 
					0x2f, 0x32, 0x2e, 0x37, 0x2e, 0x38, 0x20, 0x50, 0x79, 0x74, 0x68, 0x6f, 0x6e, 0x2f, 0x31, 0x2e, 
					0x35, 0x2e, 0x32, 0x20, 0x6d, 0x6f, 0x64, 0x5f, 0x73, 0x73, 0x6c, 0x2f, 0x32, 0x2e, 0x38, 0x2e, 
					0x31, 0x32, 0x20, 0x4f, 0x70, 0x65, 0x6e, 0x53, 0x53, 0x4c, 0x2f, 0x30, 0x2e, 0x39, 0x2e, 0x36, 
					0x62, 0x20, 0x44, 0x41, 0x56, 0x2f, 0x31, 0x2e, 0x30, 0x2e, 0x32, 0x20, 0x50, 0x48, 0x50, 0x2f, 
					0x34, 0x2e, 0x31, 0x2e, 0x32, 0x20, 0x6d, 0x6f, 0x64, 0x5f, 0x70, 0x65, 0x72, 0x6c, 0x2f, 0x31, 
					0x2e, 0x32, 0x36, 0x20, 0x6d, 0x6f, 0x64, 0x5f, 0x74, 0x68, 0x72, 0x6f, 0x74, 0x74, 0x6c, 0x65, 
					0x2f, 0x33, 0x2e, 0x31, 0x2e, 0x32, 0x0d, 0x0a, 0x4c, 0x61, 0x73, 0x74, 0x2d, 0x4d, 0x6f, 0x64, 
					0x69, 0x66, 0x69, 0x65, 0x64, 0x3a, 0x20, 0x46, 0x72, 0x69, 0x2c, 0x20, 0x30, 0x36, 0x20, 0x44, 
					0x65, 0x63, 0x20, 0x32, 0x30, 0x30, 0x32, 0x20, 0x32, 0x31, 0x3a, 0x35, 0x36, 0x3a, 0x35, 0x38, 
					0x20, 0x47, 0x4d, 0x54, 0x0d, 0x0a, 0x45, 0x54, 0x61, 0x67, 0x3a, 0x20, 0x22, 0x65, 0x66, 0x38, 
					0x38, 0x32, 0x2d, 0x32, 0x38, 0x30, 0x2d, 0x33, 0x64, 0x66, 0x31, 0x31, 0x64, 0x32, 0x61, 0x22, 
					0x0d, 0x0a, 0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x2d, 0x52, 0x61, 0x6e, 0x67, 0x65, 0x73, 0x3a, 
					0x20, 0x62, 0x79, 0x74, 0x65, 0x73, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 
					0x4c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3a, 0x20, 0x36, 0x34, 0x30, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 
					0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65, 0x0d, 0x0a, 
					0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 
					0x78, 0x74, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0x0d, 0x0a, 0x0d, 0x0a, 0x3c, 0x21, 0x44, 0x4f, 0x43, 
					0x54, 0x59, 0x50, 0x45, 0x20, 0x48, 0x54, 0x4d, 0x4c, 0x20, 0x50, 0x55, 0x42, 0x4c, 0x49, 0x43, 
					0x20, 0x22, 0x2d, 0x2f, 0x2f, 0x57, 0x33, 0x43, 0x2f, 0x2f, 0x44, 0x54, 0x44, 0x20, 0x48, 0x54, 
					0x4d, 0x4c, 0x20, 0x33, 0x2e, 0x32, 0x20, 0x46, 0x69, 0x6e, 0x61, 0x6c, 0x2f, 0x2f, 0x45, 0x4e, 
					0x22, 0x3e, 0x0a, 0x3c, 0x48, 0x54, 0x4d, 0x4c, 0x3e, 0x0a, 0x20, 0x3c, 0x48, 0x45, 0x41, 0x44, 
					0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x54, 0x49, 0x54, 0x4c, 0x45, 0x3e, 0x4d, 0x69, 0x6e, 0x69, 0x6d, 
					0x61, 0x6c, 0x20, 0x57, 0x65, 0x62, 0x20, 0x50, 0x61, 0x67, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x20, 
					0x62, 0x6f, 0x72, 0x67, 0x2e, 0x6f, 0x72, 0x67, 0x3c, 0x2f, 0x54, 0x49, 0x54, 0x4c, 0x45, 0x3e, 
					0x0a, 0x20, 0x3c, 0x2f, 0x48, 0x45, 0x41, 0x44, 0x3e, 0x0a, 0x3c, 0x21, 0x2d, 0x2d, 0x20, 0x42, 
					0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x20, 0x77, 0x68, 0x69, 0x74, 0x65, 0x2c, 
					0x20, 0x6c, 0x69, 0x6e, 0x6b, 0x73, 0x20, 0x62, 0x6c, 0x75, 0x65, 0x20, 0x28, 0x75, 0x6e, 0x76, 
					0x69, 0x73, 0x69, 0x74, 0x65, 0x64, 0x29, 0x2c, 0x20, 0x6e, 0x61, 0x76, 0x79, 0x20, 0x28, 0x76, 
					0x69, 0x73, 0x69, 0x74, 0x65, 0x64, 0x29, 0x2c, 0x20, 0x72, 0x65, 0x64, 0x20, 0x28, 0x61, 0x63, 
					0x74, 0x69, 0x76, 0x65, 0x29, 0x20, 0x2d, 0x2d, 0x3e, 0x0a, 0x20, 0x3c, 0x42, 0x4f, 0x44, 0x59, 
					0x20, 0x42, 0x47, 0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x3d, 0x22, 0x23, 0x46, 0x46, 0x46, 0x46, 0x46, 
					0x46, 0x22, 0x3e, 0x0a, 0x0a, 0x20, 0x20, 0x3c, 0x48, 0x31, 0x20, 0x41, 0x4c, 0x49, 0x47, 0x4e, 
					0x3d, 0x22, 0x43, 0x45, 0x4e, 0x54, 0x45, 0x52, 0x22, 0x3e, 0x62, 0x6f, 0x72, 0x67, 0x2e, 0x6f, 
					0x72, 0x67, 0x3c, 0x2f, 0x48, 0x31, 0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x48, 0x52, 0x20, 0x57, 0x49, 
					0x44, 0x54, 0x48, 0x3d, 0x22, 0x35, 0x30, 0x25, 0x22, 0x3e, 0x0a, 0x0a, 0x20, 0x20, 0x3c, 0x48, 
					0x32, 0x20, 0x41, 0x4c, 0x49, 0x47, 0x4e, 0x3d, 0x22, 0x43, 0x45, 0x4e, 0x54, 0x45, 0x52, 0x22, 
					0x3e, 0x54, 0x68, 0x65, 0x72, 0x65, 0x20, 0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 
					0x67, 0x20, 0x68, 0x65, 0x72, 0x65, 0x2e, 0x20, 0x20, 0x49, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x69, 
					0x6e, 0x67, 0x20, 0x6e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x74, 0x68, 0x61, 0x74, 0x0a, 
					0x20, 0x20, 0x68, 0x61, 0x73, 0x20, 0x61, 0x6e, 0x79, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x74, 
					0x6f, 0x20, 0x64, 0x6f, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x22, 0x54, 0x68, 0x65, 0x20, 0x42, 
					0x6f, 0x72, 0x67, 0x22, 0x2e, 0x20, 0x3c, 0x62, 0x72, 0x3e, 0x3c, 0x62, 0x72, 0x3e, 0x20, 0x62, 
					0x6f, 0x72, 0x67, 0x2e, 0x6f, 0x72, 0x67, 0x20, 0x69, 0x73, 0x20, 0x73, 0x69, 0x6d, 0x70, 0x6c, 
					0x79, 0x20, 0x74, 0x68, 0x65, 0x0a, 0x20, 0x20, 0x70, 0x65, 0x72, 0x73, 0x6f, 0x6e, 0x61, 0x6c, 
					0x20, 0x64, 0x6f, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x73, 0x6f, 0x6d, 0x65, 0x6f, 
					0x6e, 0x65, 0x20, 0x77, 0x68, 0x6f, 0x73, 0x65, 0x20, 0x6c, 0x61, 0x73, 0x74, 0x20, 0x6e, 0x61, 
					0x6d, 0x65, 0x20, 0x69, 0x73, 0x20, 0x42, 0x6f, 0x72, 0x67, 0x2c, 0x20, 0x61, 0x6e, 0x64, 0x20, 
					0x77, 0x68, 0x6f, 0x73, 0x65, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x0a, 0x20, 0x20, 0x68, 0x61, 0x73, 
					0x20, 0x3c, 0x69, 0x3e, 0x62, 0x65, 0x65, 0x6e, 0x3c, 0x2f, 0x69, 0x3e, 0x20, 0x42, 0x6f, 0x72, 
					0x67, 0x20, 0x73, 0x69, 0x6e, 0x63, 0x65, 0x20, 0x6c, 0x6f, 0x6e, 0x67, 0x20, 0x62, 0x65, 0x66, 
					0x6f, 0x72, 0x65, 0x20, 0x43, 0x61, 0x70, 0x74, 0x61, 0x69, 0x6e, 0x20, 0x4b, 0x69, 0x72, 0x6b, 
					0x20, 0x65, 0x76, 0x65, 0x72, 0x20, 0x74, 0x6f, 0x6f, 0x6b, 0x20, 0x61, 0x0a, 0x20, 0x20, 0x68, 
					0x65, 0x6c, 0x6d, 0x2e, 0x20, 0x20, 0x53, 0x6f, 0x72, 0x72, 0x79, 0x20, 0x74, 0x6f, 0x20, 0x64, 
					0x69, 0x73, 0x61, 0x70, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x73, 0x63, 
					0x69, 0x65, 0x6e, 0x63, 0x65, 0x20, 0x66, 0x69, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x66, 0x61, 
					0x6e, 0x73, 0x2e, 0x3c, 0x2f, 0x48, 0x32, 0x3e, 0x0a, 0x0a, 0x20, 0x3c, 0x2f, 0x42, 0x4f, 0x44, 
					0x59, 0x3e, 0x0a, 0x3c, 0x2f, 0x48, 0x54, 0x4d, 0x4c, 0x3e, 0x0a
				};
				n = sizeof(r);
				memcpy(r+0, d+6, 6);	// MAC
				memcpy(r+6, d+0, 6);	// MAC
				memcpy(r+18, d+18, 2);	// ID
				memcpy(r+20, d+20, 2);	// Frag
				memcpy(r+26, d+30, 4);	// IP
				memcpy(r+30, d+26, 4);	// IP
				memcpy(r+34, d+36, 2);	// port
				memcpy(r+36, d+34, 2);	// port
				memcpy(r+38, d+42, 4);	// 38 seq# (4 bytes)
				KUInt32 seq = ntohl(*((KUInt32*)(d+38)));
				*((KUInt32*)(r+42)) = htonl(seq+1); // 42 ack# (4 bytes)
				r[47] = 0x10;			// flags
				net->SetIPv4Checksum(r, n);
				net->SetTCPChecksum(r, n);
				net->LogPacket(r,n);
				net->Enqueue(new Packet(r, n));
			}
			return 0;
		}
		return -1;
	}
	virtual int timer() {
		return -1;
	}
	static int canHandle(KUInt8 *d, KUInt32 n) {
		if ( ((d[12]<<8)|d[13])!=0x0800 ) return 0;
		if ( d[23]!=6 ) return 0;
		if ( (d[47]&0x3f)!=0x02) return 0; // SYN set
		return 1;
	}
};


TUsermodeNetwork::TUsermodeNetwork(TLog* inLog) :
	TNetworkManager( inLog ),
	mFirstPacketHandler( 0L ),
	mFirstPacket( 0L ),
	mLastPacket( 0L )
{
	// create the package pipe for the Newton package receiver
	// create the active handler list
	//AddPacketHandler(new ARPPacketHandler());
}


TUsermodeNetwork::~TUsermodeNetwork()
{
	// release the package pipe
	while (mFirstPacket)
		DropPacket();
	// release all package handlers
	// TODO: delete handlers
	// release all other resources
}


/*
 * This function handles packet that are sent by the Newton to the outside 
 * world. In user mode, these packages are emulated for information that is 
 * already available on the host, or forwarded to a host socket. A possible
 * reply must be interpreted and a raw package must be generated for the Newton.
 */
int TUsermodeNetwork::SendPacket(KUInt8 *data, KUInt32 size)
{
	int err = -1;
	
	// offer this package to all active handlers
	PacketHandler *ph = mFirstPacketHandler;
	while (ph && err) {
		err = ph->send(data, size);
		ph = ph->next;
	}
	
	// now offer the package to possible new handlers
	if (err) {
		if (TCPPacketHandler::canHandle(data, size)) {
			ph = new TCPPacketHandler(this);
			AddPacketHandler(ph);
			err = ph->send(data, size);
		}
	}
	/*
	if (err) {
		if (UDPPacketHandler::canHandle(data, size)) {
			ph = new UDPPacketHandler();
			AddPacketHandler(ph);
			err = ph->send(data, size);
		}
	}
	 */
	if (!err) 
		return err;
	
	// FIXME: simulate the ARP request for now
	// see: man 3 getaddrinfo()
	static KUInt8 b1[] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0xc0, 0xa8, 0x00, 0xc6,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01,
	};
	static KUInt8 r1[] = {
		0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0xc0, 0xa8, 0x00, 0x01, 
		0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0xc0, 0xa8, 0x00, 0xc6,
	};
	if (size==sizeof(b1) && memcmp(b1, data, size)==0) {
		Enqueue(new Packet(r1, sizeof(r1)));
		return 0;
	}
	
	// FIXME: simulate the DNS request
	static KUInt8 b2[] = {
		0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x08, 0x00, 0x45, 0x00,
		0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0x40, 0x11, 0xf8, 0x9f, 0xc0, 0xa8, 0x00, 0xc6, 0xc0, 0xa8,
		0x00, 0x01, 0x08, 0x00, 0x00, 0x35, 0x00, 0x22, 0xc3, 0x0e, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x62, 0x6f, 0x72, 0x67, 0x03, 0x6f, 0x72, 0x67, 0x00,
		0x00, 0x01, 0x00, 0x01
	};
	static KUInt8 r2[] = {
		0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23, 0x00, 0x24, 0x36, 0xa2, 0xa7, 0xe4, 0x08, 0x00, 0x45, 0x00, 
		0x00, 0x46, 0x05, 0x97, 0x00, 0x00, 0x40, 0x11, 0xf2, 0xf6, 0xc0, 0xa8, 0x00, 0x01, 0xc0, 0xa8, 
		0x00, 0xc8, 0x00, 0x35, 0xcd, 0x0c, 0x00, 0x32, 0xaa, 0xe8, 0x00, 0x01, 0x81, 0x80, 0x00, 0x01, 
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x62, 0x6f, 0x72, 0x67, 0x03, 0x6f, 0x72, 0x67, 0x00, 
		0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x04, 0x7e, 0x00, 0x04, 
		0x40, 0x69, 0xcd, 0x7b,                   
	};
	if (size==sizeof(b2) && memcmp(b2, data, size)==0) {
		r2[34] = b2[36]; r2[35] = b2[37];
		r2[36] = b2[34]; r2[37] = b2[35]; // swap ports
		r2[33] = 198;
		SetIPv4Checksum(r2, sizeof(r2));
		SetUDPChecksum(r2, sizeof(r2));
		LogPacket(b2, sizeof(b2));
		LogPacket(r2, sizeof(r2));
		Enqueue(new Packet(r2, sizeof(r2)));
		return 0;
	}
	
	
	// if we can't interprete the package, we offer some debugging information
	if (mLog) {
		mLog->LogLine("\nTUsermodeNetwork: Newton is sending an unsupported package:");
		LogBuffer(data, size);
		LogPacket(data, size);
	}
	return 0;
}


int TUsermodeNetwork::GetDeviceAddress(KUInt8 *data, KUInt32 size)
{
	// TODO: of course we need the true MAC of this ethernet card
	// see: ioctl ? getifaddrs ? http://othermark.livejournal.com/3005.html
	static KUInt8 gLocalMAC[]   = { 0x58, 0xb0, 0x35, 0x77, 0xd7, 0x23 };
	assert(size==6);
	memcpy(data, gLocalMAC, 6);
	return 0;
}


KUInt32 TUsermodeNetwork::DataAvailable()
{	
	if (mLastPacket) {
		return mLastPacket->Size();
	} else {
		return 0;
	}
}


int TUsermodeNetwork::ReceiveData(KUInt8 *data, KUInt32 size)
{
	Packet *pkt = mLastPacket;
	if (pkt) {
		assert(pkt->Size()==size);
		// copy the data over
		memcpy(data, pkt->Data(), size);
		// remove this package from the pipe
		DropPacket();
		return 0;
	} else {
		return -1;
	}
}


/**
 * Add a new handler for a package type to the list.
 */
void TUsermodeNetwork::AddPacketHandler(PacketHandler *inPacketHandler) 
{
	PacketHandler *n = mFirstPacketHandler;
	if (n) 
		n->prev = inPacketHandler;
	else
		mLastPacketHandler = inPacketHandler;
	inPacketHandler->next = n;
	inPacketHandler->prev = 0;
	mFirstPacketHandler = inPacketHandler;
}


/**
 * Add a new package to the beginning of the pipe.
 * This makes the give block ready to be sent at the next possible occasion.
 *
 * \param inPacket the package that will be queued
 */
void TUsermodeNetwork::Enqueue(Packet *inPacket)
{
	Packet *n = mFirstPacket;
	if (n) 
		n->prev = inPacket;
	else
		mLastPacket = inPacket;
	inPacket->next = n;
	inPacket->prev = 0;
	mFirstPacket = inPacket;
}


/**
 * Drops the last packet in the pipe.
 */
void TUsermodeNetwork::DropPacket()
{
	Packet *pkt = mLastPacket;
	if (pkt) {
		Packet *prevPkt = pkt->prev;
		if (prevPkt) 
			prevPkt->next = 0L;
		else
			mFirstPacket = 0L;
		mLastPacket = prevPkt;
		delete pkt;
	}
}

// -------------------------------------------------------------------------- //
//  * SetReadFDSet(fd_set* ioFDSet)
// -------------------------------------------------------------------------- //
int
TUsermodeNetwork::SetReadFDSet(fd_set* ioFDSet) {
	// We don't have fds yet (those would be connected sockets once the Newton
	// can establish connections with this interface).
    return 0;
}

// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
