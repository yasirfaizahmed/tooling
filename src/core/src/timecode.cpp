#include "cutline/core/timecode.h"

#include <stdexcept>

namespace cutline::core {

namespace {

void appendTwoDigits(std::string& out, std::int64_t value) {
    if (value < 10) {
        out += '0';
    }
    out += std::to_string(value);
}

} // namespace

std::int64_t timecodeBase(Rational frameRate) {
    if (!frameRate.isPositive()) {
        throw std::domain_error("frame rate must be positive");
    }
    return frameRate.ceil();
}

std::string formatTimecode(FrameIndex frame, Rational frameRate) {
    const std::int64_t base = timecodeBase(frameRate);
    std::string out;
    if (frame < 0) {
        out += '-';
        frame = -frame;
    }
    const std::int64_t totalSeconds = frame / base;
    appendTwoDigits(out, totalSeconds / 3600);
    out += ':';
    appendTwoDigits(out, totalSeconds / 60 % 60);
    out += ':';
    appendTwoDigits(out, totalSeconds % 60);
    out += ':';
    appendTwoDigits(out, frame % base);
    return out;
}

} // namespace cutline::core
