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

Install Qt 6.12, e.g. with the [Qt online installer](https://www.qt.io/download-qt-installer) or
[`aqtinstall`](https://github.com/miurahr/aqtinstall), and clone
[vcpkg](https://github.com/microsoft/vcpkg). On Windows, run this from a *Developer Command Prompt
for VS 2022*:

```bash
export VCPKG_ROOT=/path/to/vcpkg
cmake --preset release -DCMAKE_PREFIX_PATH=/path/to/Qt/6.12.0/<arch>
cmake --build --preset release
ctest --preset release
cmake --install build/release --prefix dist   # self-contained app in dist/
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
