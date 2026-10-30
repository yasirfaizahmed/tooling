#include "cutline/core/validate.h"

#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace cutline::core {

namespace {

std::string trackLabel(TrackKind kind, std::size_t index) {
    return (kind == TrackKind::Video ? "V" : "A") + std::to_string(index + 1);
}

template <typename IdType>
std::string idLabel(IdType id) {
    return "#" + std::to_string(id.value());
}

struct ClipLocation {
    TrackId track;
    const Clip* clip = nullptr;
};

class Validator {
public:
    explicit Validator(const Timeline& timeline) : timeline_(timeline) {}

    std::vector<std::string> run() {
        checkFormat();
        checkTracks(timeline_.videoTracks, TrackKind::Video);
        checkTracks(timeline_.audioTracks, TrackKind::Audio);
        checkTransitions();
        checkMarkers();
        return std::move(issues_);
    }

private:
    void fail(std::string message) { issues_.push_back(std::move(message)); }

    void checkFormat() {
        if (!timeline_.frameRate.isPositive()) {
            fail("frame rate must be positive, got " + timeline_.frameRate.toString());
        }
        const Resolution& res = timeline_.resolution;
        if (res.width <= 0 || res.height <= 0 || res.width % 2 != 0 || res.height % 2 != 0) {
            fail("resolution must be positive and even, got " + std::to_string(res.width) + "x" +
                 std::to_string(res.height));
        }
        if (timeline_.sampleRate <= 0) {
            fail("sample rate must be positive, got " + std::to_string(timeline_.sampleRate));
        }
    }

    void checkTracks(const std::vector<Track>& tracks, TrackKind expectedKind) {
        for (std::size_t index = 0; index < tracks.size(); ++index) {
            const Track& track = tracks[index];
            const std::string label = trackLabel(expectedKind, index);
            if (track.kind != expectedKind) {
                fail(label + ": track kind does not match the list it is in");
            }
            if (!track.id.isValid()) {
                fail(label + ": track has no id");
            } else if (!trackIds_.insert(track.id).second) {
                fail(label + ": duplicate track id " + idLabel(track.id));
            }
            checkClips(track, label);
        }
    }

    void checkClips(const Track& track, const std::string& trackName) {
        const Clip* previous = nullptr;
        for (const Clip& clip : track.clips) {
            const std::string label = trackName + " clip " + idLabel(clip.id);
            if (!clip.id.isValid()) {
                fail(label + ": clip has no id");
            } else if (!clips_.emplace(clip.id, ClipLocation{track.id, &clip}).second) {
                fail(label + ": duplicate clip id");
            }
            if (!clip.media.isValid()) {
                fail(label + ": clip does not reference any media");
            }
            if (clip.start < 0) {
                fail(label + ": starts before frame 0");
            }
            if (clip.length < 1) {
                fail(label + ": length must be at least 1 frame");
            }
            if (!clip.speed.isPositive()) {
                fail(label + ": speed must be positive");
            }
            if (clip.sourceIn.seconds().isNegative()) {
                fail(label + ": source in-point is negative");
            }
            checkFades(label + " audio", clip.audio.fadeIn, clip.audio.fadeOut, clip.length);
            checkFades(label + " video", clip.video.fadeIn, clip.video.fadeOut, clip.length);
            if (previous != nullptr) {
                if (clip.start < previous->start) {
                    fail(label + ": clips are not sorted by start");
                } else if (clip.start < previous->end()) {
                    fail(label + ": overlaps clip " + idLabel(previous->id));
                }
            }
            previous = &clip;
        }
    }

    void checkFades(const std::string& label, FrameIndex fadeIn, FrameIndex fadeOut,
                    FrameIndex length) {
        if (fadeIn < 0 || fadeOut < 0) {
            fail(label + " fade is negative");
        } else if (fadeIn + fadeOut > length) {
            fail(label + " fades are longer than the clip");
        }
    }

    void checkTransitions() {
        std::unordered_set<TransitionId> ids;
        std::unordered_set<ClipId> cutsWithTransition;
        for (const Transition& transition : timeline_.transitions) {
            const std::string label = "transition " + idLabel(transition.id);
            if (!transition.id.isValid()) {
                fail(label + ": transition has no id");
            } else if (!ids.insert(transition.id).second) {
                fail(label + ": duplicate transition id");
            }
            if (transition.length < 1) {
                fail(label + ": length must be at least 1 frame");
            }
            if (!trackIds_.contains(transition.track)) {
                fail(label + ": track " + idLabel(transition.track) + " does not exist");
                continue;
            }
            const auto left = clips_.find(transition.left);
            const auto right = clips_.find(transition.right);
            if (left == clips_.end() || right == clips_.end()) {
                fail(label + ": refers to a clip that does not exist");
                continue;
            }
            if (left->second.track != transition.track || right->second.track != transition.track) {
                fail(label + ": clips are not on the transition's track");
                continue;
            }
            if (left->second.clip->end() != right->second.clip->start) {
                fail(label + ": clips " + idLabel(transition.left) + " and " +
                     idLabel(transition.right) + " are not adjacent");
            }
            if (!cutsWithTransition.insert(transition.left).second) {
                fail(label + ": the cut after clip " + idLabel(transition.left) +
                     " already has a transition");
            }
        }
    }

    void checkMarkers() {
        std::unordered_set<MarkerId> ids;
        for (const Marker& marker : timeline_.markers) {
            const std::string label = "marker " + idLabel(marker.id);
            if (!marker.id.isValid()) {
                fail(label + ": marker has no id");
            } else if (!ids.insert(marker.id).second) {
                fail(label + ": duplicate marker id");
            }
            if (marker.frame < 0) {
                fail(label + ": is before frame 0");
            }
        }
    }

    const Timeline& timeline_;
    std::vector<std::string> issues_;
    std::unordered_set<TrackId> trackIds_;
    std::unordered_map<ClipId, ClipLocation> clips_;
};

} // namespace

std::vector<std::string> validate(const Timeline& timeline) {
    return Validator(timeline).run();
}

} // namespace cutline::core
