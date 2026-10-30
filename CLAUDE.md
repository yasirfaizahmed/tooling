# CLAUDE.md

Working notes for anyone (human or AI) changing Cutline, a cross-platform non-linear
video/audio/image editor in C++20, Qt 6 and MLT/FFmpeg. The plan and its phases are in
[`docs/PLAN.md`](docs/PLAN.md).

## Build and test

```bash
# Linux with distribution packages (Qt 6, Catch2), no vcpkg:
cmake --preset system-debug && cmake --build --preset system-debug && ctest --preset system-debug

# Any OS with vcpkg (set VCPKG_ROOT; Qt from the Qt installer or aqtinstall):
cmake --preset debug -DCMAKE_PREFIX_PATH=/path/to/Qt/6.12.0/<arch>
cmake --build --preset debug && ctest --preset debug
```

- Presets: `debug`, `release`, `ci` (release + warnings as errors) use vcpkg; `system-debug`
  uses packages already installed on the machine.
- Tests run headless (`QT_QPA_PLATFORM=offscreen` is set by the test presets).
  `CUTLINE_SCREENSHOT=/path/shot.png ctest ...` saves a screenshot of the main window.
- `cmake --install build/<preset> --prefix dist` produces a self-contained app (Qt deployed
  next to it; needs Qt ≥ 6.5).
- **Before every commit:** build with warnings as errors and run `ctest`. Format changed files
  with `clang-format -i` (config in `.clang-format`).

## Architecture rules

- **`src/core` is pure C++20.** It must not include Qt, MLT or FFmpeg, and it does no I/O
  except project (de)serialization. Everything in it is unit-tested with Catch2.
- **`src/engine`** (from Phase 1) wraps MLT/FFmpeg. It may use QtCore/QtGui (e.g. for text
  rasterization), never Qt Widgets.
- **`src/app`** is the only layer with widgets. Everything except `main.cpp` lives in
  `cutline_app_lib` so tests can build real windows.
- **Every timeline change goes through a `Command`** on the `CommandStack` so it can be undone.
  UI code never mutates the model directly. A drag gesture commits exactly one command, on
  release.
- **Never block the UI thread.** Probing, thumbnails, waveforms, proxies and export run on
  workers. Export runs in a child process.
- **Source media is never modified.** The timeline is data, rendered at preview and export
  time.

## Time and numbers

- Timeline positions and durations are `FrameIndex` (`int64` frames at the project rate).
  Offsets into source files are `RationalTime` (exact seconds). Frame rates and speeds are
  `Rational`.
- **Never use floating point for time.** `Rational::toDouble()` is for display and on-screen
  geometry only.
- The canonical time→frame conversion is `RationalTime::frameAt()` (floor). Use
  `firstSampleOfFrame()` for audio sample positions.
- `Rational` arithmetic throws `std::overflow_error` rather than wrapping.

## Model invariants

Check them with `core::validate()`, which every edit-operation test calls afterwards:
- Clips on a track are sorted by `start` and never overlap. `length ≥ 1`, `speed > 0`, and fades
  fit inside the clip.
- `videoTracks[0]` is V1, the bottom layer; higher indices draw on top.
- Ids are typed (`ClipId`, `TrackId`, ...) and come from one `IdGenerator` per project.
  `Id{}` means "none".
- Visual parameters are `Param<T>` (keyframes later) and are resolution-independent: positions
  and crops are fractions.

## Dependencies and licensing

- **LGPL-only dependency policy.** Do not add GPL or non-free components (x264, x265, MLT
  `qt6`/`plusgpl`/`rubberband`/`vidstab`, frei0r, movit, `--enable-gpl` FFmpeg) without an
  explicit decision from the project owner.
- Every bundled dependency is listed with its license in `docs/THIRD_PARTY.md`. Update it in the
  same commit that adds or changes a dependency.
- vcpkg's `builtin-baseline` in `vcpkg.json` must equal `VCPKG_COMMIT` in
  `.github/workflows/ci.yml`.

## Code style

- Files `snake_case.{h,cpp}`; types `PascalCase`; functions and variables `camelCase`; members
  `trailingUnderscore_`; constants `kPascalCase`; namespaces `cutline::core`, `cutline::app`.
- `#pragma once`. Project includes use quotes (`"cutline/core/rational.h"`,
  `"panels/preview/preview_panel.h"`). clang-format orders the include groups.
- Use value types and RAII; no owning raw pointers. Qt objects owned by a parent widget are the
  exception.
- Core reports programming errors (invalid arguments, overflow) with exceptions. Recoverable,
  user-facing failures (e.g. an edit that isn't allowed) return a result type.
- UI strings go through `tr()`. Widgets styled by `theme/style.qss` get an `objectName`. Colours
  come from `theme::Colors` tokens, never hard-coded in widgets.
- Comments explain *why* and document contracts (units, ranges, invariants). Don't narrate
  the code.

## Workflow

- Work one phase at a time (see `docs/PLAN.md`). At the end of each phase the app builds and
  runs, tests pass, and the owner gets a summary plus manual test steps before the next
  phase starts.
- Commit after each meaningful step, with an imperative subject line and a body that explains
  why.
- If something is ambiguous and the decision is big (stack, licensing, file format,
  architecture), ask instead of guessing.
