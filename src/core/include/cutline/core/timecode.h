#pragma once

#include "cutline/core/rational.h"
#include "cutline/core/time.h"

#include <cstdint>
#include <string>

namespace cutline::core {

/// The whole-number frame count that timecode uses per second: the frame rate rounded up
/// (29.97 -> 30, 23.976 -> 24). Throws std::domain_error unless frameRate > 0.
[[nodiscard]] std::int64_t timecodeBase(Rational frameRate);

/// Formats a timeline frame as non-drop-frame timecode "HH:MM:SS:FF". Fractional rates count
/// at their nominal rate, so at 29.97 fps "01:00:00:00" is 108000 frames (3603.6 s), as in
/// other editors. Hours grow past two digits when needed; negative frames get a '-' prefix.
[[nodiscard]] std::string formatTimecode(FrameIndex frame, Rational frameRate);

} // namespace cutline::core
