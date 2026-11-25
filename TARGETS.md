# Targets and Feature Support

Historically, Einstein leveraged the
[K library](https://sourceforge.net/projects/klibs/) as a cross platform C++
abstraction layer. The K library normalized target platforms and provided a one
stop mechanism for target specific compilation. It is modeled after Apple's
`TargetConditionals.h` header.

With the need to support more platforms and features which are available across
select platforms (such as pseudoterminals, UI layers or audio frameworks), the
mechanism was changed to simplify Einstein's code base. In the order of
recommended usage, these are:

- Standard C++ 20 is the primary abstraction layer (note that due to macOS 10.13
  as the deployment target, this only covers header-only parts of C++ 20).
- The second mechanism is feature detection via CMake to enable various
  `HAVE_...` macros such as `HAVE_PTY` or `HAVE_PULSEAUDIO`.
- The third mechanism is based on the platform specific macro definitions such
  as `_WIN32` or `__linux__`. These should only be used if the code is truly
  platform specific.

Features which are configurable options are passed in a CMake option flags,
e.g, `EINSTEIN_FLTK_FRONTEND` or `EINSTEIN_BUILD_DRIVERS`.

The Xcode project for macOS and iOS uses a fallback header file to define the
`HAVE_...` macros compatible with macOS and iOS.

New code should follow the above conventions to allow more flexibility in
feature support across all targets.
