set(VER_MAJOR 2)
set(VER_MINOR 0)
set(VER_PATCH 0)
set(NAME "Chaos Mod")
set(DESCRIPTION "A module to make the game a chaotic experience that you'll never forget.")
set(CUR_PROJECT "ChaosMod")

find_package(miniaudio REQUIRED)
find_package(semver REQUIRED)
find_package(cppzmq REQUIRED)
find_package(ZeroMQ REQUIRED)

# Glob our source files and header files
begin_glob()

set(DXSDK "${VENDOR_PATH}/DXSDK")
set(FFMPEG_DIR "${VENDOR_PATH}/ffmpeg")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/metadata.rc.in" ${CMAKE_CURRENT_BINARY_DIR}/metadata.rc @ONLY)
add_library(${CUR_PROJECT} SHARED
        ${SOURCE_FILES}
        ${CMAKE_CURRENT_BINARY_DIR}/metadata.rc
)

# Dependencies
target_include_directories(${CUR_PROJECT} PRIVATE
        ${INCLUDE_PATH}
        ${FLUF_INCLUDE_PATH}
        ${FLUF_INCLUDE_PATH}/imgui
        ${FLUF_DEPENDENCIES_PATH}
        ${FLUF_DEPENDENCIES_PATH}/imcp
        ${FLUF_DEPENDENCIES_PATH}/libpqxx
        ${FLUF_DEPENDENCIES_PATH}/concurrentqueue/moodycamel
        ${FLUF_DEPENDENCIES_PATH}/magic_enum
        ${FLUF_DEPENDENCIES_PATH}/msgpack-c
        ${FLUF_DEPENDENCIES_PATH}/xbyak
        ${FLUF_DEPENDENCIES_PATH}/zstd
)

target_link_libraries(${CUR_PROJECT} PRIVATE
        yaml-cpp::yaml-cpp
        miniaudio::miniaudio
        semver::semver
        cppzmq
        libzmq-static
)

target_compile_definitions(${CUR_PROJECT} PRIVATE
        "FLUF_SDK"
)

add_dependencies(${CUR_PROJECT} reflectcpp)

target_link_libraries(${CUR_PROJECT} PRIVATE
        "${SDK_PATH}/lib/FLCoreCommon.lib"
        "${SDK_PATH}/lib/FLCoreServer.lib"
        "${SDK_PATH}/lib/FLCoreDALib.lib"
        "user32.lib"
        "Winmm.lib"
)

set_general_targets(PROJECT_NAME ${CUR_PROJECT})

reset_glob()
