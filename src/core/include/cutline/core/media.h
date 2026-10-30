#pragma once

#include "cutline/core/ids.h"
#include "cutline/core/rational.h"
#include "cutline/core/time.h"

#include <cstdint>
#include <optional>
#include <string>

namespace cutline::core {

enum class MediaKind { Video, Audio, Image };

/// Identifies a file's content so a moved or renamed file can be found again (relinking).
struct Fingerprint {
    std::uint64_t sizeBytes = 0;
    /// Hex digest of the file's first and last MiB; empty until computed.
    std::string contentHash;

    friend bool operator==(const Fingerprint&, const Fingerprint&) = default;
};

/// A source file in the project's media pool. Source files are never modified.
struct MediaAsset {
    MediaId id;
    std::string path; ///< Absolute path, UTF-8.
    Fingerprint fingerprint;
    MediaKind kind = MediaKind::Video;

    RationalTime duration; ///< Zero for still images, which can be shown for any length.
    Rational frameRate;    ///< Zero for audio and still images.
    int width = 0;
    int height = 0;
    bool hasVideo = false;
    bool hasAudio = false;
    bool isVariableFrameRate = false;
    int sampleRate = 0;
    int channels = 0;

    /// Low-resolution stand-in used for smooth preview; export always uses `path`.
    std::optional<std::string> proxyPath;

    friend bool operator==(const MediaAsset&, const MediaAsset&) = default;
};

} // namespace cutline::core
