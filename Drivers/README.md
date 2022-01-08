# Einstein ROM Extension

This directory contains the ROM extension (REx) for Einstein, which implements the protocols to interface with the
host OS. The REx is by default part of the Einstein application.

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

The C++ headers are part of the [NewtonDev](http://www.unna.org/view.php?/development/NewtonDev) archive (you will need a tool to uncompress NewtonDev.sit. On MacOS X, you can use The Unarchiver from the AppStore). An alternative is to clone this [repository](https://github.com/ekoeppen/NCT_Projects).

The environment variable `NCT_PROJECTS` needs to point to the location of the directory containing the `Includes` and `DDKIncludes` directories.

### Building

After installing GCC and the DCL tools, and setting `NCT_PROJECTS` correctly, the REx can be built with `make`. In order to use the resulting REx, it needs to be placed in the `_Data_` directory of the Einstein source tree prior to compiling Einstein (compiling Einstein will embed the REx into the application). For subsequent FLTK based builds, use `touch app/TFLRexImage.fl` to ensure that a recompiled REx is properly embedded.
