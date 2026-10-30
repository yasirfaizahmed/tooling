cmake_minimum_required(VERSION 3.25)

project(Cutline
    VERSION 0.1.0
    DESCRIPTION "Cross-platform non-linear video, audio and image editor"
    HOMEPAGE_URL "https://github.com/yasirfaizahmed/tooling"
    LANGUAGES CXX)

option(CUTLINE_BUILD_APP "Build the Qt desktop application" ON)
option(CUTLINE_BUILD_TESTS "Build unit and smoke tests" ON)
option(CUTLINE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(CutlineWarnings)

# Source files are UTF-8 everywhere; MSVC needs to be told.
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

if(CUTLINE_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(src/core)

if(CUTLINE_BUILD_APP)
    add_subdirectory(src/app)
endif()

if(CUTLINE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
