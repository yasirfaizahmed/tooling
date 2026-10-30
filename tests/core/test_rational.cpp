#include "cutline/core/rational.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>

using cutline::core::Rational;

namespace {
constexpr std::int64_t kMax = std::numeric_limits<std::int64_t>::max();
constexpr std::int64_t kMin = std::numeric_limits<std::int64_t>::min();
} // namespace

TEST_CASE("Rational normalizes on construction", "[rational]") {
    CHECK(Rational(2, 4).num() == 1);
    CHECK(Rational(2, 4).den() == 2);
    CHECK(Rational(3, -6).num() == -1);
    CHECK(Rational(3, -6).den() == 2);
    CHECK(Rational(-3, -6) == Rational(1, 2));
    CHECK(Rational(0, 7).den() == 1);
    CHECK(Rational().isZero());
    CHECK(Rational(30000, 1001).num() == 30000);
    CHECK(Rational(30000, 1001).den() == 1001);
}

TEST_CASE("Rational works in constant expressions", "[rational]") {
    constexpr Rational ntsc(60000, 2002);
    static_assert(ntsc.num() == 30000 && ntsc.den() == 1001);
    static_assert(Rational(4, 2).isInteger());
}

TEST_CASE("Rational rejects invalid values", "[rational]") {
    CHECK_THROWS_AS(Rational(1, 0), std::domain_error);
    CHECK_THROWS_AS(Rational(kMin, 1), std::overflow_error);
    CHECK_THROWS_AS(Rational(1, kMin), std::overflow_error);
    CHECK_THROWS_AS(Rational(0).reciprocal(), std::domain_error);
    CHECK_THROWS_AS(Rational(1) / Rational(0), std::domain_error);
}

TEST_CASE("Rational arithmetic is exact", "[rational]") {
    const Rational ntsc(30000, 1001);
    CHECK(Rational(1, 3) + Rational(1, 6) == Rational(1, 2));
    CHECK(Rational(1, 3) - Rational(1, 2) == Rational(-1, 6));
    CHECK(Rational(2, 3) * Rational(9, 4) == Rational(3, 2));
    CHECK(Rational(2, 3) / Rational(4, 9) == Rational(3, 2));
    CHECK(-Rational(5, 7) == Rational(-5, 7));
    CHECK(Rational(-5, 7).reciprocal() == Rational(-7, 5));

    // One hour of 29.97 fps frames lasts exactly 3603.6 seconds.
    CHECK(Rational(108000) / ntsc == Rational(36036, 10));

    // Adding 1/10 ten times gives exactly 1, unlike binary floating point.
    Rational sum;
    for (int i = 0; i < 10; ++i) {
        sum += Rational(1, 10);
    }
    CHECK(sum == Rational(1));
}

TEST_CASE("Rational arithmetic cancels common factors before multiplying", "[rational]") {
    CHECK(Rational(kMax, 3) * Rational(3, kMax) == Rational(1));
    CHECK(Rational(kMax - 1, kMax) + Rational(1, kMax) == Rational(1));
    CHECK(Rational(kMax, 2) / Rational(kMax, 4) == Rational(2));
}

TEST_CASE("Rational arithmetic throws instead of wrapping around", "[rational]") {
    CHECK_THROWS_AS(Rational(kMax) + Rational(1), std::overflow_error);
    CHECK_THROWS_AS(Rational(kMax) * Rational(2), std::overflow_error);
    CHECK_THROWS_AS(Rational(1, kMax) + Rational(1, kMax - 1), std::overflow_error);
    CHECK_THROWS_AS(-Rational(kMax) - Rational(1), std::overflow_error);
    CHECK_THROWS_AS(-Rational(kMax) - Rational(2), std::overflow_error);
}

TEST_CASE("Rational comparison is exact", "[rational]") {
    CHECK(Rational(1, 3) < Rational(1, 2));
    CHECK(Rational(-1, 2) < Rational(-1, 3));
    CHECK(Rational(-7, 2) < Rational(-10, 3));
    CHECK(Rational(30000, 1001) > Rational(2997, 100));
    CHECK(Rational(3) > Rational(5, 2));
    CHECK(Rational(2) < Rational(5, 2));
    CHECK(Rational(6, 4) <= Rational(3, 2));
    CHECK(Rational(6, 4) >= Rational(3, 2));
    CHECK(Rational(1, 2) != Rational(1, 3));

    // Cross-multiplying these would overflow 64 bits.
    CHECK(Rational(kMax - 1, kMax) > Rational(kMax - 2, kMax - 1));
    CHECK(Rational(-(kMax - 1), kMax) < Rational(-(kMax - 2), kMax - 1));
    CHECK(Rational(kMax, kMax - 1) < Rational(kMax - 1, kMax - 2));
}

TEST_CASE("Rational rounds to integers", "[rational]") {
    CHECK(Rational(7, 2).floor() == 3);
    CHECK(Rational(7, 2).ceil() == 4);
    CHECK(Rational(7, 2).round() == 4);

    CHECK(Rational(-7, 2).floor() == -4);
    CHECK(Rational(-7, 2).ceil() == -3);
    CHECK(Rational(-7, 2).round() == -3); // halves round towards +infinity

    CHECK(Rational(4, 3).round() == 1);
    CHECK(Rational(5, 3).round() == 2);
    CHECK(Rational(-5, 3).round() == -2);
    CHECK(Rational(-4, 3).round() == -1);

    CHECK(Rational(6, 3).floor() == 2);
    CHECK(Rational(6, 3).ceil() == 2);
    CHECK(Rational(6, 3).round() == 2);

    CHECK(Rational(kMax - 1, kMax).round() == 1);
    CHECK(Rational(1, kMax).ceil() == 1);
}

TEST_CASE("Rational converts to and from text", "[rational]") {
    CHECK(Rational(30000, 1001).toString() == "30000/1001");
    CHECK(Rational(-4, 2).toString() == "-2");
    CHECK(Rational().toString() == "0");

    CHECK(Rational::parse("30000/1001") == Rational(30000, 1001));
    CHECK(Rational::parse("-6/4") == Rational(-3, 2));
    CHECK(Rational::parse("25") == Rational(25));
    CHECK(Rational::parse("0/9") == Rational(0));

    for (const Rational value : {Rational(24000, 1001), Rational(-1, 3), Rational(kMax, 7),
                                 Rational(-kMax), Rational(0)}) {
        CHECK(Rational::parse(value.toString()) == value);
    }

    for (const char* malformed : {"", "/", "1/", "/2", "1/0", "1/-2", "+1", " 1", "1 ", "1.5",
                                  "abc", "1/2/3", "99999999999999999999", "-9223372036854775808"}) {
        INFO("input: \"" << malformed << '"');
        CHECK_FALSE(Rational::parse(malformed).has_value());
    }
}

TEST_CASE("Rational approximates as double for display", "[rational]") {
    CHECK(Rational(1, 4).toDouble() == 0.25);
    CHECK(Rational(-3, 2).toDouble() == -1.5);
}
