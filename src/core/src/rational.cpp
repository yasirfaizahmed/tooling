#include "cutline/core/rational.h"

#include "checked_math.h"

#include <charconv>

namespace cutline::core {

using detail::checkedAdd;
using detail::checkedMul;
using detail::floorDivMod;

std::int64_t Rational::floor() const noexcept {
    return floorDivMod(num_, den_).quotient;
}

std::int64_t Rational::ceil() const noexcept {
    const auto [quotient, remainder] = floorDivMod(num_, den_);
    return remainder == 0 ? quotient : quotient + 1;
}

std::int64_t Rational::round() const noexcept {
    const auto [quotient, remainder] = floorDivMod(num_, den_);
    // remainder/den >= 1/2  <=>  2 * remainder >= den. Unsigned maths cannot overflow here
    // because remainder < den <= 2^63 - 1.
    const bool roundUp =
        2 * static_cast<std::uint64_t>(remainder) >= static_cast<std::uint64_t>(den_);
    return roundUp ? quotient + 1 : quotient;
}

Rational Rational::reciprocal() const {
    if (num_ == 0) {
        throw std::domain_error("Rational: reciprocal of zero");
    }
    return Rational(den_, num_);
}

double Rational::toDouble() const noexcept {
    return static_cast<double>(num_) / static_cast<double>(den_);
}

std::string Rational::toString() const {
    if (den_ == 1) {
        return std::to_string(num_);
    }
    return std::to_string(num_) + "/" + std::to_string(den_);
}

namespace {

std::optional<std::int64_t> parseInteger(std::string_view text) noexcept {
    if (text.empty()) {
        return std::nullopt;
    }
    std::int64_t value = 0;
    const char* last = text.data() + text.size();
    const auto [end, error] = std::from_chars(text.data(), last, value);
    if (error != std::errc{} || end != last) {
        return std::nullopt;
    }
    return value;
}

} // namespace

std::optional<Rational> Rational::parse(std::string_view text) noexcept {
    const std::size_t slash = text.find('/');
    const auto num = parseInteger(text.substr(0, slash));
    if (!num) {
        return std::nullopt;
    }
    std::int64_t den = 1;
    if (slash != std::string_view::npos) {
        const std::string_view denText = text.substr(slash + 1);
        // The sign belongs on the numerator only.
        if (!denText.empty() && denText.front() == '-') {
            return std::nullopt;
        }
        const auto parsedDen = parseInteger(denText);
        if (!parsedDen || *parsedDen == 0) {
            return std::nullopt;
        }
        den = *parsedDen;
    }
    try {
        return Rational(*num, den);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

Rational operator+(Rational a, Rational b) {
    // Scale both fractions to the least common denominator only, to keep intermediates small.
    const std::int64_t divisor = std::gcd(a.den_, b.den_);
    const std::int64_t aScale = b.den_ / divisor;
    const std::int64_t bScale = a.den_ / divisor;
    const std::int64_t num = checkedAdd(checkedMul(a.num_, aScale), checkedMul(b.num_, bScale));
    return Rational(num, checkedMul(a.den_, aScale));
}

Rational operator-(Rational a) {
    return Rational(-a.num_, a.den_);
}

Rational operator-(Rational a, Rational b) {
    return a + (-b);
}

Rational operator*(Rational a, Rational b) {
    // Cross-cancel before multiplying. Denominators are positive, so both divisors are >= 1.
    const std::int64_t aNumBDen = std::gcd(a.num_, b.den_);
    const std::int64_t bNumADen = std::gcd(b.num_, a.den_);
    const std::int64_t num = checkedMul(a.num_ / aNumBDen, b.num_ / bNumADen);
    const std::int64_t den = checkedMul(a.den_ / bNumADen, b.den_ / aNumBDen);
    return Rational(num, den);
}

Rational operator/(Rational a, Rational b) {
    return a * b.reciprocal();
}

std::strong_ordering operator<=>(const Rational& a, const Rational& b) noexcept {
    // Compares a.num/a.den with b.num/b.den exactly, without cross-multiplying (which could
    // overflow). Equal integer parts reduce the problem to comparing the fractional parts,
    // which is the reversed comparison of their reciprocals: a continued-fraction expansion
    // that terminates like Euclid's algorithm.
    std::int64_t lhsNum = a.num_;
    std::int64_t lhsDen = a.den_;
    std::int64_t rhsNum = b.num_;
    std::int64_t rhsDen = b.den_;
    bool reversed = false;
    for (;;) {
        const auto lhs = floorDivMod(lhsNum, lhsDen);
        const auto rhs = floorDivMod(rhsNum, rhsDen);
        std::strong_ordering order = lhs.quotient <=> rhs.quotient;
        if (order == std::strong_ordering::equal) {
            if (lhs.remainder == 0 || rhs.remainder == 0) {
                order = (lhs.remainder != 0) <=> (rhs.remainder != 0);
            } else {
                lhsNum = lhsDen;
                lhsDen = lhs.remainder;
                rhsNum = rhsDen;
                rhsDen = rhs.remainder;
                reversed = !reversed;
                continue;
            }
        }
        return reversed ? 0 <=> order : order;
    }
}

} // namespace cutline::core
