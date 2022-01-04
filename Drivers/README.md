# Einstein ROM Extension

This directory contains the ROM extension (REx) for Einstein, which implements the protocols to interface with the
host OS. The REx is by default part of the Einstein application. The extension contains native parts which implement
NewtonOS protocols, as well as mixed NewtonScript/native extenions in the form of NewtonOS autoparts.

## Prerequisites

The REx is a collection of NewtonOS binaries (wrapped into NewtonOS packages), and thus the sources need to be cross
compiled with a NewtonOS compatible tool chain. Compilation also requires NewtonOS specific tools.

### GCC

GCC up to version 12.x as an ARM cross compiler, e.g. [gcc-arm-none-eabi](https://packages.ubuntu.com/search?keywords=gcc-arm-none-eabi&searchon=names)
for an Ubuntu package, [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
for the general ARM provided downloads, or a [brew tap](https://github.com/armmbed/homebrew-formulae) for installation
via Homebrew.

### DCL

Conversion of the compiles binaries requires the `Rex` and `ELFtoPKG` tools. It can be installed by compiling and installing the [DCL](https://github.com/pguyot/DCL).

### NCT and DDK Headers

The C++ headers can be obtained by cloning this [repository](https://github.com/ekoeppen/NCT_Projects).

The environment variable `NCT_PROJECTS` needs to point to the location of the directory containing the `Includes` and `DDKIncludes` directories.

### tntk

[tntk](https://github.com/ekoeppen/tntk) is an optional prerequisite to compile NewtonScript packages. If it not installed, the precompiled
packages are included in the REx.

## Building

After installing GCC and the DCL tools, and setting `NCT_PROJECTS` correctly, the REx can be built with `cmake`:

    cmake -S Drivers -B Drivers/build -DCMAKE_TOOLCHAIN_FILE=cmake/newton-cross.cmake
    cmake --build Drivers/build
    cmake --install Drivers/build

## Components

- Protocols: The NewtonOS to host implementation is provided as NewtonOS protocol implementations, which will either
  handle the calls directly natively, or will invoke the Einstein drivers via coprocessor instructions
- NSRuntime: Helper package
- EinsteinPortEnabler: Autopart which will initialize serial drivers when enabled via the Newton preferences app
- Additional packages in the `packages` directory can be included in the REx
