# How to build Einstein on various platforms

## Table of Contents

- [Introduction](#introduction)
- [CMake Build Options and Targets](#cmake-build-options-and-targets)
- [Building Einstein with Cocoa on macOS (64-bit Universal)](#building-einstein-with-cocoa-on-macos-64-bit-universal)
- [Building Einstein with FLTK on macOS](#building-einstein-with-fltk-on-macos)
- [Building Einstein on Linux](#building-einstein-on-linux)
- [Building Einstein on OpenBSD (Experimental)](#building-einstein-on-openbsd-experimental)
- [Building Einstein on Windows 10/11 (x64 and ARM64)](#building-einstein-on-windows-1011-x64-and-arm64)
- [Building Einstein for Android](#building-einstein-for-android)
- [Building Einstein for iOS](#building-einstein-for-ios)
- [Building Other Frontends and Optional Targets](#building-other-frontends-and-optional-targets)

## Introduction

Einstein uses GitHub Actions workflows in `.github/workflows/` to verify code
quality and build binaries across supported platforms. In particular,
[`.github/workflows/release.yml`](./.github/workflows/release.yml) builds and
publishes the official desktop releases and serves as the authoritative
reference for release build flags. See [README.md](./README.md) and
[TARGETS.md](./TARGETS.md) for platform support tiers and architecture notes.

For CMake-based builds (all desktop platforms and Android), Einstein requires:

- **CMake >= 3.21.0**
- A **C++20**-capable compiler
- **Git** and internet access on the first configure step: `CMakeLists.txt` uses
  CMake's `FetchContent` to automatically download and build **FLTK**
  (`release-1.4.5`), **NEWT64** (`master`), **cDCL** (`3.2.5`), and **SDL3**
  (`release-3.2.16`, when the SDL3 frontend is enabled).

## CMake Build Options and Targets

When configuring with CMake, the following options are available:

| Option                       | Default                              | Description                                                                                                             |
| :--------------------------- | :----------------------------------- | :---------------------------------------------------------------------------------------------------------------------- |
| `EINSTEIN_FLTK_FRONTEND`     | `ON` (default if no frontend is set) | Build the FLTK desktop UI.                                                                                              |
| `EINSTEIN_SDL3_FRONTEND`     | `OFF` (`ON` on Android)              | Build the SDL3 frontend.                                                                                                |
| `EINSTEIN_HEADLESS_FRONTEND` | `OFF`                                | Build the headless CLI frontend (macOS/Unix only).                                                                      |
| `EINSTEIN_BUILD_FLUID`       | `ON`                                 | Build FLTK's `fluid` UI designer (set to `OFF` for faster user/CI builds; automatically disabled when cross-compiling). |
| `EINSTEIN_BUILD_TESTS`       | `OFF`                                | Fetch GoogleTest and enable the `EinsteinTests` unit test target.                                                       |
| `EINSTEIN_BUILD_DRIVERS`     | `OFF`                                | Build the NewtonOS ROM Extension (`Einstein.rex`) drivers (requires `arm-none-eabi-gcc` or `arm-eabi-gcc`).             |

## Building Einstein with Cocoa on macOS (64-bit Universal)

### Using Xcode

Install Xcode from the Apple App Store.

Building Einstein with Cocoa is straightforward. Cocoa is the native macOS user
interface. Open `_Build_/Xcode/Einstein.xcodeproj` in Xcode and press `Cmd-R` to
build and run. Einstein will launch in Debug mode and ask for the location of a
Newton ROM file.

Einstein with Cocoa includes network and serial port emulation as well as an
assembly language Monitor. If you want PCMCIA Flash Memory card emulation or
NewtonScript toolkit features, use the FLTK build instead.

### Using the command line

Install Xcode and Xcode's Command Line Tools, then run in Terminal:

```bash
# -- Get the source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/

# -- Build the Release archive (Universal Binary: arm64 + x86_64)
xcodebuild archive \
    -project _Build_/Xcode/Einstein.xcodeproj \
    -scheme Einstein -configuration Release \
    -archivePath Einstein.xcarchive \
    ONLY_ACTIVE_ARCH=NO \
    CODE_SIGNING_ALLOWED=NO
```

*(Omit `CODE_SIGNING_ALLOWED=NO` if you have configured your own signing team;
see below.)*

Your application bundle will be at
`Einstein.xcarchive/Products/Applications/Einstein.app`, which you can copy to
`/Applications`:

```bash
cp -R Einstein.xcarchive/Products/Applications/Einstein.app /Applications
```

To run the Xcode test suite from the command line:

```bash
xcodebuild test \
    -project _Build_/Xcode/Einstein.xcodeproj \
    -scheme Einstein \
    CODE_SIGNING_ALLOWED=NO
```

### Signing for macOS

The signing configuration is defined in
[`_Build_/Xcode/Signing.xcconfig`](./_Build_/Xcode/Signing.xcconfig):

- `DEVELOPMENT_TEAM`: Team identifier used for signing (empty by default).
- `CODE_SIGN_STYLE`: `Manual` or `Automatic` (defaults to `Manual`).
- `CODE_SIGN_IDENTITY`: Developer certificate common name, e.g.
  `"Apple Development"` or `"iPhone Developer"`.
- `CODE_SIGN_ENTITLEMENTS`: Entitlements file to use (empty by default).

To override the defaults locally without modifying tracked files, create
`_Build_/Xcode/LocalSigning.xcconfig` using
[`_Build_/Xcode/LocalSigningSample.xcconfig`](./_Build_/Xcode/LocalSigningSample.xcconfig)
as a template.

## Building Einstein with FLTK on macOS

### Using the command line

Install Xcode (with Command Line Tools) and CMake (`>= 3.21`). Then run:

```bash
# -- Get the Einstein source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/

# -- Configure and build Einstein (FLTK, NEWT64, and cDCL are fetched automatically)
cmake -S . -B build \
    -D EINSTEIN_BUILD_FLUID=OFF \
    -D CMAKE_BUILD_TYPE=Release
cmake --build build --target Einstein
```

Note that the default macOS deployment target of 10.13 might not be supported on
your local Xcode install anymore, in that case, it can be overridden using
`-D CMAKE_OSX_DEPLOYMENT_TARGET=12.0` or any higher supported version.

Your application bundle will be in `build/Einstein.app`:

```bash
# -- Optional: Copy Einstein to the Applications folder
cp -R build/Einstein.app /Applications
```

## Building Einstein on Linux

These instructions build a native executable for your host architecture (x86_64
or ARM64).

### Using the command line (FLTK frontend)

On Ubuntu/Debian, install the build tools and FLTK/audio dependencies (see
[`.github/workflows/release.yml`](./.github/workflows/release.yml) and
[`.github/workflows/ubuntu.yml`](./.github/workflows/ubuntu.yml)):

```bash
sudo apt-get update
sudo apt-get install -y git cmake ninja-build build-essential libpulse-dev libbsd-dev

# FLTK dependencies:
sudo apt-get install -y libx11-dev libglu1-mesa-dev libxft-dev libxcursor-dev \
    libasound2-dev freeglut3-dev libcairo2-dev libfontconfig1-dev libglew-dev \
    libjpeg-dev libpng-dev libxinerama-dev

# FLTK Wayland dependencies:
sudo apt-get install -y libpango1.0-dev libwayland-dev wayland-protocols \
    libxkbcommon-dev libdbus-1-dev libdecor-0-dev libgtk-3-dev
```

*(Package names vary on other distributions.)*

Then clone and build Einstein:

```bash
# -- Get the Einstein source code from GitHub
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/

# -- Configure and build Einstein (FLTK, NEWT64, and cDCL are fetched automatically)
cmake -S . -B build -D EINSTEIN_BUILD_FLUID=OFF -D CMAKE_BUILD_TYPE=Release
cmake --build build --target Einstein
```

Your binary will be at `build/Einstein`, which you can optionally install:

```bash
sudo cp build/Einstein /usr/local/bin
```

Continue with setting up the Newton ROM as described in the manual.

### SDL3 frontend on Linux

To build Einstein with the SDL3 frontend instead of FLTK (see
[`.github/workflows/ubuntu_sdl.yml`](./.github/workflows/ubuntu_sdl.yml)),
install the SDL3 system dependencies and select `EINSTEIN_SDL3_FRONTEND`:

```bash
sudo apt-get update
sudo apt-get install -y git cmake pkg-config ninja-build build-essential libbsd-dev \
    libasound2-dev libpulse-dev libpipewire-0.3-dev \
    libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev \
    libwayland-dev wayland-protocols libxkbcommon-dev libdbus-1-dev libibus-1.0-dev \
    libdecor-0-dev libegl1-mesa-dev libgl1-mesa-dev libdrm-dev libgbm-dev libudev-dev

cmake -S . -B build-sdl -G Ninja \
    -D CMAKE_BUILD_TYPE=Release \
    -D EINSTEIN_FLTK_FRONTEND=OFF \
    -D EINSTEIN_SDL3_FRONTEND=ON
cmake --build build-sdl --target Einstein
```

## Building Einstein on OpenBSD (Experimental)

OpenBSD (along with FreeBSD/NetBSD) is currently an experimental target
and is not built in CI.

Install the required packages:

```bash
doas pkg_add git cmake pulseaudio
```

Then clone and build Einstein:

```bash
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein/

LOCALBASE=/usr/local cmake -S . -B build \
    -D EINSTEIN_BUILD_FLUID=OFF \
    -D CMAKE_BUILD_TYPE=Release
cmake --build build --target Einstein
```

Your binary will be at `build/Einstein`, which you can optionally copy to
`/usr/local/bin`:

```bash
doas cp build/Einstein /usr/local/bin
```

## Building Einstein on Windows 10/11 (x64 and ARM64)

### Using Visual Studio (MSVC)

Install Visual Studio 2022 (or newer) with the **Desktop development with C++**
and **CMake tools** components, plus
[Git for Windows](https://gitforwindows.org).

Open a **Developer Command Prompt for VS** (or PowerShell with VS environment)
and run:

```bat
git clone https://github.com/pguyot/Einstein.git Einstein
cd Einstein

:: For x64:
cmake -S . -B build -D EINSTEIN_BUILD_FLUID=OFF -A x64 -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target Einstein

:: Or for Windows ARM64:
cmake -S . -B build-arm64 -D EINSTEIN_BUILD_FLUID=OFF -A ARM64 -D CMAKE_BUILD_TYPE=Release
cmake --build build-arm64 --config Release --target Einstein
```

MSVC builds link the C/C++ runtime statically (`/MT`), so `Einstein.exe` does
not require installing a separate Visual C++ Redistributable.

Your executable will be in `build\Release\Einstein.exe` (or
`build-arm64\Release\Einstein.exe`), which you can optionally copy to
`C:\Program Files\Einstein`:

```bat
md "C:\Program Files\Einstein"
copy build\Release\Einstein.exe "C:\Program Files\Einstein"
```

### Using MinGW (Native on Windows or Cross-Compiled from Linux)

Einstein also supports MinGW builds using
[`cmake/toolchain-mingw.cmake`](./cmake/toolchain-mingw.cmake) (see
[`.github/workflows/mingw.yml`](./.github/workflows/mingw.yml)):

- **On Windows (MinGW):**

  ```bash
  cmake -G "MinGW Makefiles" -S . -B build-mingw \
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw.cmake
  cmake --build build-mingw --config Release --target Einstein
  ```

- **Cross-compiling from Ubuntu/Debian Linux:**

  ```bash
  sudo apt-get update
  sudo apt-get install -y mingw-w64 ninja-build
  sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
  sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

  cmake -G Ninja -S . -B build-mingw \
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw.cmake
  cmake --build build-mingw --target Einstein
  ```

*(Add `-D EINSTEIN_BUILD_DRIVERS=ON` if `gcc-arm-none-eabi` is also installed.)*

## Building Einstein for Android

The `android-project/` directory contains the SDL3-based Gradle setup for
Einstein on Android (see [README.Android.md](./README.Android.md) and
[`.github/workflows/android.yml`](./.github/workflows/android.yml)).

1. Download the prebuilt SDL `3.2.16` Android `.aar` into
   `android-project/app/libs`:

   ```bash
   gh -R libsdl-org/SDL release download release-3.2.16 -p 'SDL3-devel-*-android.zip'
   unzip "SDL3-devel-3.2.16-android.zip" -d android-project/app/libs SDL3-3.2.16.aar
   ```

2. Open `android-project/` in Android Studio, or build a debug APK from the
   command line (requires JDK 17 and the Android SDK/NDK):

   ```bash
   cd android-project
   ./gradlew assembleDebug -PeinsteinAbis=arm64-v8a
   ```

The Android version of Einstein is still in an early stage and may lag behind
desktop source changes.

## Building Einstein for iOS

*(Experimental; last verified on macOS 12.0.1 Monterey / iOS 15.2.)*

- Install Xcode from the Apple App Store and install the Command Line Tools.

- Clone the repository:

  ```bash
  git clone https://github.com/pguyot/Einstein.git Einstein4iOS
  open Einstein4iOS/_Build_/Xcode/Einstein.xcodeproj
  ```

- In Xcode, select the `iOSEinstein` scheme and your device as the target.

- Adjust the signing settings in the target's **Signing & Capabilities** tab as
  needed.

- Build and run Einstein for your device — the first launch will fail until you
  place `717006.rom` into the app directory shown in Finder.

## Building Other Frontends and Optional Targets

### Headless CLI Frontend

On macOS and Unix systems, you can build Einstein without a GUI using the
headless frontend:

```bash
cmake -S . -B build-headless \
    -D CMAKE_BUILD_TYPE=Release \
    -D EINSTEIN_HEADLESS_FRONTEND=ON
cmake --build build-headless --target Einstein
```

### Experimental macOS SDL3 Cross-Compile (from Linux)

[`.github/workflows/macos_sdl.yml`](./.github/workflows/macos_sdl.yml) builds
the SDL3 macOS frontend on Linux using Clang/LLD and
[`cmake/toolchain-macos-cross.cmake`](./cmake/toolchain-macos-cross.cmake) with
a macOS SDK (`CMAKE_OSX_SYSROOT`).

### Unit Tests (`EinsteinTests`)

To build and run the CMake unit tests (see
[`.github/workflows/tests.yml`](./.github/workflows/tests.yml)):

```bash
cmake -S . -B build -D EINSTEIN_BUILD_TESTS=ON -D EINSTEIN_BUILD_FLUID=OFF -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target EinsteinTests
ctest --test-dir build --build-config Release --output-on-failure
```

### NewtonOS Drivers / REx (`Einstein.rex`)

Building the NewtonOS ROM Extension requires an ARM cross-compiler
(`arm-none-eabi-gcc` or `arm-eabi-gcc`, e.g.
`sudo apt-get install gcc-arm-none-eabi` on Ubuntu). You can build it either
standalone (see [`.github/workflows/rex.yml`](./.github/workflows/rex.yml)):

```bash
cmake -S Drivers -B Drivers/build
cmake --build Drivers/build
```

or as part of the main Einstein build by passing `-D EINSTEIN_BUILD_DRIVERS=ON`.
The output is written to `_Data_/Einstein.rex`.

### Developer Helper Targets

- `clang-format-check`: verifies formatting in CI (requires `clang-format`).
- `fluid_update`: regenerates `.cpp`/`.h` files from FLTK `.fl` files when
  `EINSTEIN_BUILD_FLUID=ON`.
