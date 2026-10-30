#pragma once

#include <algorithm>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace cutline::core {

/// A strongly typed object id. Ids of different kinds do not convert into each other, so a
/// ClipId can never be passed where a TrackId is expected. The default value (0) means "none"
/// and never refers to an object.
template <typename Tag>
class Id {
public:
    constexpr Id() noexcept = default;
    constexpr explicit Id(std::uint64_t value) noexcept : value_(value) {}

    [[nodiscard]] constexpr std::uint64_t value() const noexcept { return value_; }
    [[nodiscard]] constexpr bool isValid() const noexcept { return value_ != 0; }

    friend constexpr auto operator<=>(const Id&, const Id&) noexcept = default;

private:
    std::uint64_t value_ = 0;
};

using MediaId = Id<struct MediaTag>;
using ClipId = Id<struct ClipTag>;
using TrackId = Id<struct TrackTag>;
using TransitionId = Id<struct TransitionTag>;
using MarkerId = Id<struct MarkerTag>;
/// Groups clips that move, trim and split together, e.g. the video and audio of one file.
using LinkId = Id<struct LinkTag>;

template <typename T>
inline constexpr bool kIsId = false;
template <typename Tag>
inline constexpr bool kIsId<Id<Tag>> = true;

/// Hands out project-unique ids. All id kinds share one counter, so every object in a project
/// has a distinct number, which keeps logs and project files unambiguous.
class IdGenerator {
public:
    template <typename IdType>
        requires kIsId<IdType>
    [[nodiscard]] IdType next() noexcept {
        return IdType{++last_};
    }

    /// Records an id that is already in use (e.g. read from a project file) so that next()
    /// never issues it again.
    void reserve(std::uint64_t usedValue) noexcept { last_ = std::max(last_, usedValue); }

    [[nodiscard]] std::uint64_t lastIssued() const noexcept { return last_; }

private:
    std::uint64_t last_ = 0;
};

} // namespace cutline::core

template <typename Tag>
struct std::hash<cutline::core::Id<Tag>> {
    std::size_t operator()(cutline::core::Id<Tag> id) const noexcept {
        return std::hash<std::uint64_t>{}(id.value());
    }
};
