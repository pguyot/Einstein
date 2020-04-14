
Building Einstein form Source
=============================


Linux
-----


macOS
-----

Install FLTK first: get it from GitHub, go into the root directory, 
```bash
mkdir build
cd build
mkdir Makefiles
cd Makefiles
cmake ../..
make
sudo make install
```
then
```bash
cd _Build_
mkdir Makefiles
cd Makefiles
cmake ../..
make
```
or
```bash
cd _Build_
mkdir Xcode
cd Xcode
cmake -G Xcode ../..
open Einstein.xcodeproj
```


MSWindows
---------

Install FLTK first: get it from GitHub, then launch VisualC 2017. Select the menu `File > Open > CMake...`
and select `CMakeLists.txt` in the FLTK directory. Select the menu item `Cmake > Build All`.
If all goes well, select `CMake > Install > FLTK`.





Android
-------


iOS
---


