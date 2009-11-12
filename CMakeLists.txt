
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (FireBreath)
message("Firebreath solution dir: " ${PROJECT_BINARY_DIR})

add_subdirectory(${UNITTEST_FW_SOURCE_DIR} ${UNITTEST_FW_BINARY_DIR})

add_subdirectory(${ACTIVEXPLUGIN_SOURCE_DIR} ${ACTIVEXPLUGIN_BINARY_DIR})
add_subdirectory(${ACTIVEXPLUGINTEST_SOURCE_DIR} ${ACTIVEXPLUGINTEST_BINARY_DIR})

add_subdirectory(${SCRIPTINGCORE_SOURCE_DIR} ${SCRIPTINGCORE_BINARY_DIR})
add_subdirectory(${SCRIPTINGCORETEST_SOURCE_DIR} ${SCRIPTINGCORETEST_BINARY_DIR})

add_subdirectory(${NPAPIPLUGIN_SOURCE_DIR} ${NPAPIPLUGIN_BINARY_DIR})
add_subdirectory(${NPAPIHOST_SOURCE_DIR} ${NPAPIHOST_BINARY_DIR})
