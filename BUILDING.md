
How to build Einstein on various platforms in 2020
==================================================


Building Einstein on macOS in 64 bit with Xcode
-----------------------------------------------

Tested on macOS 10.15.3 Catalina with Xcode 11.4.

Download, build, and install FLTK first. FLTK is a cross-platform user interface library
that is easy to install and use and very light on resources. See https://www.fltk.org .

```bash
# -- Get the source code for FLTK from GitHub
git clone https://github.com/fltk/fltk.git fltk
# -- Go into the FLTK root directory
cd fltk
# -- Make sure that Einstein will run on older versions of MacOS
ex -s -c '1i|set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)' -c x CMakeLists.txt
# -- Create the CMake directory tree
mkdir build
cd build
mkdir Makefiles
cd Makefiles
# -- Create the CMake build files
cmake ../..
# -- Build FLTK
make 
# -- Install the FLTK library, includes, and tools
sudo make install
cd ../../..
# -- make Fluid, the GUI editor, accessible from the command line
ln -s /usr/local/bin/fluid.app/Contents/MacOS/fluid /usr/local/bin/fluid
```

Then download and build Einstein:

```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git -b matt2020 Einstein
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

If you are planning to develop code for Einstein, you may want to use Xcode instead of Makefiles:
```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git -b matt2020 Einstein
cd Einstein/
# -- Create CMake paths
cd _Build_/
mkdir Xcode
cd Xcode/
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


There is a version of the Macintosh Emulator BasiliskII for macOS that can connect directly
to Einstein via serial port emulation:

```bash
git clone https://github.com/MatthiasWM/macemu.git macemu
open macemu/BasiliskII/src/MacOSX/BasiliskII.xcodeproj
```


Building Einstein on Linux in 64 bit
------------------------------------

Tested on Linux Ubuntun 18.04.4 LTS

Install Clang, Make, and CMake. CMake may ask for more resources in the process. Install
them with `sudo apt-get install ...`.

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
cmake ../..
# -- Build FLTK
make 
# -- Install the FLTK library, includes, and tools
sudo make install
cd ../../..
```

Then download and build Einstein:

```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git -b matt2020 Einstein
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
./Einstein
cd ../../..
```

Continue with setting up the ROM as described in the manual. Enjoy.


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


Building Einstein on Windows 10
-------------------------------

Install VisualStudio 2019 with the C++ compiler, CMake, and Git components. 
I like to install TortoiseGIT as well to make accessing GitHub easy.

Download, build, and install FLTK first. FLTK is a cross-platform user interface library
that is easy to install and use and very light on resources. See https://www.fltk.org .

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

Add a Release target if VC did not already create it. The build `fluid.exe`.

Install FLTK by creating `C:\Program FIles\FLTK\` with the subdirectories
`bin`, `lib` and `include`. Copy `fluid.exe` into `bin`, copy all libraries into
`lib`, and copy the `FL` directory into `include`. Fianlly, copy `fltk\out\build\x64-Release\FL\abi-version.h`
into `include\FL`.


Then download and build Einstein:

Clone the `matt2020` branch from `https://github.com/pguyot/Einstein.git` into a folder named `Einstein`.

Again, launch VisualStudio and click "Continue without code...", the select
`File > Open > CMake...` from the VisualStudio
main menu and select `CMakeLists.txt` in the Einstein root directory.

VisualStudio shoudl find all resources including FLTK and Fluid. Set Einstein
as your startup project and compile and run the program.

Continue with setting up the ROM as described in the manual. Enjoy.


There is a version of the Macintosh Emulator BasiliskII for Windows that can connect directly
to Einstein via serial port emulation. You need to install SDL2 to compile BasiliskII:

Clone `https://github.com/MatthiasWM/macemu.git` into `macemu`.
Grab the SDL2 sourcecode and put it in `.../macemu/external`.
Then load and compile the VisualStudio Solution in `macemu/BasiliskII/src/Windows`.


 A L P H A - building with Toolkit
-----------------------------------

To build the 'matt2020_Toolkit' branch, some additional installs are required.

On MacOS X, get MatthiasWM/newt64, build the library, and copy it to '/usr/local/lib/libnewt64.s'
Also, copy all files in '.../src/newt_core/incs/' into '/usr/local/include/newt64/'.
You may have to install libiconv, but it seemed to be installed already on my machine.

On Linux, get MatthiasWM/newt64 and build it by calling
  git clone https://github.com/MatthiasWM/NEWT64.git
  cd NEWT64/
  autoconf
  CPPFLAGS=-m64 ./configure
  make libnewt 
  sudo cp build/libnewt.a /usr/local/lib/libnewt64.a
  sudo mkdir /usr/local/include/newt64
  sudo cp -R src/newt_core/incs/* /usr/local/include/newt64/

Also, get libiconv, build and install it by running these shell commands:
  wget http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz
  tar xvzf libiconv-1.16.tar.gz 
  cd libiconv-1.16/
  ./configure --prefix=/usr/local/ --enable-static
  make
  sudo make install


On MSWindows, you need to install flexx and bison in C:/GnuWin32/ 
and add C:/GnuWin32/bin to the Path variable in the user environment settings.

We don't have iconv yet, so your NS files must not have anything but ASCII characters

git clone https://github.com/MatthiasWM/NEWT64.git

... and build the newt64 library using VisualStudio and CMake



