
Building Einstein form Source
=============================


Linux
-----


macOS
-----

Install FLTK first: get it from GitHub, go into the root directory, 
```bash
mkdir build
cd build, 
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


Android
-------


iOS
---


