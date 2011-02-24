#/**********************************************************\ 
# Auto-generated Mac project definition file for the
# FBTestPlugin project
#\**********************************************************/

# Mac template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Mac/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*.cpp
    Mac/*.h
    Mac/*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    
)

SOURCE_GROUP(Mac FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

set(PLIST "Mac/bundle_template/Info.plist")
set(STRINGS "Mac/bundle_template/InfoPlist.strings")
set(LOCALIZED "Mac/bundle_template/Localized.r")

add_mac_plugin(${PROJECT_NAME} ${PLIST} ${STRINGS} ${LOCALIZED} SOURCES)

message("Linking ${PROJECT_NAME} to ${PLUGIN_INTERNAL_DEPS}")
# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME}
    ${PLUGIN_INTERNAL_DEPS}
    )

get_target_property(PLUGIN_BUILD_DIR ${PROJECT_NAME} LIBRARY_OUTPUT_DIRECTORY)
get_target_property(PLUGIN_FNAME ${PROJECT_NAME} OUTPUT_NAME)

if (NOT PLUGIN_BUILD_DIR)
    set (PLUGIN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()
if (NOT PLUGIN_FNAME)
    set (PLUGIN_FNAME ${PLUGIN_NAME})
endif()

# This is still highly experimental!

INSTALL(DIRECTORY ${PLUGIN_BUILD_DIR}/Debug/${PLUGIN_FNAME}.plugin/ DESTINATION "${PLUGIN_NAME}.plugin" COMPONENT plugin)

set (CPACK_INSTALL_CMAKE_PROJECTS 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_NAME}
    plugin
    /
)
set (CPACK_DMG_FORMAT UDBZ)
set (CPACK_OSX_PACKAGE_VERSION "10.5")
set (CPACK_PACKAGE_NAME "${FBSTRING_ProductName}")
set (CPACK_PACKAGING_INSTALL_PREFIX "/private/tmp/plugin_${PLUGIN_FNAME}")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "${FBSTRING_ProductName}")
set (CPACK_PACKAGE_VENDOR "${FBSTRING_CompanyName}")
set (CPACK_PACKAGE_VERSION "${FBSTRING_PLUGIN_VERSION}")
set (CPACK_PACKAGE_FILE_NAME "${FBSTRING_PluginFileName}")
set (CPACK_GENERATOR PackageMaker DragNDrop TGZ ZIP)
set (CPACK_INSTALL_DIRECTORIES ${PLUGIN_BUILD_DIR}/Debug/${PLUGIN_FNAME}.plugin/)
set (CPACK_OUTPUT_CONFIG_FILE ${PLUGIN_BUILD_DIR}/Debug/CPackConfig.cmake)
include (CPack)
