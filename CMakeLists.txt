cmake_minimum_required(VERSION 3.23)

project("ChaosMod" LANGUAGES C CXX VERSION 1.0.0)
message(STATUS "Detected compiler: ${CMAKE_CXX_COMPILER}")
add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")

# Needed flags for Freelancer exception handling
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D /SAFESEH:NO")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

# Paths
set(ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(VENDOR_PATH "${PROJECT_SOURCE_DIR}/vendor")
set(SDK_PATH "${VENDOR_PATH}/FLSDK")
set(REFLECT_PATH "${VENDOR_PATH}/reflect-cpp")
set(FLUF_PATH "${VENDOR_PATH}/FLUF/development")
set(FLUF_DEPENDENCIES_PATH "${FLUF_PATH}/dependencies")
set(FLUF_INCLUDE_PATH "${FLUF_PATH}/include")
set(FLUF_LIB_PATH "${FLUF_PATH}/lib")

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