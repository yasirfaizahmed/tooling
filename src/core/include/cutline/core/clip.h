#pragma once

#include "cutline/core/ids.h"
#include "cutline/core/param.h"
#include "cutline/core/rational.h"
#include "cutline/core/time.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace cutline::core {

/// Placement of a visual clip on the canvas. Positions are fractions of the canvas size, so a
/// project keeps its layout when the output resolution changes.
struct Transform {
    Param<double> positionX;       ///< Offset of the clip centre from the canvas centre.
    Param<double> positionY;       ///< Positive is downwards.
    Param<double> scale{1.0};      ///< 1.0 fits the clip to the canvas.
    Param<double> rotationDegrees; ///< Clockwise.
    bool flipHorizontal = false;
    bool flipVertical = false;

    friend bool operator==(const Transform&, const Transform&) = default;
};

/// Amount removed from each edge, as a fraction of the source size (independent of proxies).
struct Crop {
    Param<double> left;
    Param<double> top;
    Param<double> right;
    Param<double> bottom;

    friend bool operator==(const Crop&, const Crop&) = default;
};

struct ColorAdjust {
    Param<double> brightness;      ///< -1 .. 1, 0 is neutral.
    Param<double> contrast{1.0};   ///< 0 .. 2, 1 is neutral.
    Param<double> saturation{1.0}; ///< 0 .. 2, 1 is neutral; 0 is greyscale.

    friend bool operator==(const ColorAdjust&, const ColorAdjust&) = default;
};

struct VideoProps {
    Transform transform;
    Crop crop;
    Param<double> opacity{1.0};
    ColorAdjust color;
    FrameIndex fadeIn = 0; ///< Fade from black, in frames.
    FrameIndex fadeOut = 0;

    friend bool operator==(const VideoProps&, const VideoProps&) = default;
};

struct AudioProps {
    Param<double> gainDb;
    bool muted = false;
    FrameIndex fadeIn = 0; ///< Linear gain ramp, in frames.
    FrameIndex fadeOut = 0;

    friend bool operator==(const AudioProps&, const AudioProps&) = default;
};

using EffectValue = std::variant<double, std::int64_t, bool, std::string>;

/// A generic effect slot, so new effect types need no model changes.
struct Effect {
    std::string type;
    std::map<std::string, EffectValue> params;
    bool enabled = true;

    friend bool operator==(const Effect&, const Effect&) = default;
};

/// A use of a media asset on a track. Positions are timeline frames; the source offset is
/// exact time, because the source frame rate may differ from the project's.
struct Clip {
    ClipId id;
    MediaId media;

    FrameIndex start = 0;  ///< First timeline frame.
    FrameIndex length = 0; ///< Duration in timeline frames, at least 1.
    RationalTime sourceIn; ///< Source time shown at `start`.
    Rational speed{1};     ///< Source seconds per timeline second (0.25 .. 4).

    std::optional<LinkId> link;
    bool enabled = true;

    AudioProps audio;
    VideoProps video;
    std::vector<Effect> effects;

    /// One past the last frame: the clip occupies [start, end()).
    [[nodiscard]] FrameIndex end() const noexcept { return start + length; }

    friend bool operator==(const Clip&, const Clip&) = default;
};

} // namespace cutline::core
