#pragma once

#include "cutline/core/ids.h"
#include "cutline/core/time.h"

namespace cutline::core {

enum class TransitionKind { CrossDissolve, DipToBlack };

/// Where a transition sits relative to the cut it belongs to.
enum class TransitionAlignment { CenteredOnCut, StartsAtCut, EndsAtCut };

/// A transition across the cut between two adjacent clips on one track. It overlaps the
/// clips' source media beyond the cut ("handles"), so the timeline length does not change.
struct Transition {
    TransitionId id;
    TrackId track;
    ClipId left;  ///< Clip ending at the cut.
    ClipId right; ///< Clip starting at the cut.
    FrameIndex length = 0;
    TransitionAlignment alignment = TransitionAlignment::CenteredOnCut;
    TransitionKind kind = TransitionKind::CrossDissolve;

    friend bool operator==(const Transition&, const Transition&) = default;
};

} // namespace cutline::core
