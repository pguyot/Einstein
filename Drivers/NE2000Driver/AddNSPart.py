#!/usr/bin/env python3
# ==============================
# File:            AddNSPart.py
# Project:         Einstein
#
# Copyright 2026 by Matthias Melcher.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
# ==============================

"""
Add a NewtonScript part to a package with one protocol part.

ELFtoPKG makes a package with the protocol part of a driver. NE2K.pkg also
needs the NewtonScript part that registers the driver with NIE when the
package is installed (NE2K.auto, made with NTK in 1997, see Install and
Project Data).

Both kinds of parts depend on their place in the package: the references in
a frame part are offsets from the start of the package, and so are the
offsets in the relocation data of a protocol part and the values of the
words it relocates. So the NewtonScript part goes first, at the offset where
NTK put it (the directory is padded to get there), and the protocol part
follows, with its relocation data and the relocated words moved.

Usage: AddNSPart.py protocol.pkg nspart.bin offset type flags name copyright out.pkg
  offset the offset of the NewtonScript part in the package it came from
  type   the part type, e.g. auto
  flags  the part flags, e.g. 0x81 (NewtonScript part, notify)
"""

import struct
import sys


def utf16(s):
    return (s + "\0").encode("utf-16-be")


def relocatedWords(reloc):
    """The offsets of the relocated words, from the start of the package."""
    reserved, size, pageSize, numEntries, base = struct.unpack(">IIIII", reloc[0:20])
    offsets = []
    i = 20
    for _ in range(numEntries):
        page, count = struct.unpack(">HH", reloc[i:i + 4])
        offsets += [page * pageSize + o * 4 for o in reloc[i + 4:i + 4 + count]]
        i = (i + 4 + count + 3) & ~3
    return offsets


def moveRelocations(reloc, delta):
    """
    The relocation data lists the words to relocate by their offset from the
    start of the package, in pages: header (reserved, size, page size, number
    of entries, base address), then per page its number, the number of
    offsets, and the offsets in words, padded to a word. A bigger directory
    moves the parts, so the offsets move by delta.
    """
    reserved, size, pageSize, numEntries, base = struct.unpack(">IIIII", reloc[0:20])
    offsets = []
    i = 20
    for _ in range(numEntries):
        page, count = struct.unpack(">HH", reloc[i:i + 4])
        offsets += [page * pageSize + o * 4 + delta for o in reloc[i + 4:i + 4 + count]]
        i = (i + 4 + count + 3) & ~3
    pages = {}
    for o in sorted(offsets):
        pages.setdefault(o // pageSize, []).append((o % pageSize) // 4)
    entries = bytearray()
    for page in sorted(pages):
        entries += struct.pack(">HH", page, len(pages[page])) + bytes(pages[page])
        while len(entries) % 4:
            entries += b"\0"
    return struct.pack(">IIIII", reserved, 20 + len(entries), pageSize, len(pages), base) + entries


def main():
    if len(sys.argv) != 9:
        sys.exit(__doc__)
    inPkg, nsPart, nsStart, partType, partFlags, name, copyright, outPkg = sys.argv[1:]
    nsStart = int(nsStart, 0)
    d = open(inPkg, "rb").read()
    part = open(nsPart, "rb").read()

    if d[:8] not in (b"package0", b"package1"):
        sys.exit("%s is not a Newton package" % inPkg)
    flags, = struct.unpack(">I", d[12:16])
    dirSize, numParts = struct.unpack(">II", d[44:52])
    if numParts != 1:
        sys.exit("%s must have exactly one part" % inPkg)
    # The relocation data follows the directory (package1 with the flag).
    relocSize = 0
    if d[:8] == b"package1" and (flags & 0x04000000):
        relocSize, = struct.unpack(">I", d[dirSize + 4:dirSize + 8])
    reloc = d[dirSize:dirSize + relocSize]
    protoEntry = d[52:84]
    protoOff, protoSize = struct.unpack(">II", protoEntry[0:8])
    oldProtoStart = dirSize + relocSize + protoOff
    proto = d[oldProtoStart:oldProtoStart + protoSize]

    # The protocol part starts on a word boundary after the NewtonScript part.
    protoOff = (len(part) + 3) & ~3
    pad = b"\0" * (protoOff - len(part))
    if reloc:
        # The relocated words hold offsets from the start of the package too.
        delta = nsStart + protoOff - oldProtoStart
        proto = bytearray(proto)
        for o in relocatedWords(reloc):
            v, = struct.unpack(">I", proto[o - oldProtoStart:o - oldProtoStart + 4])
            struct.pack_into(">I", proto, o - oldProtoStart, v + delta)
        reloc = moveRelocations(reloc, delta)
    relocSize = len(reloc)

    # The variable data: copyright and name, then the protocol part's info,
    # padded so that the parts start at nsStart.
    infoOff, infoLen = struct.unpack(">HH", protoEntry[24:28])
    info = d[52 + 32 + infoOff:52 + 32 + infoOff + infoLen]
    varData = bytearray()
    crRef = (len(varData), len(utf16(copyright)))
    varData += utf16(copyright)
    nameRef = (len(varData), len(utf16(name)))
    varData += utf16(name)
    protoInfoRef = (len(varData), len(info))
    varData += info
    newDirSize = nsStart - relocSize
    if 52 + 2 * 32 + len(varData) > newDirSize:
        sys.exit("The directory does not fit before offset %d" % nsStart)
    varData += b"\0" * (newDirSize - 52 - 2 * 32 - len(varData))
    total = nsStart + protoOff + protoSize

    out = bytearray(d[0:52])
    struct.pack_into(">HH", out, 20, *crRef)
    struct.pack_into(">HH", out, 24, *nameRef)
    struct.pack_into(">I", out, 28, total)
    struct.pack_into(">II", out, 44, newDirSize, 2)
    # offset, size, size2, type, reserved, flags, info, compressor
    typeCode = partType.encode("ascii").ljust(4)[:4]
    out += struct.pack(">III4sIIHHHH", 0, len(part), len(part), typeCode, 0, int(partFlags, 0), 0, 0, 0, 0)
    entry = bytearray(protoEntry)
    struct.pack_into(">II", entry, 0, protoOff, protoSize)
    struct.pack_into(">HH", entry, 24, *protoInfoRef)
    out += entry
    out += varData
    out += reloc
    assert len(out) == nsStart
    out += part + pad + proto
    assert len(out) == total
    open(outPkg, "wb").write(out)


if __name__ == "__main__":
    main()
