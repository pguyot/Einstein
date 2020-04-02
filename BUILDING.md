
How to build Einstein on various platforms in 2020
==================================================


Building Einstein on macOS in 64 bit with Xcode
-----------------------------------------------

Tested on macOS 10.15.3 Catalina with Xcode 11.4.

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
CMake -DCMAKE_BUILD_TYPE=Release ../..
# repeat until you have all resources
make 
```

A lot of warnings may appear that you can ignore. In the end, you will have a file named
`einstein` in `_Build_/Makefile`. Just launch it by typing `./einstein path_to_my_rom`.
