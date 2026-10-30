#include "cutline/core/time.h"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <stdexcept>

using namespace cutline::core;
using namespace cutline::core::frame_rates;

TEST_CASE("Frame start times are exact", "[time]") {
    CHECK(RationalTime::fromFrames(30, k29_97).seconds() == Rational(1001, 1000));
    CHECK(RationalTime::fromFrames(0, k24).seconds().isZero());
    CHECK(RationalTime::fromFrames(-1, k25).seconds() == Rational(-1, 25));
    CHECK(RationalTime::fromFrames(1, k23_976).seconds() == Rational(1001, 24000));
}

TEST_CASE("Frames round-trip through time at every common rate", "[time]") {
    for (const Rational fps : {k23_976, k24, k25, k29_97, k30, k50, k59_94, k60}) {
        for (const FrameIndex frame :
             {FrameIndex{0}, FrameIndex{1}, FrameIndex{2}, FrameIndex{999}, FrameIndex{1'000'000},
              FrameIndex{10'000'000}, FrameIndex{-5}}) {
            INFO("fps " << fps.toString() << ", frame " << frame);
            const RationalTime start = RationalTime::fromFrames(frame, fps);
            CHECK(start.frameAt(fps) == frame);
            CHECK(start.nearestFrame(fps) == frame);
        }
    }
}

TEST_CASE("frameAt returns the frame on screen at an instant", "[time]") {
    // At 30 fps, frame 1 is on screen during [1/30 s, 2/30 s).
    CHECK(RationalTime(Rational(1, 30)).frameAt(k30) == 1);
    CHECK(RationalTime(Rational(1, 30) - Rational(1, 1'000'000)).frameAt(k30) == 0);
    CHECK(RationalTime(Rational(2, 30) - Rational(1, 1'000'000)).frameAt(k30) == 1);

    // 59/900 s is 1.97 frames in: frame 1 is on screen, frame 2's boundary is nearest.
    CHECK(RationalTime(Rational(59, 900)).frameAt(k30) == 1);
    CHECK(RationalTime(Rational(59, 900)).nearestFrame(k30) == 2);
}

TEST_CASE("Source offsets convert exactly between frame rates", "[time]") {
    // Frame 1001 of a 23.976 fps source...
    const RationalTime sourceTime = RationalTime::fromFrames(1001, k23_976);
    CHECK(sourceTime.seconds() == Rational(1001 * 1001, 24000));
    // ...is 1251.25 frames into a 29.97 fps timeline.
    CHECK(sourceTime.frameAt(k29_97) == 1251);
    CHECK(sourceTime.nearestFrame(k29_97) == 1251);
}

TEST_CASE("RationalTime arithmetic", "[time]") {
    const RationalTime a(Rational(3, 2));
    const RationalTime b(Rational(1, 4));
    CHECK(a + b == RationalTime(Rational(7, 4)));
    CHECK(a - b == RationalTime(Rational(5, 4)));
    CHECK(RationalTime(Rational(10)) * Rational(1, 4) == RationalTime(Rational(5, 2)));
    CHECK(b < a);
    CHECK(a > b);
    CHECK(RationalTime() == RationalTime(Rational(0)));
}

TEST_CASE("Non-positive frame and sample rates are rejected", "[time]") {
    CHECK_THROWS_AS(RationalTime::fromFrames(1, Rational(0)), std::domain_error);
    CHECK_THROWS_AS(RationalTime(Rational(1)).frameAt(Rational(-25)), std::domain_error);
    CHECK_THROWS_AS(RationalTime(Rational(1)).nearestFrame(Rational(0)), std::domain_error);
    CHECK_THROWS_AS(firstSampleOfFrame(1, k30, 0), std::domain_error);
}

TEST_CASE("Audio samples per frame follow a drift-free pattern", "[time]") {
    // 48 kHz at 29.97 fps is 1601.6 samples per frame, distributed in a five-frame cycle.
    constexpr std::array<std::int64_t, 5> kCycle{1601, 1602, 1601, 1602, 1602};
    for (FrameIndex frame = 0; frame < 20; ++frame) {
        INFO("frame " << frame);
        CHECK(sampleCountOfFrame(frame, k29_97, 48000) ==
              kCycle[static_cast<std::size_t>(frame % 5)]);
    }
    CHECK(firstSampleOfFrame(5, k29_97, 48000) == 8008);

    // One hour of frames (108000) is exactly 3603.6 s of audio: no accumulated drift.
    CHECK(firstSampleOfFrame(108000, k29_97, 48000) == 172'972'800);

    // 48 kHz at 25 fps divides evenly; 44.1 kHz at 24 fps (1837.5) alternates.
    CHECK(sampleCountOfFrame(12345, k25, 48000) == 1920);
    CHECK(sampleCountOfFrame(6, k24, 44100) == 1837);
    CHECK(sampleCountOfFrame(7, k24, 44100) == 1838);
}
