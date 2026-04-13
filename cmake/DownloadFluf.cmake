# Download FLUF latest stable release, if check should only fail if multiple CMake presets are running concurrently
if (NOT EXISTS ${FLUF_PATH})
    message(STATUS "Downloading FLUF Stable")
    file(DOWNLOAD https://codeberg.org/api/v1/repos/TheStarport/FLUF/releases ${CMAKE_CURRENT_SOURCE_DIR}/latestRelease.json)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/latestRelease.json FLUF_RELEASES_JSON)
    string(JSON FLUF_LATEST_RELEASE GET ${FLUF_RELEASES_JSON} 0 assets 0 browser_download_url)
    file(DOWNLOAD ${FLUF_LATEST_RELEASE} ${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz)

    message(STATUS "Extracting")
    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        execute_process(COMMAND bash -c "mkdir -p '${FLUF_PATH}';tar -xf '${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz' -C '${FLUF_PATH}'" OUTPUT_QUIET)
    else ()
        execute_process(COMMAND powershell -c "New-Item -Path '${FLUF_PATH}' -ItemType Directory;tar -xf '${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz' -C '${FLUF_PATH}'" OUTPUT_QUIET)
    endif ()

    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/latestRelease.json)
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/fluf.tar.xz)
endif ()