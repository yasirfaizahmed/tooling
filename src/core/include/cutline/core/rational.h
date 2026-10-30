#pragma once

#include <compare>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace cutline::core {

/// An exact rational number num/den, used for frame rates, speeds and time.
///
/// Always normalized: den > 0 and gcd(|num|, den) == 1, so two equal values have identical
/// members. Numerator and denominator must lie within ±(2^63 - 1). Arithmetic never wraps:
/// a result that does not fit throws std::overflow_error, and dividing by zero (or
/// constructing with den == 0) throws std::domain_error.
class Rational {
public:
    constexpr Rational() noexcept = default;

    constexpr explicit Rational(std::int64_t num, std::int64_t den = 1) : num_(num), den_(den) {
        normalize();
    }

    [[nodiscard]] constexpr std::int64_t num() const noexcept { return num_; }
    [[nodiscard]] constexpr std::int64_t den() const noexcept { return den_; }

    [[nodiscard]] constexpr bool isZero() const noexcept { return num_ == 0; }
    [[nodiscard]] constexpr bool isPositive() const noexcept { return num_ > 0; }
    [[nodiscard]] constexpr bool isNegative() const noexcept { return num_ < 0; }
    [[nodiscard]] constexpr bool isInteger() const noexcept { return den_ == 1; }

    /// Largest integer not greater than this value.
    [[nodiscard]] std::int64_t floor() const noexcept;
    /// Smallest integer not less than this value.
    [[nodiscard]] std::int64_t ceil() const noexcept;
    /// Nearest integer; exact halves round towards +infinity (2.5 -> 3, -2.5 -> -2).
    [[nodiscard]] std::int64_t round() const noexcept;

    /// 1 / value. Throws std::domain_error for zero.
    [[nodiscard]] Rational reciprocal() const;

    /// Approximate value for display and on-screen geometry only. Never use it for timing.
    [[nodiscard]] double toDouble() const noexcept;

    /// "num/den", or just "num" for integers. The inverse of parse().
    [[nodiscard]] std::string toString() const;

    /// Parses "n" or "n/d" (decimal integers, optional leading '-', no spaces).
    /// Returns nullopt for malformed text, a zero denominator or out-of-range values.
    [[nodiscard]] static std::optional<Rational> parse(std::string_view text) noexcept;

    friend Rational operator+(Rational a, Rational b);
    friend Rational operator-(Rational a, Rational b);
    friend Rational operator*(Rational a, Rational b);
    friend Rational operator/(Rational a, Rational b);
    friend Rational operator-(Rational a);

    Rational& operator+=(Rational other) { return *this = *this + other; }
    Rational& operator-=(Rational other) { return *this = *this - other; }
    Rational& operator*=(Rational other) { return *this = *this * other; }
    Rational& operator/=(Rational other) { return *this = *this / other; }

    friend constexpr bool operator==(const Rational&, const Rational&) noexcept = default;
    friend std::strong_ordering operator<=>(const Rational& a, const Rational& b) noexcept;

private:
    constexpr void normalize() {
        constexpr auto kMin = std::numeric_limits<std::int64_t>::min();
        if (den_ == 0) {
            throw std::domain_error("Rational: zero denominator");
        }
        if (num_ == kMin || den_ == kMin) {
            throw std::overflow_error("Rational: value out of range");
        }
        if (den_ < 0) {
            num_ = -num_;
            den_ = -den_;
        }
        const std::int64_t divisor = std::gcd(num_, den_);
        num_ /= divisor;
        den_ /= divisor;
    }

    std::int64_t num_ = 0;
    std::int64_t den_ = 1;
};

} // namespace cutline::core
