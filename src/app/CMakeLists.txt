find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

configure_file(app_info.h.in "${CMAKE_CURRENT_BINARY_DIR}/generated/app_info.h" @ONLY)

# Everything except main() lives in this library, so tests can build the real windows.
add_library(cutline_app_lib STATIC
    main_window.cpp
    main_window.h
    panels/empty_state.cpp
    panels/empty_state.h
    panels/panel_title_bar.cpp
    panels/panel_title_bar.h
    panels/inspector/inspector_panel.cpp
    panels/inspector/inspector_panel.h
    panels/media_bin/media_bin_panel.cpp
    panels/media_bin/media_bin_panel.h
    panels/preview/preview_panel.cpp
    panels/preview/preview_panel.h
    panels/timeline/timeline_panel.cpp
    panels/timeline/timeline_panel.h
    theme/theme.cpp
    theme/theme.h
)
qt_add_resources(cutline_app_lib "cutline_theme"
    PREFIX "/cutline"
    FILES theme/style.qss
)
target_include_directories(cutline_app_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)
target_link_libraries(cutline_app_lib PUBLIC Cutline::core Qt6::Widgets)
cutline_set_warnings(cutline_app_lib)

qt_add_executable(cutline WIN32 MACOSX_BUNDLE main.cpp)
target_link_libraries(cutline PRIVATE cutline_app_lib)
cutline_set_warnings(cutline)

set_target_properties(cutline PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "Cutline"
    MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.yasirfaizahmed.cutline"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
)
if(APPLE OR WIN32)
    set_target_properties(cutline PROPERTIES OUTPUT_NAME "Cutline")
else()
    # The installed binary finds the bundled Qt libraries next to it.
    set_target_properties(cutline PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

install(TARGETS cutline
    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

# `cmake --install` produces a self-contained app: Qt's deploy script runs windeployqt or
# macdeployqt, or copies the Qt libraries and plugins on Linux (Qt 6.5+).
if(Qt6_VERSION VERSION_GREATER_EQUAL 6.5)
    qt_generate_deploy_app_script(
        TARGET cutline
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT "${deploy_script}")
endif()

if(WIN32)
    # Ship the MSVC runtime DLLs so users do not need the Visual C++ redistributable.
    include(InstallRequiredSystemLibraries)
endif()
