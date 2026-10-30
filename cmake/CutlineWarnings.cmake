# cutline_set_warnings(<target> [STRICT])
#
# Applies the project warning set to a target. STRICT adds implicit-conversion warnings and is
# used for the pure core library, where silent integer narrowing would corrupt time values.
# CUTLINE_WARNINGS_AS_ERRORS (ON in CI) turns every warning into an error.
function(cutline_set_warnings target)
    cmake_parse_arguments(ARG "STRICT" "" "" ${ARGN})

    if(MSVC)
        set(flags /W4 /permissive-)
        if(CUTLINE_WARNINGS_AS_ERRORS)
            list(APPEND flags /WX)
        endif()
    else()
        set(flags -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual)
        if(ARG_STRICT)
            list(APPEND flags -Wconversion -Wsign-conversion -Wold-style-cast)
        endif()
        if(CUTLINE_WARNINGS_AS_ERRORS)
            list(APPEND flags -Werror)
        endif()
    endif()

    target_compile_options(${target} PRIVATE ${flags})
endfunction()
