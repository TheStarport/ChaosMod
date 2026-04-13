cmake_minimum_required(VERSION 3.23)

project("ChaosMod" LANGUAGES C CXX VERSION 1.0.0)
message(STATUS "Detected compiler: ${CMAKE_CXX_COMPILER}")
add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# Disabled warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -Wno-ignored-attributes \
    -Wno-microsoft-cast \
    -Wno-explicit-specialization-storage-class \
    -Wno-comment")

# Paths
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(VENDOR_PATH "${PROJECT_SOURCE_DIR}/vendor")
set(SDK_PATH "${VENDOR_PATH}/FLSDK")
set(REFLECT_PATH "${VENDOR_PATH}/reflect-cpp")
set(FLUF_PATH "${VENDOR_PATH}/FLUF")
set(FLUF_DEPENDENCIES_PATH "${FLUF_PATH}/development/dependencies")
set(FLUF_INCLUDE_PATH "${FLUF_PATH}/development/include")
set(FLUF_LIB_PATH "${FLUF_PATH}/development/lib")

find_package(yaml-cpp 0.9.0 REQUIRED)

include("cmake/Linux.cmake") # Extra steps needed for a good Linux UX
include("cmake/DownloadFluf.cmake")
include("cmake/LoadConf.cmake")
include("cmake/ModuleFunctions.cmake")

add_compile_definitions("YAML_CPP_STATIC_DEFINE")

set(REFLECTCPP_MSGPACK ON)
set(REFLECTCPP_YAML ON)
set(REFLECTCPP_BUILD_SHARED OFF)
add_subdirectory(${REFLECT_PATH})
add_subdirectory("ChaosMod")