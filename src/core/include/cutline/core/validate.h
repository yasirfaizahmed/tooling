#pragma once

#include "cutline/core/timeline.h"

#include <string>
#include <vector>

namespace cutline::core {

/// Checks every structural invariant of a timeline and describes each violation in plain
/// language. An empty result means the timeline is valid. Every edit operation must keep a
/// valid timeline valid; the tests call this after each one.
///
/// Media-dependent checks (e.g. a clip reading past the end of its source) need the media pool
/// and live in the project-level validation.
[[nodiscard]] std::vector<std::string> validate(const Timeline& timeline);

} // namespace cutline::core
