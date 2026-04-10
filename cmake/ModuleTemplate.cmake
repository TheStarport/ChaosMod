set(VER_MAJOR 1)
set(VER_MINOR 0)
set(VER_PATCH 0)
set(NAME "Module Name")
set(DESCRIPTION "short description of the module")
set(CUR_PROJECT "dll_name")

# Glob our source files and header files
begin_glob()

configure_file(${FLUF_PATH}/development/tools/metadata.rc.in ${CMAKE_CURRENT_BINARY_DIR}/metadata.rc @ONLY)
add_library(${CUR_PROJECT} SHARED
        ${FLUF_INCLUDE_PATH}/PCH.cpp
        ${FLUF_INCLUDE_PATH}/PCH.hpp
        ${SOURCE_FILES}
        ${CMAKE_CURRENT_BINARY_DIR}/metadata.rc
)

# Dependencies
target_include_directories(${CUR_PROJECT} PRIVATE
        ${INCLUDE_PATH}
)

# Link against FLCore
target_link_libraries(${CUR_PROJECT} PRIVATE "${SDK_PATH}/lib/FLCoreCommon.lib")
set_general_targets(PROJECT_NAME ${CUR_PROJECT})

reset_glob()
