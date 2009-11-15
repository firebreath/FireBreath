
get_filename_component (CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component (FB_ROOT_DIR "${CMAKE_DIR}/.." ABSOLUTE)
get_filename_component (SOURCE_DIR "${CMAKE_DIR}/../src" ABSOLUTE)
get_filename_component (BUILD_DIR "${CMAKE_DIR}/../build" ABSOLUTE)

set (PROJECT_ROOT "${BUILD_DIR}/projects")
set (BIN_DIR "${BUILD_DIR}/bin")

if (NOT EXISTS ${BUILD_DIR})
   file (MAKE_DIRECTORY ${BUILD_DIR})
endif()

# Get the project paths
include(${CMAKE_DIR}/paths.cmake)

# include the build configuration
include(${CMAKE_DIR}/buildconfig.cmake)

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
endif()

function (browserplugin_project PLUGIN_NAME)

    Project (${PLUGIN_NAME})
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

    include(${FB_ROOT_DIR}/pluginProjects.cmake)

endfunction(browserplugin_project)
