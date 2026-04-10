set(_ROOT "${FLUF_DEPENDENCIES_PATH}/msgpack-c")
set(_LIB_DIR "${_ROOT}/lib")

if (EXISTS "${_ROOT}" AND EXISTS "${_LIB_DIR}")
    if (NOT TARGET msgpack-c)
        add_library(msgpack-c UNKNOWN IMPORTED)
    endif ()

    set(_lib_candidates "${_LIB_DIR}/msgpack-c.lib")
    set(_imported_location "")
    foreach (_c IN LISTS _lib_candidates)
        if (EXISTS "${_c}")
            set(_imported_location "${_c}")
            break()
        endif ()
    endforeach ()

    if (_imported_location)
        set_target_properties(msgpack-c PROPERTIES
                IMPORTED_LOCATION "${_imported_location}"
                INTERFACE_INCLUDE_DIRECTORIES "${_ROOT}"
                IMPORTED_GLOBAL TRUE
        )
    else ()
        set_target_properties(msgpack-c PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${_ROOT}"
                IMPORTED_GLOBAL TRUE
        )
    endif ()

    set(msgpack-c_INCLUDE_DIRS "${_ROOT}" PATH "msgpack-c include directories")
    set(msgpack-c_INCLUDE_DIR "${_ROOT}" PATH "msgpack-c include directory")
    if (_imported_location)
        set(msgpack-c_LIBRARIES "${_imported_location}" FILEPATH "msgpack-c library")
    else ()
        set(msgpack-c_LIBRARIES "" STRING "msgpack-c library (none found)")
    endif ()
    set(msgpack-c_FOUND TRUE BOOL "msgpack-c found")
else ()
    set(msgpack-c_FOUND FALSE BOOL "msgpack-c found")
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(msgpack-c
        REQUIRED_VARS msgpack-c_FOUND
        FOUND_VAR msgpack-c_FOUND
)
