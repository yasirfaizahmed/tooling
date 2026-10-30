#pragma once

#include "cutline/core/ids.h"
#include "cutline/core/rational.h"
#include "cutline/core/time.h"
#include "cutline/core/track.h"
#include "cutline/core/transition.h"

#include <cstdint>
#include <string>
#include <vector>

namespace cutline::core {

struct Resolution {
    int width = 0;
    int height = 0;

    friend bool operator==(const Resolution&, const Resolution&) = default;
};

enum class ChannelLayout { Mono, Stereo };

struct Marker {
    MarkerId id;
    FrameIndex frame = 0;
    std::string label;
    std::uint32_t colorRgb = 0xF5C542;

    friend bool operator==(const Marker&, const Marker&) = default;
};

/// The edit: a stack of video tracks composited bottom to top, plus audio tracks mixed together.
/// A plain value type; all changes go through edit commands so they can be undone.
struct Timeline {
    Rational frameRate = frame_rates::k30;
    Resolution resolution{1920, 1080};
    int sampleRate = 48000;
    ChannelLayout channelLayout = ChannelLayout::Stereo;

    /// videoTracks[0] is V1, the bottom layer; higher indices are drawn on top.
    std::vector<Track> videoTracks;
    /// audioTracks[0] is A1.
    std::vector<Track> audioTracks;
    std::vector<Transition> transitions;
    std::vector<Marker> markers;

    friend bool operator==(const Timeline&, const Timeline&) = default;
};

} // namespace cutline::core
