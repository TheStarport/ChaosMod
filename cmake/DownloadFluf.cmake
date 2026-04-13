# Download FLUF latest stable release, if check should only fail if multiple CMake presets are running concurrently
if (NOT EXISTS ${FLUF_PATH})
    message(STATUS "Downloading FLUF Stable")
    file(DOWNLOAD https://codeberg.org/api/v1/repos/TheStarport/FLUF/releases ${CMAKE_CURRENT_SOURCE_DIR}/latestRelease.json)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/latestRelease.json FLUF_RELEASES_JSON)

    string(JSON ASSETS_JSON GET ${FLUF_RELEASES_JSON} 0 assets)
    string(JSON ASSETS_COUNT LENGTH ${ASSETS_JSON})
    set(WANTED_NAME "FLUF.tar.xz")

    math(EXPR LAST_INDEX "${ASSETS_COUNT} - 1")
    foreach(i RANGE 0 ${LAST_INDEX})
        string(JSON asset_name GET ${FLUF_RELEASES_JSON} 0 assets ${i} name)
        if(asset_name STREQUAL "${WANTED_NAME}")
            string(JSON TARGET_DOWNLOAD_URL GET ${FLUF_RELEASES_JSON} 0 assets ${i} browser_download_url)
            break()
        endif()
    endforeach()

    if(NOT TARGET_DOWNLOAD_URL)
        message(FATAL_ERROR "No matching asset named ${WANTED_NAME} found")
    endif()

    file(DOWNLOAD ${TARGET_DOWNLOAD_URL} ${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz SHOW_PROGRESS)

    message(STATUS "Extracting")
    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        execute_process(COMMAND bash -c "mkdir -p '${FLUF_PATH}';tar -xf '${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz' -C '${FLUF_PATH}'" OUTPUT_QUIET)
    else ()
        execute_process(COMMAND powershell -c "New-Item -Path '${FLUF_PATH}' -ItemType Directory;tar -xf '${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz' -C '${FLUF_PATH}'" OUTPUT_QUIET)
    endif ()

    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/latestRelease.json)
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz)
endif ()