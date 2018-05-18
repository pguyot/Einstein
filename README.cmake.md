# Building Einstein on Linux Systems

Einstein currently only runs correctly on 32-bit platforms.  So you need to
build a 32-bit binary.  This is a problem in 2018, because most systems are now
64-bit native.  Until all of the code is updated, you will need to install some
32-bit native development and shared libraries on your system.  They will be
part of the "i386" or "i686" architecture, depending on your distribution.

* libc and libstdc++
* libX11 and libXau
* libffi
* Sound libraries, depending on your target:
    * ALSA
    * Pulseaudio
    * Portaudio

Once you have installed 32-bit libraries from your distribution, `cmake` needs
to be able to tell the compiler to build 32-bit objects, as well as search for
and link against those 32-bit libraries.  This is done by telling `cmake` to
build Einstein as if we were cross-compiling for another platform - which we
are, in a way.

# Stop typing and tell me what to do!

* Check out the Einstein repository to a location, we'll call it `[repo-dir]`
* `cd [repo-dir]/_Build_/CMake`
* `./build.sh && make install`

If all goes well, an `einstein` program will end up in
`[repo-dir]/bin/[Platform]-Release/`

If you want a debug build - because you like running emulators slower, or
you like cryptic debugging logs, or you are an Einstein developer - pass the
`debug` argument to `build.sh`.  This will end up in `[repo-dir]/bin/[Platform]-Debug/`
