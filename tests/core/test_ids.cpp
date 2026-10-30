#include "cutline/core/ids.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <set>
#include <type_traits>
#include <unordered_set>

using namespace cutline::core;

TEST_CASE("Default ids mean none", "[ids]") {
    CHECK_FALSE(ClipId{}.isValid());
    CHECK(ClipId{7}.isValid());
    CHECK(ClipId{7}.value() == 7);
    CHECK(ClipId{7} == ClipId{7});
    CHECK(ClipId{3} < ClipId{7});
}

TEST_CASE("Ids of different kinds do not mix", "[ids]") {
    static_assert(!std::is_convertible_v<ClipId, TrackId>);
    static_assert(!std::is_constructible_v<TrackId, ClipId>);
    static_assert(!std::is_convertible_v<std::uint64_t, ClipId>);
    static_assert(kIsId<MediaId> && !kIsId<std::uint64_t>);
    SUCCEED();
}

TEST_CASE("IdGenerator issues unique ids across all kinds", "[ids]") {
    IdGenerator ids;
    std::set<std::uint64_t> seen;
    for (int i = 0; i < 1000; ++i) {
        CHECK(seen.insert(ids.next<ClipId>().value()).second);
        CHECK(seen.insert(ids.next<TrackId>().value()).second);
        CHECK(seen.insert(ids.next<MediaId>().value()).second);
    }
    CHECK(seen.count(0) == 0);
    CHECK(ids.lastIssued() == 3000);
}

TEST_CASE("IdGenerator never reissues reserved ids", "[ids]") {
    IdGenerator ids;
    ids.reserve(41);
    CHECK(ids.next<ClipId>().value() == 42);
    ids.reserve(10); // lower than what was issued: no effect
    CHECK(ids.next<MediaId>().value() == 43);
}

TEST_CASE("Ids work as hash keys", "[ids]") {
    const std::unordered_set<ClipId> clips{ClipId{1}, ClipId{2}, ClipId{1}};
    CHECK(clips.size() == 2);
    CHECK(clips.contains(ClipId{2}));
}
