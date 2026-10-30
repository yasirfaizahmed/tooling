#include "cutline/core/time.h"

#include <stdexcept>

namespace cutline::core {

namespace {

void requirePositiveRate(Rational frameRate) {
    if (!frameRate.isPositive()) {
        throw std::domain_error("frame rate must be positive");
    }
}

} // namespace

RationalTime RationalTime::fromFrames(FrameIndex frame, Rational frameRate) {
    requirePositiveRate(frameRate);
    return RationalTime(Rational(frame) / frameRate);
}

FrameIndex RationalTime::frameAt(Rational frameRate) const {
    requirePositiveRate(frameRate);
    return (seconds_ * frameRate).floor();
}

FrameIndex RationalTime::nearestFrame(Rational frameRate) const {
    requirePositiveRate(frameRate);
    return (seconds_ * frameRate).round();
}

std::int64_t firstSampleOfFrame(FrameIndex frame, Rational frameRate, std::int64_t sampleRate) {
    requirePositiveRate(frameRate);
    if (sampleRate <= 0) {
        throw std::domain_error("sample rate must be positive");
    }
    return (Rational(frame) * Rational(sampleRate) / frameRate).floor();
}

std::int64_t sampleCountOfFrame(FrameIndex frame, Rational frameRate, std::int64_t sampleRate) {
    return firstSampleOfFrame(frame + 1, frameRate, sampleRate) -
           firstSampleOfFrame(frame, frameRate, sampleRate);
}

} // namespace cutline::core
