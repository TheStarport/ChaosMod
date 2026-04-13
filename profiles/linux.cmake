list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
message(STATUS ${CMAKE_CURRENT_LIST_DIR})
include("clang")

# This is hardcoded in generate_overlay.py
set(clangcl_linux_vfs_root_dir "/winsys")

set(clangcl_linux_triple "i686-windows-msvc")
set(clangcl_linux_msvc_dir "${clangcl_linux_vfs_root_dir}/msvc/${CLANGCL_LINUX_MSVC_VERSION}")
set(clangcl_linux_sdk_dir "${clangcl_linux_vfs_root_dir}/sdk")

# Compiler flags (uses ' ' as separator)
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} /vctoolsdir ${clangcl_linux_msvc_dir}")
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} /vctoolsversion ${CLANGCL_LINUX_MSVC_VERSION}")
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} /winsdkdir ${clangcl_linux_sdk_dir}")
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} /winsdkversion ${CLANGCL_LINUX_SDK_VERSION}")
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} -vfsoverlay ${CLANGCL_LINUX_VFS}")
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} /EHa") # Enable C++ exceptions
set(clangcl_linux_compiler_flags "${clangcl_linux_compiler_flags} -gdwarf") # Generate DWARF debug info instead of PDB for Wine + LLDB debugging

# Linker flags (uses ':' as separator)
set(clangcl_linux_linker_flags "${clangcl_linux_linker_flags} /vctoolsdir:${clangcl_linux_msvc_dir}")
set(clangcl_linux_linker_flags "${clangcl_linux_linker_flags} /vctoolsversion:${CLANGCL_LINUX_MSVC_VERSION}")
set(clangcl_linux_linker_flags "${clangcl_linux_linker_flags} /winsdkdir:${clangcl_linux_sdk_dir}")
set(clangcl_linux_linker_flags "${clangcl_linux_linker_flags} /winsdkversion:${CLANGCL_LINUX_SDK_VERSION}")
set(clangcl_linux_linker_flags "${clangcl_linux_linker_flags} /vfsoverlay:${CLANGCL_LINUX_VFS}")

if(CLANGCL_LINUX_VERBOSE)
    set(clangcl_linux_linker_flags "${clangcl_linux_linker_flags} /verbose /verbose:lib")
endif()

set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CMAKE_C_COMPILER "clang-cl")
set(CMAKE_CXX_COMPILER "clang-cl")

set(CMAKE_C_COMPILER_TARGET ${clangcl_linux_triple})
set(CMAKE_CXX_COMPILER_TARGET ${clangcl_linux_triple})

set(CMAKE_LINKER_TYPE "LLD")

set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} ${clangcl_linux_compiler_flags}")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} ${clangcl_linux_compiler_flags}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} ${clangcl_linux_linker_flags}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${CMAKE_SHARED_LINKER_FLAGS_INIT} ${clangcl_linux_linker_flags}")

if(CLANGCL_LINUX_INCLUDE_RC)
    set(CMAKE_RC_FLAGS_INIT "${CMAKE_RC_FLAGS_INIT} /I${CLANGCL_LINUX_INCLUDE_RC}")
endif()

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
    include_directories(SYSTEM ${MSVC_INCLUDE})
endif ()