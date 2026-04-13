set(_ROOT "${FLUF_DEPENDENCIES_PATH}/yaml-cpp")
set(_LIB_DIR "${_ROOT}/lib")

# -- Provide an imported target if the expected layout exists -----------------
if (EXISTS "${_ROOT}" AND EXISTS "${_LIB_DIR}")
    # Create imported target
    if (NOT TARGET yaml-cpp::yaml-cpp)
        add_library(yaml-cpp::yaml-cpp UNKNOWN IMPORTED)
    endif ()

    set(_lib_candidates "${_LIB_DIR}/yaml-cpp.lib")

    # Pick the first existing candidate for IMPORTED_LOCATION
    set(_imported_location "")
    foreach (_c IN LISTS _lib_candidates)
        if (EXISTS "${_c}")
            set(_imported_location "${_c}")
            break()
        endif ()
    endforeach ()

    if (_imported_location)
        set_target_properties(yaml-cpp::yaml-cpp PROPERTIES
                IMPORTED_LOCATION "${_imported_location}"
                INTERFACE_INCLUDE_DIRECTORIES "${_ROOT}"
                IMPORTED_GLOBAL TRUE
        )
    else ()
        set_target_properties(yaml-cpp::yaml-cpp PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${_ROOT}"
                IMPORTED_GLOBAL TRUE
        )
    endif ()

    set(yaml-cpp_VERSION_STRING "0.8.0" STRING "yaml-cpp version")
    set(yaml-cpp_INCLUDE_DIRS "${_ROOT}" PATH "yaml-cpp include directories")
    set(yaml-cpp_INCLUDE_DIR "${_ROOT}" PATH "yaml-cpp include directory")
    if (_imported_location)
        set(yaml-cpp_LIBRARIES "${_imported_location}" FILEPATH "yaml-cpp library")
    else ()
        set(yaml-cpp_LIBRARIES "" STRING "yaml-cpp library (none found)")
    endif ()
    set(yaml-cpp_FOUND TRUE BOOL "yaml-cpp found")
else ()
    set(yaml-cpp_FOUND FALSE BOOL "yaml-cpp found")
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yaml-cpp
        REQUIRED_VARS yaml-cpp_FOUND
        VERSION_VAR yaml-cpp_VERSION_STRING
        FOUND_VAR yaml-cpp_FOUND
)
