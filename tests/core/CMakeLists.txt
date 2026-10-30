find_package(Catch2 3 REQUIRED)
include(Catch)

add_executable(test_core
    test_ids.cpp
    test_rational.cpp
    test_time.cpp
    test_timecode.cpp
    test_validate.cpp
)
target_link_libraries(test_core PRIVATE Cutline::core Catch2::Catch2WithMain)
cutline_set_warnings(test_core)

catch_discover_tests(test_core TEST_PREFIX "core: " DISCOVERY_MODE PRE_TEST)
