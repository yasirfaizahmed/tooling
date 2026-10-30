#pragma once

#include "cutline/core/rational.h"

#include <compare>
#include <cstdint>

namespace cutline::core {

/// A frame number on the timeline grid, at the project frame rate. Frame n is displayed during
/// [n / fps, (n + 1) / fps). Timeline positions and durations are always whole frames.
using FrameIndex = std::int64_t;

/// An exact instant or duration, in seconds. Used where values are not on the timeline grid,
/// e.g. offsets into a source file whose frame rate differs from the project's.
class RationalTime {
public:
    constexpr RationalTime() noexcept = default;
    constexpr explicit RationalTime(Rational seconds) noexcept : seconds_(seconds) {}

    /// The start time of frame `frame` at `frameRate`. Throws std::domain_error unless
    /// frameRate > 0.
    [[nodiscard]] static RationalTime fromFrames(FrameIndex frame, Rational frameRate);

    [[nodiscard]] constexpr Rational seconds() const noexcept { return seconds_; }

    /// The frame being displayed at this instant: floor(seconds * frameRate).
    /// This is the canonical time -> frame conversion; use it unless a caller has a
    /// documented reason not to.
    [[nodiscard]] FrameIndex frameAt(Rational frameRate) const;

    /// The frame boundary nearest to this instant; exact halves round towards +infinity.
    [[nodiscard]] FrameIndex nearestFrame(Rational frameRate) const;

    friend RationalTime operator+(RationalTime a, RationalTime b) {
        return RationalTime(a.seconds_ + b.seconds_);
    }
    friend RationalTime operator-(RationalTime a, RationalTime b) {
        return RationalTime(a.seconds_ - b.seconds_);
    }
    /// Scales a duration, e.g. by a clip's playback speed.
    friend RationalTime operator*(RationalTime time, Rational factor) {
        return RationalTime(time.seconds_ * factor);
    }

    friend constexpr bool operator==(const RationalTime&, const RationalTime&) noexcept = default;
    friend std::strong_ordering operator<=>(const RationalTime& a, const RationalTime& b) noexcept {
        return a.seconds_ <=> b.seconds_;
    }

private:
    Rational seconds_;
};

/// Index of the first audio sample that belongs to `frame`: floor(frame * sampleRate / fps).
/// Frames at non-integer sample counts (e.g. 48 kHz at 30000/1001 fps) get 1601 or 1602
/// samples in a fixed pattern, so audio never drifts against video.
[[nodiscard]] std::int64_t firstSampleOfFrame(FrameIndex frame, Rational frameRate,
                                              std::int64_t sampleRate);

/// Number of audio samples that belong to `frame` (see firstSampleOfFrame()).
[[nodiscard]] std::int64_t sampleCountOfFrame(FrameIndex frame, Rational frameRate,
                                              std::int64_t sampleRate);

/// Frame rates in common use.
namespace frame_rates {
inline constexpr Rational k23_976{24000, 1001};
inline constexpr Rational k24{24};
inline constexpr Rational k25{25};
inline constexpr Rational k29_97{30000, 1001};
inline constexpr Rational k30{30};
inline constexpr Rational k50{50};
inline constexpr Rational k59_94{60000, 1001};
inline constexpr Rational k60{60};
} // namespace frame_rates

} // namespace cutline::core
