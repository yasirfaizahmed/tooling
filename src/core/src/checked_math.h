#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>

// Overflow-checked 64-bit integer arithmetic. Deliberately portable (no compiler builtins) so
// the exact same code runs and is tested on every platform.
namespace cutline::core::detail {

[[noreturn]] inline void throwOverflow() {
    throw std::overflow_error("Rational: arithmetic overflow");
}

inline std::int64_t checkedAdd(std::int64_t a, std::int64_t b) {
    constexpr auto kMax = std::numeric_limits<std::int64_t>::max();
    constexpr auto kMin = std::numeric_limits<std::int64_t>::min();
    if ((b > 0 && a > kMax - b) || (b < 0 && a < kMin - b)) {
        throwOverflow();
    }
    return a + b;
}

inline std::uint64_t magnitude(std::int64_t value) noexcept {
    // Unsigned negation is well defined, including for INT64_MIN.
    return value < 0 ? std::uint64_t{0} - static_cast<std::uint64_t>(value)
                     : static_cast<std::uint64_t>(value);
}

inline std::int64_t checkedMul(std::int64_t a, std::int64_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    const bool negative = (a < 0) != (b < 0);
    const std::uint64_t ua = magnitude(a);
    const std::uint64_t ub = magnitude(b);
    constexpr std::uint64_t kNegativeLimit = std::uint64_t{1} << 63;
    constexpr auto kPositiveLimit =
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
    const std::uint64_t limit = negative ? kNegativeLimit : kPositiveLimit;
    if (ua > limit / ub) {
        throwOverflow();
    }
    const std::uint64_t product = ua * ub;
    if (!negative) {
        return static_cast<std::int64_t>(product);
    }
    if (product == kNegativeLimit) {
        return std::numeric_limits<std::int64_t>::min();
    }
    return -static_cast<std::int64_t>(product);
}

struct FloorDivMod {
    std::int64_t quotient;
    std::int64_t remainder; // always in [0, divisor)
};

/// Floor division for a positive divisor, without any intermediate overflow.
constexpr FloorDivMod floorDivMod(std::int64_t dividend, std::int64_t divisor) noexcept {
    std::int64_t quotient = dividend / divisor;
    std::int64_t remainder = dividend % divisor;
    if (remainder < 0) {
        remainder += divisor;
        --quotient;
    }
    return {quotient, remainder};
}

} // namespace cutline::core::detail
