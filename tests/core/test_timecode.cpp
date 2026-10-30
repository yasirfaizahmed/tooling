#include "cutline/core/timecode.h"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

using namespace cutline::core;
using namespace cutline::core::frame_rates;

TEST_CASE("Timecode base is the frame rate rounded up", "[timecode]") {
    CHECK(timecodeBase(k23_976) == 24);
    CHECK(timecodeBase(k25) == 25);
    CHECK(timecodeBase(k29_97) == 30);
    CHECK(timecodeBase(k59_94) == 60);
    CHECK_THROWS_AS(timecodeBase(Rational(0)), std::domain_error);
}

TEST_CASE("Frames format as HH:MM:SS:FF", "[timecode]") {
    CHECK(formatTimecode(0, k30) == "00:00:00:00");
    CHECK(formatTimecode(29, k30) == "00:00:00:29");
    CHECK(formatTimecode(30, k30) == "00:00:01:00");
    CHECK(formatTimecode(30 * 61 + 5, k30) == "00:01:01:05");
    CHECK(formatTimecode(108000, k30) == "01:00:00:00");
    CHECK(formatTimecode(24 * 60, k23_976) == "00:01:00:00");
    CHECK(formatTimecode(25 * 3600 * 100, k25) == "100:00:00:00");
}

TEST_CASE("Fractional rates count at their nominal rate", "[timecode]") {
    // Non-drop-frame: 108000 frames at 29.97 fps read as one hour, though 3603.6 s elapse.
    CHECK(formatTimecode(108000, k29_97) == "01:00:00:00");
    CHECK(formatTimecode(59, k59_94) == "00:00:00:59");
}

TEST_CASE("Negative frames get a sign", "[timecode]") {
    CHECK(formatTimecode(-1, k25) == "-00:00:00:01");
    CHECK(formatTimecode(-26, k25) == "-00:00:01:01");
}
