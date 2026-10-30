#pragma once

#include "cutline/core/clip.h"
#include "cutline/core/ids.h"

#include <string>
#include <vector>

namespace cutline::core {

enum class TrackKind { Video, Audio };

struct Track {
    TrackId id;
    TrackKind kind = TrackKind::Video;
    std::string name;

    bool muted = false;  ///< Audio tracks: silent. Video tracks: same as hidden.
    bool solo = false;   ///< Audio tracks: when any track is solo, only solo tracks are heard.
    bool locked = false; ///< Locked tracks ignore every edit operation.
    bool hidden = false; ///< Video tracks: not composited.
    double gainDb = 0.0;

    /// Sorted by start, never overlapping (see validate()).
    std::vector<Clip> clips;

    friend bool operator==(const Track&, const Track&) = default;
};

} // namespace cutline::core
