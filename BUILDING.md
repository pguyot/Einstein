
How to build Einstein on various platforms in 2020
==================================================


Building Einstein on macOS in 64 bit with Xcode
-----------------------------------------------

Tested on macOS 10.15.3 Catalina with Xcode 11.4.

TODO: install FLTK first

```bash
git clone https://github.com/fltk/fltk.git fltk
cd fltk
ex -s -c '1i|set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9)' -c x CMakeLists.txt
mkdir build
cd build
mkdir Makefiles
cd Makefiles
cmake ../..
make 
sudo make install
```

Find the file `Einstein.xcodeproj` in the directory `_Build_/Xcode/Einstein.xcodeproj` 
inside your Einstein archive.

Select `Einstein > My Mac` in the top left picker next to the Stop button as the current 
Scheme (selection box at the top left). 

Typing Apple-R will create and run the debug version for you which is relatively slow.
Instead select `Product > Archive` to create an optimized version. This takes a few minutes,
but the Xcode will show you a dialog box with the archive that you just created.

Right-click on the little icon and select `Show in Finder`. Then in Finder, right-click
on the archive again and choose `Show Package Contents`. You will find the fast version 
of Einstein inside the folder `Products > Application`.

Continue with setting up the ROM as described in the manual. Enjoy.

(BasiliskII: open `.../BasiliskII/src/MacOSX/BasiliskII.xcodeproj` in Xcode and Apple-R to run)


Building Einstein on Linux in 64 bit
------------------------------------

Tested on Linux Ubuntun 18.04.4 LTS

Install Clang, Make, and CMake. CMake may ask for more resources in the process. Install
them with `sudo apt-get install ...`.

Use the terminal and change into the `_Build_` directory. Create a directoy named `Makefiles`.

```bash
cd _Build_
mkdir Makefiles
cd Makefiles
```

Now run CMake. If your missing resources, CMake will tell you. Install tehm and run CMake again 
until you get a list of stuff ending with `-- Build files have been written to:...`. The run `make`.

```bash
cmake -DCMAKE_BUILD_TYPE=Release ../..
# repeat until you have all resources
make 
```

A lot of warnings may appear that you can ignore. In the end, you will have a file named
`einstein` in `_Build_/Makefile`. Just launch it by typing `./einstein path_to_my_rom`.

(BasiliskII: in `.../BasiliskII/src/Unix`, run `./autogen.sh --without-gtk --without-x --disable-standalone-gui --disable-gtk-test --with-sdl-static --with-sdl-video --with-sdl-audio --with-sdl-framework` and `make`)




(please ignore the text below. This will eventually be the Windows installation instructions. Maybe.)

Prerequisites: 7-zip, ability to switch to US keyboard.

Installing Einstein on Windows 10:

Downlaod the binary from http://www.elektriktrick.com/Newton_Installs.html :

Einstein.Windows.2020.3.4.zip
and uncompress into $HOME/Newton
Einstein.rex.zip
and uncompress into $HOME/Newton
Find the ROM: 717006.rom
and save in $HOME/Newton

"Windows protected your PC" -> More Info... -> Run Anyway
"The code execution cannot procede because VCRUNTIME140_1.dll was not found"
-> download and run (CFLAGS: -d2FH4- LDFLAGS: -d2:-FH4-)
http://aka.ms/vs/16/release/vc_redist.x64.exe (requires restart - moan!)

Einstein settings will pop up: In the "ROM" section, choose your 717006.rom file.
Keep "Machine" at MP2x00 US.
I like to change the location of the Flash file to the same directory, but you can leave it as is ((FLTK Preferences?!) c:\users\micro\AppData\Roaming\robowerk.com\einstein\internal.flash)
Screen is 320x480 for the original feeling. I prefer 480x720.
Leave the RAM size alone (only 1MB and 4MB seem to work, just like the original)

If the screen resolution is not working out, Ctrl-'+' and Ctrl-'-' change the screen scaling.

Install apps by simply dragging the package onto the Newton screen.


Installing BasiliskII with NTK on Windows 10:

Download the binary from http://www.elektriktrick.com/Newton_Installs.html :

BasiliskII.Windows.E.4.zip

Download from Unna:
boot disk
dev disk (WinZIp universal will not work, use 7-zip from www.7-zip.org)

Find and download Performa.rom

Unpack into $HOME/Newton.

Open BasiliskII_prefs with Notepad and adjust the netries for rom, disk, again disk, and extfs. Leave
seriala at tcp:3679

If you like, create a fake MacOS Classic disk that the emulated Mac and your PC can see in $HOME/Newton/MacDisk.

Launch BasiliskII. Windows Firewall will complain. Allow basiliskii.exe to communicate within private networks.



Build from scratch:

Prerequisites: VisualC 2019 (Community) with Universal Windows Platform development and C++, and the 
Additional Components "CMake" and "git for Windows"

go inot $HOME/dev
git clone fltk > $HOME/dev/fltk-1.4.git
git clone Einstein branch matt2020



git clone macemu >$HOME/dev/macemu (includes BasiliskII)
grab the SDL2 sourcecode and put it in $HOME/dev/macemu/external

















