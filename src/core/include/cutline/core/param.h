#pragma once

namespace cutline::core {

/// A clip parameter that may be animated in the future. Today it holds a single constant
/// value; keyframes will be added here, so constant parameters keep the same model and
/// project-file shape when animation arrives.
template <typename T>
struct Param {
    T value{};

    friend bool operator==(const Param&, const Param&) = default;
};

} // namespace cutline::core
