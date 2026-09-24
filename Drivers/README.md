# Einstein ROM Extension

This directory contains the ROM extension (REx) for Einstein, which implements
the protocols to interface with the host OS. The REx is by default part of the
Einstein application. The extension contains native parts which implement
NewtonOS protocols, as well as mixed NewtonScript/native extensions in the
form of NewtonOS autoparts.

## Prerequisites

The REx is a collection of NewtonOS binaries (wrapped into NewtonOS packages),
and thus the sources need to be cross-compiled with a NewtonOS-compatible tool
chain. Compilation also requires NewtonOS-specific tools.

CMake will fetch missing dependencies automatically except for GCC. If GCC is
not found, the build will be skipped.

### GCC

GCC with support for `armv4` as an ARM cross compiler, e.g. [gcc-arm-none-eabi](https://packages.ubuntu.com/search?keywords=gcc-arm-none-eabi&searchon=names)
for an Ubuntu package, [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
for the general ARM-provided downloads, or a
[brew tap](https://github.com/armmbed/homebrew-formulae) for installation
via Homebrew.

### DCL

Conversion of the compiled binaries requires the `Rex` and `ELFtoPKG` tools
provided by [DCL](https://github.com/pguyot/DCL).

### NCT and DDK Headers

The C++ headers and Newton Platform file are obtained by cloning this
[repository](https://github.com/ekoeppen/NCT_Projects).

### tntk

[tntk](https://github.com/ekoeppen/tntk) is an optional prerequisite to
compile NewtonScript packages. If it is not installed, the precompiled
packages are included in the REx.

## Building

The CMake script will pull in the dependencies and build first the DCL, then
tntk using the CMake file in `./host`, followed by compiling the REx using the
CMake file in `./target`. This is driven by the top level CMake file, e.g.
executing in the directory here:

    cmake -S . -B build
    cmake --build build

## Components

- Protocols: The NewtonOS to host implementation is provided as NewtonOS
  protocol implementations, which will either handle the calls directly
  natively, or will invoke the Einstein drivers via coprocessor instructions
- NSRuntime: Helper package
- EinsteinPortEnabler: Autopart which will initialize serial drivers when
  enabled via the Newton preferences app
- Additional packages in the `packages` directory can be included in the REx
