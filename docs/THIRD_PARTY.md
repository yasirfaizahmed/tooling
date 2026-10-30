# Third-party components

Cutline follows an **LGPL-only dependency policy**: nothing GPL-licensed or non-free is
bundled. This file lists every third-party component, its license, and whether it ships with
the app. Update it in the same commit that adds or changes a dependency.

## Shipped with the app

| Component | Version | License | How it's used |
|---|---|---|---|
| [Qt](https://www.qt.io/) (Core, Gui, Widgets, platform and style plugins) | 6.12 (release builds) | LGPL-3.0 | UI framework, dynamically linked |
| Microsoft Visual C++ runtime (Windows only) | VS 2022 | Microsoft redistributable license | C++ runtime DLLs copied next to `Cutline.exe` |

## Build and test only (not shipped)

| Component | License | Use |
|---|---|---|
| [Catch2](https://github.com/catchorg/Catch2) | BSL-1.0 | Unit tests for `core` |
| [vcpkg](https://github.com/microsoft/vcpkg) | MIT | Dependency builds |
| [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/) | BSD-3-Clause, Apache-2.0 | Build system |

## Planned (with the phase that adds them)

| Component | License | Phase | Notes |
|---|---|---|---|
| [FFmpeg](https://ffmpeg.org/) | LGPL-2.1+ | 1 | Built with `--disable-gpl --disable-nonfree --enable-shared` |
| [MLT](https://www.mltframework.org/) | LGPL-2.1+ | 1 | Built with `-DGPL=OFF`. Modules: core, avformat, plus, xml, sdl2 |
| [SDL2](https://www.libsdl.org/) | Zlib | 1 | Audio output for preview |
| [dav1d](https://code.videolan.org/videolan/dav1d) | BSD-2-Clause | 1 | AV1 decoding |
| [libvpx](https://chromium.googlesource.com/webm/libvpx) | BSD-3-Clause | 1 | VP8/VP9 (WebM) |
| [Opus](https://opus-codec.org/) | BSD-3-Clause | 1 | Opus audio (WebM) |
| [LAME](https://lame.sourceforge.io/) | LGPL-2.0+ | 5 | MP3 export |
| [OpenH264](https://www.openh264.org/) | BSD-2-Clause | 5 | Software H.264 fallback when no HW encoder exists. Cisco's patent license covers only Cisco-distributed binaries; this gets decided before the first public release |
| [nv-codec-headers](https://github.com/FFmpeg/nv-codec-headers) | MIT | 5 | NVENC/NVDEC interface (the NVIDIA driver provides the implementation) |
| [oneVPL / libvpl](https://github.com/intel/libvpl) | MIT | 5 | Intel Quick Sync |
| [AMF headers](https://github.com/GPUOpen-LibrariesAndSDKs/AMF) | MIT | 5 | AMD hardware encoding |
| [nlohmann/json](https://github.com/nlohmann/json) | MIT | 3 or 8 | Project file (de)serialization in `core` |

## Deliberately excluded (GPL or non-free)

x264, x265, fdk-aac, FFmpeg `--enable-gpl`/`--enable-nonfree`, MLT's `qt6`, `plusgpl`,
`rubberband`, `vidstab`, `resample`, `normalize`, `openfx` and `xine` modules, frei0r
plugins, movit.

## LGPL compliance

How Cutline meets its LGPL obligations (starting with the Phase 8 installers, and for any build
distributed before then):

- LGPL libraries are **dynamically linked** and shipped as separate shared libraries
  (`.dll`/`.so`/`.dylib`/frameworks), so users can swap in their own builds.
- License texts for every shipped component are included in the installers.
- The exact source versions and build flags are pinned in this repository (`vcpkg.json`, overlay
  ports, CI workflow), and the corresponding source is offered on request.
