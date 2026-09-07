# Shijima-Qt

> [!IMPORTANT]
> Shijima-Qt was discontinued and archived due to lack of maintenance and the project's current state. The latest recommended/stable build has been v0.1.0 for over a year now and it is missing important fixes and improvements from libshijima/libshimejifinder. Furthermore, the feature/wayland-layer-shell branch has deviated so much from the main branch that it has become very hard to merge back. I now believe that Qt was not the right framework for this project, and the amount of hacks I had to use to make it work with Qt has turned this project into an unmanageable mess. Therefore, I don't think it's a good idea to continue working on this project further. I may work on a new, more optimized desktop version of Shijima that does not use Qt in the future. In the meantime, please check out the other, more current versions of Shijima: [Shijima-iOS](https://havoc.app/package/shijima), [Shijima-Android](https://play.google.com/store/apps/details?id=com.pixelomer.shijima), [Shijima-Web](https://pixelomer.github.io/Shijima-Web/)

![Shijima-Qt running on Fedora 41](.images/Shijima-Qt-Fedora.jpg)

Cross-platform shimeji desktop pet simulator. Built with Qt6. Supports macOS, Linux and Windows.

- [Download the latest release](https://github.com/pixelomer/Shijima-Qt/releases/latest)
- [See all releases](https://github.com/pixelomer/Shijima-Qt/releases)
- [Report a bug or make a feature request](https://github.com/pixelomer/Shijima-Qt/issues)
- [Shijima homepage](https://getshijima.app)

If you'd like to support the development of Shijima, consider becoming a [sponsor on GitHub](https://github.com/sponsors/pixelomer) or [buy me a coffee](https://buymeacoffee.com/pixelomer).

## Building

CMake is the primary development build. It supports native Windows, Linux and
macOS builds and keeps generated files and dependencies in the build directory.
The existing Makefiles and release workflow remain available for legacy
Docker cross-compilation and AppImage packaging.

Requirements: CMake 3.21+, a C++17 compiler, Python 3.8+, Qt 6.2+ (Core, Gui,
Widgets, Concurrent, and Multimedia), and libarchive development files. Ninja is
required for the provided presets. Qt Multimedia can be omitted when building
with `-DSHIJIMA_USE_QTMULTIMEDIA=OFF`.

Initialize the pinned dependencies first (no GitHub SSH key is needed):

```sh
git submodule update --init --recursive
```

### Linux

On Ubuntu/Debian:

```sh
sudo apt install cmake ninja-build g++ python3 qt6-base-dev qt6-multimedia-dev libarchive-dev libx11-dev libbz2-dev liblzma-dev zlib1g-dev
cmake --preset debug
cmake --build --preset debug --parallel
./build/debug/shijima-qt
```

Use `release` instead of `debug` for an optimized build. If Qt was installed
outside the system directories, add `-DCMAKE_PREFIX_PATH=/path/to/qt/kit` when
configuring. Linux extension resources are bundled with Python; Node, Yarn and
JavaScript minification are no longer required for the CMake build.

### Windows (MSVC)

Install Visual Studio's **Desktop development with C++** tools, an MSVC Qt 6 kit,
CMake, Ninja, Python, and a current [vcpkg](https://github.com/microsoft/vcpkg)
checkout. Use an **x64 Native Tools Command Prompt for VS 2022** so CMake can
find the compiler. Adjust these example paths to match your installations:

```bat
cmake --preset debug -DCMAKE_PREFIX_PATH=C:/Qt/6.8.2/msvc2022_64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build --preset debug --parallel
set PATH=C:\Qt\6.8.2\msvc2022_64\bin;%PATH%
build\debug\shijima-qt.exe
```

The vcpkg manifest installs libarchive and its dependencies; install Qt separately.
Keep the Qt kit, compiler and dependency architecture compatible. For MinGW, use
a MinGW Qt kit and matching libarchive binaries instead of MSVC packages.

### macOS

With MacPorts:

```sh
sudo port install cmake ninja python312 qt6-qtbase qt6-qtmultimedia libarchive
cmake --preset debug -DCMAKE_PREFIX_PATH="/opt/local/libexec/qt6;/opt/local" -DPython3_EXECUTABLE=/opt/local/bin/python3.12
cmake --build --preset debug --parallel
open build/debug/shijima-qt.app
```

Alternatively, use a Qt installer kit and point `CMAKE_PREFIX_PATH` at it and
your libarchive installation. CMake includes the app icon and Info.plist in the
macOS bundle.

### CLion

Open the root `CMakeLists.txt` as a project. Select a toolchain matching your Qt
kit and set `CMAKE_PREFIX_PATH` in the CMake profile. On Windows with vcpkg, also
set `CMAKE_TOOLCHAIN_FILE` and `VCPKG_TARGET_TRIPLET` as above. If CLion remembers
an old Qt path or a Makefile project, reload it as a CMake project and reset the
CMake cache. Use the `shijima-qt` run configuration. Add the Qt `bin` directory
to its `PATH` on Windows.

### Installing and packaging

```sh
cmake --install build/release --prefix /path/to/staging
```

The installation contains the executable, licenses and Linux desktop metadata
(or a macOS app bundle). libarchive and Qt must be available at runtime.
For a native build with Qt 6.5+, configure with `-DSHIJIMA_DEPLOY_QT=ON` before
installing to bundle Qt's runtime libraries and plugins. Native Windows installs
also collect libarchive and its runtime DLLs from the executable directory or
`PATH`; vcpkg copies these beside the build executable automatically.

`SHIJIMA_USE_STUB_PLATFORM=ON` disables native desktop integration for development.
Debug and Release use separate build directories, so switching configurations
does not require `make clean`. The submodule sources stay unmodified; the small
integration layer in `cmake/dependencies` replaces their shell-based generators
and compiler-specific build flags. Keep its source lists aligned when updating
the pinned submodules.

## Platform Notes

### macOS

Shijima-Qt needs the Accessibility permission to access the frontmost window.

### Linux

Shijima-Qt supports KDE Plasma 6 and GNOME 46 in both Wayland and X11. To get the frontmost window, Shijima-Qt automatically installs and enables a shell plugin when started.  
- On KDE, this is transparent to the user.
- On GNOME, the shell needs to be restarted on the first run. This can be done by logging out and logging back in. Shijima-Qt will exit with an appropriate error message if this is required.
- On other desktop environments, window tracking will not be available.

### Windows

Only tested on Windows 11. May also work on Windows 10. Window tracking is supported and no extra actions should be necessary to run Shijima-Qt.
