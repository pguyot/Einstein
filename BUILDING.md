
# How to build Einstein on various platforms

## Table Of Contents

* Building Einstein with Cocoa on macOS in 64 bit Universal
* Building Einstein with FLTK on macOS in 64 bit with Xcode
* Building Einstein on Linux in 64 bit
* Building Einstein on Windows 10/11
* Buidling Einstein for Android
* Building Einstein for iOS

## Introduction

Einstein uses GitHub Workflows to verify integrity and code quality by building 
test scenarios and Einstein itself whenever code is changed. The `release.yml`
builds and releases the final user version of Einstein for all desktop platforms,
making it the final authority in building the latest release of Einstein.

## Building Einstein with Cocoa on macOS in 64 bit Universal

### Using Xcode

Tested on macOS 12.0.1 Monterey with Xcode 13.2.1

Install Xcode from the Apple AppStore.

Building Einstein with Cocoa is very easy and straight forward. Cocoa is the
user interface that is built into macOS. Just open the
```_Build_/Xcode/Einstein.xcodeproj``` in Xcode and press Cmd-R to
build and run the package. Einstein should launch in Debug mode
after a few minutes and ask you for the location of the ROM.

Einstein with Cocoa comes with network and serial port emulation,
and an assembly language Monitor. If you are interested in
emulating PCMCIA Flash Memory cards, or would like to try
some programming in NewtonScript, Einstein with FLTK is the better choice.

### Using the command line

Install Xcode and Xcode's command line tools.
Start the Terminal:

```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/
# -- Build the Archive version (optimized)
xcodebuild archive \
    -project _Build_/Xcode/Einstein.xcodeproj \
    -scheme Einstein -configuration Release \
    -archivePath Einstein.xcarchive \
    ONLY_ACTIVE_ARCH=NO
# -- Optional: Copy Einstein to the Applications folder
cp -R Einstein.xcarchive/Products/Applications/Einstein.app /Applications
```
Your final version of Einstein will be in ```/Applications/Einstein.app```.

## Building Einstein with FLTK on macOS in 64 bit with Xcode

Tested on macOS 12.0.1 Monterey with Xcode 13.2.1

### Using the command line

Install Xcode and Xcode's command line tools.
Install the current version of CMake from https://cmake.org .
Start the Terminal:

```bash
# -- Get the Einstein source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/
# -- Get the FLTK source code from GitHub
git clone https://github.com/fltk/fltk.git fltk
# -- Get the newt64 source code from GitHub
git clone https://github.com/MatthiasWM/NEWT64.git newt64
# -- Compile FLTK
cmake -S fltk -B fltk/build \
    -D OPTION_USE_SYSTEM_LIBJPEG=Off \
    -D OPTION_USE_SYSTEM_ZLIB=Off \
    -D OPTION_USE_SYSTEM_LIBPNG=Off \
    -D FLTK_BUILD_TEST=Off \
    -D OPTION_USE_GL=Off \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_OSX_DEPLOYMENT_TARGET=10.9 \
    -D "CMAKE_OSX_ARCHITECTURES=arm64;x86_64"
cmake --build fltk/build
# -- Compile newt64 (a few warnings will pop up)
cmake -S newt64 -B newt64/build \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_OSX_DEPLOYMENT_TARGET=10.9 \
    -D "CMAKE_OSX_ARCHITECTURES=arm64;x86_64"
cmake --build newt64/build
# -- Compile Einstein (this may take 20 inutes or more)
cmake -S . -B build \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_OSX_DEPLOYMENT_TARGET=10.9 \
    -D "CMAKE_OSX_ARCHITECTURES=arm64;x86_64"
cmake --build build --target Einstein
# -- Optional: Copy Einstein to the Applications folder
cp -R build/Einstein.app /Applications
```

### BasiliskII

There is a version of the Macintosh Emulator BasiliskII for macOS that can connect directly
to Einstein via serial port emulation:

```bash
git clone https://github.com/MatthiasWM/macemu.git macemu
open macemu/BasiliskII/src/MacOSX/BasiliskII.xcodeproj
```

## Building Einstein on Linux in 64 bit

Tested on Linux Ubuntu 20.04.2 LTS on March 13th 2021

### Prerequisites

Install Clang, Make, and CMake. CMake may ask for more resources in the process. Install
them with `sudo apt-get install ...`.

```bash
sudo apt-get install git cmake
sudo apt-get install clang
sudo apt-get install autoconf
sudo apt-get install libx11-dev
sudo apt-get install libglu1-mesa-dev
sudo apt-get install libasound2-dev
sudo apt-get install libxft-dev
sudo apt-get install bison flex
sudo apt-get install pulseaudio
sudo apt-get install libpulse-dev
```

### FLTK

Download, build, and install FLTK first. FLTK is a cross-platform user interface library
that is easy to install and use and very light on resources. See https://www.fltk.org .

```bash
# -- Get the source code for FLTK from GitHub
git clone https://github.com/fltk/fltk.git fltk
# -- Go into the FLTK root directory
cd fltk
# -- Create the CMake directory tree
mkdir build
cd build
mkdir Makefiles
cd Makefiles
# -- Create the CMake build files
cmake -DCMAKE_BUILD_TYPE=Release \
      -D OPTION_USE_SYSTEM_LIBJPEG=Off \
      -D OPTION_USE_SYSTEM_ZLIB=Off \
      -D OPTION_USE_SYSTEM_LIBPNG=Off \
      -D FLTK_BUILD_TEST=Off \
      ../..
# -- Build FLTK
make
# -- Install the FLTK library, includes, and tools
sudo make install
cd ../../..
```

### Newt64/Toolkit

Newt64 will enable the built-in Developer Toolkit in Einstein
and a few other features that depend on NewtonScript compilation features.

```bash
# -- Get the source code for Newt64
git clone https://github.com/MatthiasWM/NEWT64.git
cd NEWT64/
# create the build environment and Makefile
mkdir Build
cd Build
cmake ..
# -- Build the library and the tools
make
# -- Install the library, so Einstein can find it
sudo make install
cd ../..
```

### Einstein

Then download and build Einstein:

```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/
# -- Create CMake paths
cd _Build_/
mkdir Makefiles
cd Makefiles/
# -- Create the CMake build files
cmake -DCMAKE_BUILD_TYPE=Release ../..
# -- Build Einstein (this will take a while, you need 4GB or RAM)
make
# -- Run Einstein and enjoy
./Einstein
cd ../../..
```

Continue with setting up the ROM as described in the manual. Enjoy.

### BasiliskII (optional)

There is a version of the Macintosh Emulator BasiliskII for Linux that can connect directly
to Einstein via serial port emulation. You need to install SDL2 to compile BasiliskII:

```bash
git clone https://github.com/MatthiasWM/macemu.git macemu
cd macemu/BasiliskII/src/Unix
./autogen.sh --without-gtk --without-x --disable-standalone-gui \
    --disable-gtk-test --with-sdl-static --with-sdl-video \
    --with-sdl-audio --with-sdl-framework
make
```


## Building Einstein on Windows 10/11

### Prerequisites

Install VisualStudio 2020. It will include the C++ compiler. Make sure that
the CMake tools are also installed.

You will also need git. You can install it
from this source: https://gitforwindows.org .

Einstein can built very easily from the command line. Open the Developer Command
Prompt:
```
Start Menu
  -> All Apps
    -> Visual Studio 2020 Folder
      -> Developer Command Promt for VS 2022
```

Enter the following commands into the shell.

Downlaod the Einstein source code:
```
git clone https://github.com/pguyot/Einstein.git
cd Einstein
```
Download and build FLTK inside the Einstein folder. FLTK is a cross-platform
user interface toolkit. FLTK is not optional.
```
git clone https://github.com/fltk/fltk.git
cmake -S fltk -B fltk/build -D OPTION_USE_SYSTEM_LIBJPEG=Off -D OPTION_USE_SYSTEM_ZLIB=Off -D OPTION_USE_SYSTEM_LIBPNG=Off -D CMAKE_BUILD_TYPE=Release -D OPTION_USE_GL=Off -D FLTK_BUILD_TEST=Off
cmake --build fltk/build --config Release
```
Download and build Newt64 inside the Einstein folder. Newt64 is a NewtonScript
interpreter and is the basis of the built-in Toolkit. Newt64 is optional.
```
git clone https://github.com/MatthiasWM/NEWT64.git newt64
cmake -S newt64 -B newt64/build -D CMAKE_BUILD_TYPE=Release
cmake --build newt64/build --config Release
```
With all prerequisites installed, we can finally build Einstein:
```
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
If everything went well, Einstein can be found here:
```
build\Release\Einstein.exe
```
Continue with setting up the ROM as described in the manual. Enjoy.

### BasiliskII
There is a version of the Macintosh Emulator BasiliskII for Windows that can connect directly
to Einstein via serial port emulation. You need to install SDL2 to compile BasiliskII:

Clone `https://github.com/MatthiasWM/macemu.git` into `macemu`.
Grab the SDL2 sourcecode and put it in `.../macemu/external`.
Then load and compile the VisualStudio Solution in `macemu/BasiliskII/src/Windows`.


## <a target=Android/>Building Einstein for Android

The ```_Build_``` folder contains the current setup for Einstein on Android. Open
Android Studio and launch the project in the ```_Build_/AndroidStudioNative```.

The Android version of EInstein is still in its Alpha stage and will need
some more development before it is actually usable.

Note that changes in the source tree are sometimes not propageted
to the Android build for a while.


## Building Einstein for iOS

Tested on Dec 27 2021, compiled 12.0.1 Monterey and M1 CPU,
running on iPhone 13 Max Pro with iOS 15.2

 - install the Xcode developer environment from teh Apple AppStore, it's free
 - make sure you also install the command line tools, so you have `git` available
 - in the shell, do ```git clone https://github.com/pguyot/Einstein.git Einstein4iOS```
 - in the shell, do ```open Einstein4iOS/_Build_/Xcode/Einstein.xcodeproj```
 - now in Xcode, select the ```iOSEinstein``` scheme and your phone as your target
 - you may have to change details for the Target Settings in the *Signing* tab
 - build and run Einstein for your phone - it will fail, but now Finder will
   show the directory that you need to install the ```717006.rom``` file.





Build log Window 11 and VisualStudin 2022

Open a Developer COmmand Promt (Start Menu -> All Apps -> Visual Studi 2020 Folder -> Developer Command Promt for VS 2022)

git clone https://github.com/pguyot/Einstein.git
cd Einstein
git clone https://github.com/fltk/fltk.git
cmake -S fltk -B fltk/build -D OPTION_USE_SYSTEM_LIBJPEG=Off -D OPTION_USE_SYSTEM_ZLIB=Off -D OPTION_USE_SYSTEM_LIBPNG=Off -D CMAKE_BUILD_TYPE=Release -D OPTION_USE_GL=Off -D FLTK_BUILD_TEST=Off
cmake --build fltk/build --config Release
git clone https://github.com/MatthiasWM/NEWT64.git newt64
cmake -S newt64 -B newt64/build -D CMAKE_BUILD_TYPE=Release
# ignore warning
cmake --build newt64/build --config Release
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release
build\Release\Einstein.exe



