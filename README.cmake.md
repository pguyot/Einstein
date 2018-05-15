# Compiling on Linux et al

Einstein currently only runs correctly on 32-bit platforms.  So you need to
build a 32-bit binary.  This is a problem in 2018, because most systems are now
64-bit native.

Once you install the 32-bit libraries from your distribution, you need to
tell `cmake` to search for and link against those 32-bit libraries.  This is
done by specifying them as if it was a cross-compile.

* `cd _Build_/CMake`
* `cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-32-linux.cmake ../..`
