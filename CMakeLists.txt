cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "ChaosMod")

project("${PROJECT_NAME}" LANGUAGES C CXX)

message(STATUS "Detected compiler: ${CMAKE_CXX_COMPILER}")

set(PROJECT_VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Only LLVM Clang and MSVC compilers is currently supported.")
endif()

if (NOT DEFINED ENV{DX9_SDK})
    message(FATAL_ERROR "ENV: DX9_SDK not defined. The DX9 SDK is required.")
endif()

set(CODE_DIR "${PROJECT_SOURCE_DIR}/ChaosMod")

set(SOURCE_PATH "${CODE_DIR}/Source" )
set(INCLUDE_PATH "${CODE_DIR}/Include")
set(SDK_PATH "${PROJECT_SOURCE_DIR}/vendor/flhook-sdk")
set(imgui "${PROJECT_SOURCE_DIR}/vendor/imgui")
set(imgui_fb "${PROJECT_SOURCE_DIR}/vendor/imgui-filebrowser")
set(reshade "${PROJECT_SOURCE_DIR}/vendor/reshade")

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

# DirectX SDK
target_include_directories(${PROJECT_NAME} PRIVATE $ENV{DX9_SDK}/Include)
target_link_directories("${PROJECT_NAME}" PRIVATE $ENV{DX9_SDK}/Lib/x86)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH}/PCH.hpp)

find_package(FFMPEG REQUIRED)
target_include_directories("${PROJECT_NAME}" PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories("${PROJECT_NAME}" PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries("${PROJECT_NAME}" PRIVATE ${FFMPEG_LIBRARIES})

find_package(glm CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE glm::glm)

find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")
target_include_directories("${PROJECT_NAME}" PRIVATE ${MINIAUDIO_INCLUDE_DIRS})

find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp")
target_include_directories("${PROJECT_NAME}" PRIVATE ${NEARGYE_SEMVER_INCLUDE_DIRS})

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE nlohmann_json::nlohmann_json)

find_path(REFL_CPP_INCLUDE_DIRS "refl.hpp")
target_include_directories("${PROJECT_NAME}" PRIVATE ${REFL_CPP_INCLUDE_DIRS})

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE magic_enum::magic_enum)

# FLCore

target_link_libraries(${PROJECT_NAME} PRIVATE "${SDK_PATH}/lib/FLCoreCommon.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${SDK_PATH}/lib/FLCoreDACom.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${SDK_PATH}/lib/FLCoreDALib.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${SDK_PATH}/lib/FLCoreFLServerEXE.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${SDK_PATH}/lib/FLCoreRemoteClient.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "${SDK_PATH}/lib/FLCoreServer.lib")

# Build Scripts

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE "Winmm.lib")

    if (DEFINED ENV{FL_CHAOS_MOD})
        message(STATUS "Adding post build step")
        set(OUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE_INIT}/*.dll)
        string(REPLACE "/" "\\" OUT_PATH "${OUT_PATH}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND copy /Y "${OUT_PATH}" "$ENV{FL_CHAOS_MOD}"
            COMMAND powershell ${PROJECT_SOURCE_DIR}/BinUtils/CopyChaosAssets.ps1 -NoLaunch true -NoXml true
        )
    else()
        message(STATUS "ENV: FL_CHAOS_MOD not defined. Not adding post build step.")
    endif()
else()
    message(STATUS "Not building on Windows. Not adding post build step.")
endif()