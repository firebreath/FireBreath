#/**********************************************************\ 
# Auto-generated Windows project definition file for the
# FBTestPlugin project
#\**********************************************************/

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Win/
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

message("SOURCES = ${SOURCES}")

set(PLIST "Mac/bundle_template/Info.plist")
set(STRINGS "Mac/bundle_template/InfoPlist.strings")
set(LOCALIZED "Mac/bundle_template/Localized.r")


add_mac_plugin(${PROJNAME} ${PLIST} ${STRINGS} ${LOCALIZED} ${SOURCES})
    )

message("Linking ${PROJNAME} to ${PLUGIN_INTERNAL_DEPS}")
# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    )

add_dependencies(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    )
