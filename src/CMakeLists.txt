project (sibuserv-webui)
cmake_minimum_required (VERSION 3.4)

find_package (Qt5Core 5.4 REQUIRED)

list (APPEND PROJECT_DIRS ${PROJECT_SOURCE_DIR}
                          ${PROJECT_SOURCE_DIR}/basic
                          ${PROJECT_SOURCE_DIR}/items
                          ${PROJECT_SOURCE_DIR}/pages
                          ${PROJECT_SOURCE_DIR}/settings
                          ${PROJECT_SOURCE_DIR}/tools)

foreach (DIR ${PROJECT_DIRS})
    aux_source_directory (${DIR} FILES)
    list (APPEND SRC_FILES ${FILES})
endforeach(DIR)

list (APPEND QRC_FILES ${PROJECT_SOURCE_DIR}/resources.qrc)
qt5_add_resources(RESOURCES ${QRC_FILES})

add_executable (${PROJECT_NAME} ${SRC_FILES} ${RESOURCES})

include_directories (${PROJECT_DIRS}
                     ${CMAKE_BINARY_DIR}
                     ${Qt5Core_INCLUDE_DIRS})

list (APPEND LIBS Qt5::Core fcgi)
target_link_libraries (${PROJECT_NAME} ${LIBS})

execute_process (COMMAND gzip -9
                 INPUT_FILE ${PROJECT_NAME}.1
                 OUTPUT_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.1.gz
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

install (TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/")
install (FILES webui_launcher
         DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/sibuserv/"
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE)
install (FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.1.gz
         DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1")

if (WITH_SYSTEMD_SUPPORT)
    install (FILES sibuserv-webui.service
             DESTINATION "${SYSTEMD_UNIT_FILES_DIR}"
             PERMISSIONS OWNER_READ OWNER_WRITE
                         GROUP_READ WORLD_READ)
endif (WITH_SYSTEMD_SUPPORT)

