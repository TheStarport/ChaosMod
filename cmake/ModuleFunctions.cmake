function(set_general_targets)
    cmake_parse_arguments(
            PARSED_ARGS # prefix of output variables
            ""
            "PROJECT_NAME" # list of names of mono-valued arguments
            ""
            ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if (NOT PARSED_ARGS_PROJECT_NAME)
        message(FATAL_ERROR "set_general_targets called without specifying a project name")
    endif ()

    target_include_directories(${PARSED_ARGS_PROJECT_NAME} PRIVATE
            ${ROOT_DIR}
            ${ROOT_DIR}/modules
            ${SDK_PATH}/include
            ${REFLECT_PATH}/include
            ${REFLECT_PATH}/include/rfl/thirdparty
            ${FLUF_INCLUDE_PATH}
            ${FLUF_INCLUDE_PATH}/FLUF
            ${FLUF_DEPENDENCIES_PATH}/FLUF
            ${VENDOR_PATH}
    )

    target_compile_definitions(${PARSED_ARGS_PROJECT_NAME} PRIVATE
            "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING;"
            "_CRT_SECURE_NO_WARNINGS;"
            "WIN32;"
            "WIN32_LEAN_AND_MEAN;"
            "UNICODE;_UNICODE;"
            "NOMINMAX;"
            "IMGUI_DEFINE_MATH_OPERATORS;"
            "IMGUI_ENABLE_FREETYPE;"
            "IMGUI_USE_STB_SPRINTF;"
            "IMGUI_USE_WCHAR32;"
    )

    add_dependencies(${PARSED_ARGS_PROJECT_NAME} reflectcpp)

    target_link_libraries(${PARSED_ARGS_PROJECT_NAME} PRIVATE
            ${FLUF_LIB_PATH}/FLUF.lib
            ${FLUF_LIB_PATH}/FreelancerSDK.lib
            msgpack-c
            yaml-cpp::yaml-cpp
            reflectcpp
    )

    set_target_properties(${PARSED_ARGS_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

    target_link_options(${PARSED_ARGS_PROJECT_NAME} PRIVATE
            /INCREMENTAL:NO
            /OPT:ICF
            /OPT:REF
    )
endfunction()

function(begin_glob)
    set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Source)
    set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Include)
    file(GLOB_RECURSE TMP_FILES LIST_DIRECTORIES false
            "${SOURCE_PATH}/*.c*"
            "${SOURCE_PATH}/*.h*"
            "${INCLUDE_PATH}/*.c*"
            "${INCLUDE_PATH}/*.h*"
            "${SDK_PATH}/*.h*"
    )
    set(SOURCE_PATH ${SOURCE_PATH} PARENT_SCOPE)
    set(INCLUDE_PATH ${INCLUDE_PATH} PARENT_SCOPE)
    set(SOURCE_FILES ${TMP_FILES} PARENT_SCOPE)
endfunction()

function(reset_glob)
    unset(SOURCE_PATH PARENT_SCOPE)
    unset(INCLUDE_PATH PARENT_SCOPE)
    unset(SOURCE_FILES PARENT_SCOPE)
endfunction()