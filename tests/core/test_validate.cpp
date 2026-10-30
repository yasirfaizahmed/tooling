#include "cutline/core/validate.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

using namespace cutline::core;

namespace {

class TimelineBuilder {
public:
    TimelineBuilder() {
        // Tests keep references to tracks while adding more, so the lists must not reallocate.
        timeline.videoTracks.reserve(8);
        timeline.audioTracks.reserve(8);
    }

    Track& addTrack(TrackKind kind) {
        auto& tracks = kind == TrackKind::Video ? timeline.videoTracks : timeline.audioTracks;
        Track& track = tracks.emplace_back();
        track.id = ids.next<TrackId>();
        track.kind = kind;
        return track;
    }

    Clip& addClip(Track& track, FrameIndex start, FrameIndex length) {
        Clip& clip = track.clips.emplace_back();
        clip.id = ids.next<ClipId>();
        clip.media = MediaId{1000};
        clip.start = start;
        clip.length = length;
        return clip;
    }

    IdGenerator ids;
    Timeline timeline;
};

Transition makeTransition(TransitionId id, TrackId track, ClipId left, ClipId right,
                          FrameIndex length) {
    Transition transition;
    transition.id = id;
    transition.track = track;
    transition.left = left;
    transition.right = right;
    transition.length = length;
    return transition;
}

bool mentions(const std::vector<std::string>& issues, std::string_view text) {
    return std::any_of(issues.begin(), issues.end(), [text](const std::string& issue) {
        return issue.find(text) != std::string::npos;
    });
}

std::string joined(const std::vector<std::string>& issues) {
    std::string all;
    for (const std::string& issue : issues) {
        all += issue + "\n";
    }
    return all;
}

} // namespace

TEST_CASE("An empty timeline is valid", "[validate]") {
    CHECK(validate(Timeline{}).empty());
}

TEST_CASE("A well-formed multi-track timeline is valid", "[validate]") {
    TimelineBuilder b;
    Track& v1 = b.addTrack(TrackKind::Video);
    const Clip first = b.addClip(v1, 0, 100);
    const Clip second = b.addClip(v1, 100, 50);
    b.addClip(v1, 200, 1);
    Track& v2 = b.addTrack(TrackKind::Video);
    b.addClip(v2, 20, 30).video.fadeIn = 10;
    Track& a1 = b.addTrack(TrackKind::Audio);
    Clip& music = b.addClip(a1, 0, 300);
    music.audio.fadeIn = 10;
    music.audio.fadeOut = 290;

    b.timeline.transitions.push_back(
        makeTransition(b.ids.next<TransitionId>(), v1.id, first.id, second.id, 12));
    Marker& marker = b.timeline.markers.emplace_back();
    marker.id = b.ids.next<MarkerId>();
    marker.frame = 42;

    const auto issues = validate(b.timeline);
    INFO(joined(issues));
    CHECK(issues.empty());
}

TEST_CASE("Overlapping and unsorted clips are reported", "[validate]") {
    TimelineBuilder b;
    Track& v1 = b.addTrack(TrackKind::Video);
    b.addClip(v1, 0, 100);
    b.addClip(v1, 99, 10);
    CHECK(mentions(validate(b.timeline), "overlaps"));

    v1.clips.back().start = 100; // touching is fine
    CHECK(validate(b.timeline).empty());

    b.addClip(v1, 50, 10);
    CHECK(mentions(validate(b.timeline), "not sorted"));
}

TEST_CASE("Clips need a length, a positive speed and media", "[validate]") {
    TimelineBuilder b;
    Track& a1 = b.addTrack(TrackKind::Audio);
    b.addClip(a1, 0, 0);
    b.addClip(a1, 10, 5).speed = Rational(0);
    b.addClip(a1, 20, 5).media = MediaId{};
    b.addClip(a1, -1, 1);
    b.addClip(a1, 30, 5).sourceIn = RationalTime(Rational(-1, 2));

    const auto issues = validate(b.timeline);
    CHECK(mentions(issues, "at least 1 frame"));
    CHECK(mentions(issues, "speed must be positive"));
    CHECK(mentions(issues, "does not reference any media"));
    CHECK(mentions(issues, "before frame 0"));
    CHECK(mentions(issues, "in-point is negative"));
}

TEST_CASE("Fades must fit inside the clip", "[validate]") {
    TimelineBuilder b;
    Track& a1 = b.addTrack(TrackKind::Audio);
    Clip& clip = b.addClip(a1, 0, 50);
    clip.audio.fadeIn = 30;
    clip.audio.fadeOut = 21;
    CHECK(mentions(validate(b.timeline), "audio fades are longer than the clip"));

    clip.audio.fadeOut = 20;
    clip.video.fadeIn = -1;
    CHECK(mentions(validate(b.timeline), "video fade is negative"));
}

TEST_CASE("Ids must be present and unique", "[validate]") {
    TimelineBuilder b;
    Track& v1 = b.addTrack(TrackKind::Video);
    Track& v2 = b.addTrack(TrackKind::Video);
    const ClipId shared = b.addClip(v1, 0, 10).id;
    b.addClip(v2, 0, 10).id = shared;
    CHECK(mentions(validate(b.timeline), "duplicate clip id"));

    v2.clips.back().id = b.ids.next<ClipId>();
    v2.id = v1.id;
    CHECK(mentions(validate(b.timeline), "duplicate track id"));

    v2.id = TrackId{};
    CHECK(mentions(validate(b.timeline), "track has no id"));
}

TEST_CASE("Tracks must sit in the list matching their kind", "[validate]") {
    TimelineBuilder b;
    b.addTrack(TrackKind::Video).kind = TrackKind::Audio;
    const auto issues = validate(b.timeline);
    CHECK(mentions(issues, "V1: track kind does not match"));
}

TEST_CASE("Transitions must join adjacent clips on their own track", "[validate]") {
    TimelineBuilder b;
    Track& v1 = b.addTrack(TrackKind::Video);
    Track& v2 = b.addTrack(TrackKind::Video);
    const ClipId left = b.addClip(v1, 0, 100).id;
    const ClipId gapped = b.addClip(v1, 150, 50).id;
    const ClipId elsewhere = b.addClip(v2, 100, 50).id;

    auto withTransition = [&](TrackId track, ClipId from, ClipId to, FrameIndex length) {
        Timeline copy = b.timeline;
        copy.transitions.push_back(makeTransition(TransitionId{900}, track, from, to, length));
        return validate(copy);
    };

    CHECK(mentions(withTransition(v1.id, left, gapped, 10), "are not adjacent"));
    CHECK(mentions(withTransition(v1.id, left, elsewhere, 10), "not on the transition's track"));
    CHECK(mentions(withTransition(v1.id, left, ClipId{12345}, 10), "does not exist"));
    CHECK(mentions(withTransition(TrackId{12345}, left, gapped, 10), "does not exist"));
    CHECK(mentions(withTransition(v1.id, left, gapped, 0), "at least 1 frame"));
}

TEST_CASE("The output format must be encodable", "[validate]") {
    Timeline timeline;
    timeline.resolution = {1921, 1080};
    timeline.frameRate = Rational(0);
    timeline.sampleRate = 0;
    const auto issues = validate(timeline);
    CHECK(mentions(issues, "resolution must be positive and even"));
    CHECK(mentions(issues, "frame rate must be positive"));
    CHECK(mentions(issues, "sample rate must be positive"));
}
