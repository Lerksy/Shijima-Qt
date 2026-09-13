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

CMake 3.24+, Conan 2, a C++17 compiler, Python 3.8+, Ninja, and a Qt 6 kit are
required. Qt is installed separately; Conan supplies libarchive, cpp-httplib,
zlib, bzip2, liblzma (xz_utils), and their dependencies. Qt needs Core, Gui,
Widgets, Concurrent, and optionally Multimedia
(`-DSHIJIMA_USE_QTMULTIMEDIA=OFF` disables sound).

Install Conan into your Python environment:

```sh
python -m pip install "conan>=2.23,<3"
```

libshijima, libshimejifinder, and unarr are included in the source tree. Their
small CMake targets are built directly with the application.

The presets inject Conan through
`CMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake`. The provider detects
the selected compiler and configuration, installs missing dependencies, and
generates CMake package targets inside the build directory. No separate
`conan install` or Conan toolchain argument is needed.

### Windows (MSVC)

Install Visual Studio's **Desktop development with C++** tools and a matching
MSVC Qt kit. In an **x64 Native Tools Command Prompt for VS 2022**:

```bat
cmake --preset debug -DCMAKE_PREFIX_PATH=C:/Qt/6.8.2/msvc2022_64
cmake --build --preset debug --parallel
set PATH=C:\Qt\6.8.2\msvc2022_64\bin;%PATH%
build\debug\shijima-qt.exe
```

Adjust the Qt path for your installation. The compiler, Qt kit, and architecture
must match. Use `release` instead of `debug` for an optimized build.

### Linux

On Ubuntu/Debian, install Qt and the native desktop integration dependencies:

```sh
sudo apt install cmake ninja-build g++ python3 python3-venv qt6-base-dev qt6-multimedia-dev libx11-dev
python3 -m venv .venv
. .venv/bin/activate
python -m pip install "conan>=2.23,<3"
cmake --preset debug
cmake --build --preset debug --parallel
./build/debug/shijima-qt
```

For an installer Qt kit, add `-DCMAKE_PREFIX_PATH=/path/to/qt/kit`.
Linux extension resources are generated with Python.

### macOS

Install a Qt kit, or use MacPorts:

```sh
sudo port install cmake ninja python312 qt6-qtbase qt6-qtmultimedia
/opt/local/bin/python3.12 -m venv .venv
. .venv/bin/activate
python -m pip install "conan>=2.23,<3"
cmake --preset debug -DCMAKE_PREFIX_PATH=/opt/local/libexec/qt6 -DPython3_EXECUTABLE=/opt/local/bin/python3.12
cmake --build --preset debug --parallel
open build/debug/shijima-qt.app
```

### CLion and custom CMake profiles

Keep your Qt kit in `CMAKE_PREFIX_PATH` and inject the provider in the profile:

```text
-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake
```

Conan must be on the toolchain's `PATH`. For a custom Conan build or host
profile, set `CONAN_BUILD_PROFILE` or `CONAN_HOST_PROFILE`; the default host
profiles are `default;auto-cmake`. Select the `shijima-qt` run configuration
and add the Qt `bin` directory to its `PATH` on Windows.

When migrating an existing build directory from another dependency toolchain,
remove its old toolchain/profile arguments and reset the CMake cache, or run
`cmake --fresh --preset debug` with your Qt path.

### Installing and packaging

```sh
cmake --preset release
cmake --build --preset release --parallel
cmake --install build/release --prefix /path/to/staging
```

The installation contains the executable, licenses, and Linux desktop metadata
(or a macOS app bundle). Conan dependencies default to static libraries. Qt must
be available at runtime; with Qt 6.5+,
`-DSHIJIMA_DEPLOY_QT=ON` bundles its runtime libraries and plugins.

If you override Conan dependencies to shared libraries, activate the generated
`conanrun.bat` or `conanrun.sh` in
`<build-dir>/conan/build/<Debug-or-Release>/generators` before running or
installing. Native Windows installation uses that environment to locate DLLs.

Both CI workflows use CMake and the injected Conan provider. Release artifacts
use the same native toolchains as development builds, including MSVC on Windows.

The presets put build output under `build/debug` and `build/release`.
Pass extra options directly to CMake, such as
`cmake --preset release -DCMAKE_PREFIX_PATH=/path/to/qt -DSHIJIMA_DEPLOY_QT=ON`.

The optional Fedora development container includes Conan and MinGW Qt; cross
builds need matching CMake/Conan host configuration.

`SHIJIMA_USE_STUB_PLATFORM=ON` disables native desktop integration.

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
