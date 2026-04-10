# Extra work needed to make Linux under MSVC-Wine work properly.
# It'll compile just fine without this, but CLion & VSCode
# will both throw a massive fit while trying to work out proper include paths
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions("WINAPI_FAMILY=100")
    if (NOT DEFINED ENV{MSVC})
        message(FATAL_ERROR "MSVC env was not found and running on Linux")
    endif ()

    if (NOT DEFINED ENV{INCLUDE})
        message(FATAL_ERROR "'INCLUDE' env not found. Have you run 'source $ENV{MSVC}/bin/x86/msvcenv.sh' ?")
    endif ()

    string(REPLACE "z:\\" "/" MSVC_INCLUDE "$ENV{INCLUDE}")
    string(REPLACE "\\" "/" MSVC_INCLUDE "${MSVC_INCLUDE}")
    string(REPLACE ";" " " MSVC_INCLUDE "${MSVC_INCLUDE}")
    separate_arguments(MSVC_INCLUDE)
    include_directories(${MSVC_INCLUDE})
endif ()

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    message(STATUS "Found CCache, applying rules")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif ()