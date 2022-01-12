
# How to build Einstein on various platforms

## Table Of Contents
* Building Einstein with Cocoa on macOS in 64 bit with Xcode
* Building Einstein with FLTK on macOS in 64 bit with Xcode
* Building Einstein on Linux in 64 bit
* Building Einstein on Windows 10
* Buidling Einstein for Android
* Building Einstein for iOS


## Building Einstein with Cocoa on macOS in 64 bit with Xcode

Tested on macOS 10.15.3 Catalina with Xcode 11.4.

Install Xcode from the Apple AppStore.

Building Einstein with Cocoa is very easy and straight forward. Cocoa is the 
user interface that is built into MacOS. Just open the 
```_Build_/Xcode/Einstein.xcodeproj``` in Xcode and press Cmd-R to
build and run the package. Einstein should launch in Debug mode
after a few minutes and ask you for the location of the ROM.

Einstein with Cocoa comes with network and serial port emulation,
and an assembly language Monitor. If you are interested in
emulating PCMCIA Flash Memory cards, or would like to try 
some programming in NewtonScript, Einstein with FLTK is the better choice.


## Building Einstein with FLTK on macOS in 64 bit with Xcode

Tested on macOS 10.15.3 Catalina with Xcode 11.4.

### Prerequisites

Install Xcode and Xcode's command line tools.
Install the current version of CMake from https://cmake.org .

### FLTK

Download, build, and install FLTK first. FLTK is a cross-platform user interface library
which is easy to install and use and very light on resources. See https://www.fltk.org .

```bash
# -- Get the source code for FLTK from GitHub
git clone https://github.com/fltk/fltk.git fltk
# -- Go into the FLTK root directory
cd fltk
# -- Make sure that Einstein will run on older versions of MacOS
# use the line below if you build FLTK for macOS 10 (Mavericks all the way up to Catalina)
#ex -s -c '1i|set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)' -c x CMakeLists.txt
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

Clone the Git repository in https://github.com/MatthiasWM/NEWT64.git, 
build the library with *CMake*, and copy it to ```/usr/local/lib/libnewt64.s```
Also, copy all files in ```.../src/newt_core/incs/``` into ```/usr/local/include/newt64/```.

On Windows machines, you will have to install the lex and yacc tools, i.e. GnuWin32's flexx and bison .

```
# -- Get the source code from GitHub
git clone https://github.com/MatthiasWM/NEWT64.git Newt64
cd Newt64/
# -- Build release version
mkdir _Build_; cd _Build_
mkdir Release; cd Release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build .
# -- Install library and headers
sudo cmake --install .
```

You may have to install libiconv, but it seemed to be installed already on my machine.

### Einstein (Makefiles)

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
# -- Build Einstein
make
# -- Run Einstein and enjoy
open ./Einstein.app
cd ../../..
```

### Einstein (Xcode)

If you are planning to develop code for Einstein, you may want to use Xcode instead of Makefiles:
```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/
# -- Create CMake paths
cd _Build_/
mkdir Xcode.FLTK
cd Xcode.FLTK/
# -- Create the CMake build files
cmake -G Xcode ../..
# -- Now open the Xcode IDE
open Einstein.xcodeproj
cd ../../..
```

In Xcode, select `Product > Run` from the main menu or type `Apple-R` to compile and run Einstein
in debugging mode.

To generate the faster release version, select `Product > Archive` to create an optimized version. 
This takes a few minutes, but eventually Xcode will show you a dialog box with the archive that you just created.

Right-click on the little icon and select `Show in Finder`. Then in Finder, right-click
on the archive again and choose `Show Package Contents`. You will find the fast version 
of Einstein inside the folder `Products > Application`.

Continue with setting up the ROM as described in the manual. Enjoy.

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


## Building Einstein on Windows 10

### Prerequisites

Install VisualStudio 2019 with the C++ compiler, CMake, and Git components. 
I like to install TortoiseGIT as well to make accessing GitHub easy.

### FLTK 

Download, build, and install FLTK first. FLTK is a cross-platform user interface library
that is easy to use and very light on resources. See https://www.fltk.org .

Clone `https://github.com/fltk/fltk.git` into a directory named `fltk`.

Add the following lines to the beginning of `fltk/CMakeListst.txt`:
```
cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

Launch VisualStudio 2019. Click on "Continue without code...". An 
empty project will open. Select `File > Open > CMake...` from the VisualStudio
main menu and select `CMakeLists.txt` in the fltk root directory.

Add a Release target if VC did not already create it. Then build and run `fluid.exe`
in x64-Release mode to verify that FLTK built fine.

Next, chose `Project > CMake settings for FLTK` from the main menu. Scroll down
in the settings window and find the entry for `CMAKE_INSTALL_PREFIX`. Copy
the path. We will need it later. In my case, it was `C:\Users\micro\dev\fltk-1.4 for Einstein.git\out\install\x64-Release`,
but it will be diffferent on your machine.

Install FLTK by choosing `Build > Install FLTK`.

For Einstein to find FLTK, we need to create a link in the file system. Open
a Command Prompt in Administrator mode and type the following commands, but replace
the last path with whatever your FLTK path is.

```
c:
cd "c:\Program Files"
mklink /D FLTK "C:\Users\micro\dev\fltk-1.4 for Einstein.git\out\install\x64-Release"
```

### Newt64/Toolkit

Newt64 will enable the built-in Developer Toolkit in Einstein 
and a few other features that depend on NewtonScript compilation features.

You need to install _flex_ and _bison_ in `C:/GnuWin32/` and add `C:/GnuWin32/bin` to the Path variable in the user environment settings.

As of December 2020, both tools can be downloaded from `https://github.com/lexxmark/winflexbison/releases/tag/v2.5.23`, unpacked, and then
copy the content of the archive into `C:/GnuWin32/bin`. Remove the *win_* prefix from *yacc.exe* and *bison.exe*. 
Next, launch `Advanced System Settings` from the _Window 10_ Search Bar, click on
`Environmet Variables...`, the edit the _PATH_ variable and add `C:/GnuWin32/bin`. Close all dialogs.

Open Visual Studio and clone the GitHub project `https://github.com/MatthiasWM/NEWT64.git`.
Add the `x64-Release` configuration. Newt64 should find all resources now. Build and install.


### Einstein

Next, download and build Einstein:

Clone the Einstein source code from `https://github.com/pguyot/Einstein.git` into a folder named `Einstein`.

Again, launch VisualStudio and click "Continue without code...", the select
`File > Open > CMake...` from the VisualStudio
main menu and select `CMakeLists.txt` in the Einstein root directory.

VisualStudio should find all resources including FLTK and Fluid. Set Einstein
as your startup project and compile and run the program.

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



