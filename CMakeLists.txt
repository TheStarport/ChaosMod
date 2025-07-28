cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "ChaosMod")

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    message(STATUS "Found CCache, applying rules")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif ()


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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"SAFESEH:NO\"")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

project("${PROJECT_NAME}" LANGUAGES C CXX)

message(STATUS "Detected compiler: ${CMAKE_CXX_COMPILER}")

set(PROJECT_VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)

# Load config
include("LoadConf.cmake")

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Only LLVM Clang and MSVC compilers is currently supported.")
endif ()

set(VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor)
if (NOT IS_DIRECTORY ${VENDOR_PATH}/DXSDK)
    message(FATAL_ERROR "DXSDK Not found. Did you run 'python cli.py requirements'?")
endif ()

set(CODE_DIR "${PROJECT_SOURCE_DIR}/ChaosMod")

set(SOURCE_PATH "${CODE_DIR}/Source")
set(INCLUDE_PATH "${CODE_DIR}/Include")
set(SDK_PATH "${VENDOR_PATH}/flhook-sdk")
set(imgui "${VENDOR_PATH}/imgui")
set(imgui_fb "${VENDOR_PATH}/imgui-filebrowser")
set(reshade "${VENDOR_PATH}/reshade")
set(DISCORD_GAME_SDK "${VENDOR_PATH}/discord_game_sdk")
set(DXSDK "${VENDOR_PATH}/DXSDK")
set(FFMPEG_DIR "${VENDOR_PATH}/ffmpeg")

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false
        "${SOURCE_PATH}/*.c*"
        "${SOURCE_PATH}/*.h*"
        "${INCLUDE_PATH}/*.c*"
        "${INCLUDE_PATH}/*.h*"
        "${PROJECT_SOURCE_DIR}/resource.h"
        "${PROJECT_SOURCE_DIR}/Resources.rc"
        "${SDK_PATH}/*.h*"
        "${imgui}/*.h"
        "${imgui}/*.cpp"
        "${imgui_fb}/*.h"
        "${reshade}/*.hpp"
        "${DISCORD_GAME_SDK}/cpp/*.h"
        "${DISCORD_GAME_SDK}/cpp/*.cpp"
)

# Disable PCH for C files
set_source_files_properties(${SOURCE_PATH}/miniaudio.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

add_library("${PROJECT_NAME}" SHARED ${SOURCE_FILES} ${INCLUDE_PATH}/PCH.hpp ${INCLUDE_PATH}/PCH.cpp)
set_target_properties("${PROJECT_NAME}" PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING;"
        "_CRT_SECURE_NO_WARNINGS;"
        "WIN32;"
        "UNICODE;_UNICODE;"
        "NOMINMAX;"
        "IMGUI_DEFINE_MATH_OPERATORS;"
        "ImTextureID=ImU64;"
        "USE_GLM;"
)

# Conditionally add debug flags
add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDK_PATH}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDK_PATH}/vendor)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH}/External/uvw)
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui})
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_fb})
target_include_directories(${PROJECT_NAME} PRIVATE ${reshade})

# Discord Game SDK
target_include_directories(${PROJECT_NAME} PRIVATE ${DISCORD_GAME_SDK}/cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${DISCORD_GAME_SDK}/lib/x86/discord_game_sdk.dll.lib)

# DirectX SDK
target_include_directories(${PROJECT_NAME} PRIVATE ${DXSDK}/Include)
target_link_directories("${PROJECT_NAME}" PRIVATE ${DXSDK}/Lib/x86)

# FFMPEG
target_include_directories("${PROJECT_NAME}" PRIVATE ${FFMPEG_DIR}/include)
target_link_libraries("${PROJECT_NAME}" PRIVATE
        "${FFMPEG_DIR}/lib/avcodec.lib"
        "${FFMPEG_DIR}/lib/avdevice.lib"
        "${FFMPEG_DIR}/lib/avfilter.lib"
        "${FFMPEG_DIR}/lib/avformat.lib"
        "${FFMPEG_DIR}/lib/avutil.lib"
        "${FFMPEG_DIR}/lib/avformat.lib"
        "${FFMPEG_DIR}/lib/postproc.lib"
        "${FFMPEG_DIR}/lib/swresample.lib"
        "${FFMPEG_DIR}/lib/swscale.lib"
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH}/PCH.hpp)

# Import Conan Packages
find_package(glm)
find_package(magic_enum)
find_package(miniaudio)
find_package(semver)
find_package(nlohmann_json)
find_package(refl-cpp)
find_package(xbyak)
find_package(cppzmq)
find_package(concurrentqueue)
target_link_libraries(${PROJECT_NAME} PRIVATE
        glm::glm
        magic_enum::magic_enum
        miniaudio::miniaudio
        semver::semver
        nlohmann_json::nlohmann_json
        refl-cpp::refl-cpp
        xbyak::xbyak
        cppzmq
        concurrentqueue::concurrentqueue
)

# FLCore

target_link_libraries(${PROJECT_NAME} PRIVATE
        "${SDK_PATH}/lib/FLCoreCommon.lib"
        "${SDK_PATH}/lib/FLCoreDACom.lib"
        "${SDK_PATH}/lib/FLCoreDALib.lib"
        "${SDK_PATH}/lib/FLCoreFLServerEXE.lib"
        "${SDK_PATH}/lib/FLCoreRemoteClient.lib"
        "${SDK_PATH}/lib/FLCoreServer.lib"
        "Winmm.lib")

# Copy Discord SDK
file(COPY "${VENDOR_PATH}/discord_game_sdk/lib/x86/discord_game_sdk.dll"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# If we are building on Linux, import the MSVC headers
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if (NOT DEFINED ENV{MSVC})
        message(FATAL_ERROR "MSVC was not found and running on Linux")
    endif ()
    string(REPLACE ":" " " MSVC_INCLUDES $ENV{MSVC})
    separate_arguments(MSVC_INCLUDES)
    target_include_directories(${PROJECT_NAME} PRIVATE ${MSVC_INCLUDES})
endif ()

# Build Scripts

find_package(Python COMPONENTS Interpreter )
if (PYTHON_FOUND AND DEFINED BINARY_COPY_DESTINATION)
    set(OUT_PATH ${CMAKE_BINARY_DIR}/*.dll)
    string(REPLACE "/" "\\" OUT_PATH "${OUT_PATH}")
    file(TO_NATIVE_PATH ${VENDOR_PATH} VENDOR_PATH)
    message(STATUS "Adding post build step")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${PYTHON_EXECUTABLE} copy_assets.py ${BINARY_COPY_DESTINATION} "$<IF:$<CONFIG:Release>,release,debug>"
    )
endif ()
