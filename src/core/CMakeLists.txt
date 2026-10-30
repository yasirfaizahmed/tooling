# cutline_core: the timeline data model and edit operations.
# Pure C++20 with no Qt, MLT or FFmpeg dependency, so all of it is unit-testable.
add_library(cutline_core
    include/cutline/core/clip.h
    include/cutline/core/ids.h
    include/cutline/core/media.h
    include/cutline/core/param.h
    include/cutline/core/rational.h
    include/cutline/core/time.h
    include/cutline/core/timecode.h
    include/cutline/core/timeline.h
    include/cutline/core/track.h
    include/cutline/core/transition.h
    include/cutline/core/validate.h
    src/checked_math.h
    src/rational.cpp
    src/time.cpp
    src/timecode.cpp
    src/validate.cpp
)
add_library(Cutline::core ALIAS cutline_core)

target_include_directories(cutline_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(cutline_core PUBLIC cxx_std_20)
cutline_set_warnings(cutline_core STRICT)
