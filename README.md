# Cutline

A lightweight, cross-platform non-linear editor for video, audio and images, for Windows, Linux
and macOS. The aim is a small set of basic features done really well: a multi-track timeline,
precise cutting and trimming, background music, smooth real-time preview, and reliable export.

> **Status:** Phase 0 of 8. The app skeleton, data-model foundations and CI exist; media import
> and playback come next. See [`docs/PLAN.md`](docs/PLAN.md) for the roadmap.

Built with C++20, Qt 6, and (from Phase 1) MLT and FFmpeg. Only LGPL-compatible components are
bundled; see [`docs/THIRD_PARTY.md`](docs/THIRD_PARTY.md).

## Building

Requirements: CMake ≥ 3.25, Ninja, a C++20 compiler (GCC 12+, Clang 15+, MSVC 2022), and Qt ≥ 6.4.
CI and releases use Qt 6.12.

### Linux, with distribution packages

```bash
sudo apt install cmake ninja-build g++ qt6-base-dev catch2   # Ubuntu 24.04
cmake --preset system-debug
cmake --build --preset system-debug
ctest --preset system-debug
./build/system-debug/src/app/cutline
```

### Any OS, with vcpkg

You need Qt 6.12, from the [Qt online installer](https://www.qt.io/download-qt-installer) or
[`aqtinstall`](https://github.com/miurahr/aqtinstall), and a clone of
[vcpkg](https://github.com/microsoft/vcpkg), bootstrapped with `bootstrap-vcpkg.bat` (Windows)
or `./bootstrap-vcpkg.sh` (Linux, macOS). The first configure downloads and builds the
dependencies, which takes a few minutes.

**Windows, one command.** From the repository folder, in any Command Prompt or PowerShell
window, run:

```bat
scripts\build-windows.bat
```

It finds Visual Studio, Qt and vcpkg (cloning vcpkg if it's missing), then builds, tests,
installs into `dist\` and starts the app. If a prerequisite is missing, it says which one and
how to install it. CI runs this same script on every push.

**Windows, step by step.** Install Visual Studio 2022 (or its Build Tools) with the *Desktop
development with C++* workload, which includes CMake and Ninja. Install Qt's **MSVC 2022 64-bit**
component; the MinGW one won't work. Then open **x64 Native Tools Command Prompt for VS 2022**. The plain
*Developer Command Prompt* targets 32-bit and fails to link against 64-bit Qt. A regular
PowerShell or terminal window won't work either: it has no compiler or Ninja on PATH, and its
`set` doesn't set environment variables. Keep the clone outside OneDrive (e.g. `C:\dev`),
because syncing the build folder causes locked-file errors.

```bat
cd C:\path\to\tooling
rem The VS prompt points VCPKG_ROOT at Visual Studio's bundled vcpkg, which can't use the
rem pinned baseline, so point it at your clone:
set VCPKG_ROOT=C:\path\to\vcpkg
rem Qt's DLLs must be on PATH for the tests and for running from the build tree:
set PATH=C:\Qt\6.12.0\msvc2022_64\bin;%PATH%

cmake --preset release -DCMAKE_PREFIX_PATH=C:\Qt\6.12.0\msvc2022_64
cmake --build --preset release
ctest --preset release
cmake --install build\release --prefix dist
dist\bin\Cutline.exe
```

**Linux and macOS** (macOS: `xcode-select --install` and `brew install ninja`; Linux: `ninja-build`
and `libgl1-mesa-dev`):

```bash
export VCPKG_ROOT=~/vcpkg
cmake --preset release -DCMAKE_PREFIX_PATH=~/Qt/6.12.0/gcc_64   # macOS: ~/Qt/6.12.0/macos
cmake --build --preset release
ctest --preset release
cmake --install build/release --prefix dist   # dist/bin/cutline, or dist/Cutline.app on macOS
```

Every push builds and tests on Linux, Windows and macOS. App archives and a screenshot from
each OS are attached to the GitHub Actions run.

## Repository layout

| Path | Contents |
|---|---|
| `src/core` | Timeline data model, exact time arithmetic, validation (pure C++, no Qt) |
| `src/app` | Qt Widgets application: main window, panels, theme |
| `tests` | Catch2 unit tests (`core`) and headless Qt Test smoke tests (`app`) |
| `docs` | Project plan and third-party licenses |
| `CLAUDE.md` | Conventions for contributors |

## License

Cutline's own license hasn't been chosen yet. Third-party components keep their own licenses
(see [`docs/THIRD_PARTY.md`](docs/THIRD_PARTY.md)).
