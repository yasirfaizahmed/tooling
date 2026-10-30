find_package(Qt6 REQUIRED COMPONENTS Test Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(test_app_main_window test_main_window.cpp)
target_link_libraries(test_app_main_window PRIVATE cutline_app_lib Qt6::Test)
cutline_set_warnings(test_app_main_window)

# Runs headless everywhere, including CI machines without a display.
add_test(NAME "app: main window" COMMAND test_app_main_window)
set_tests_properties("app: main window" PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
