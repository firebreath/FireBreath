
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (FireBreath)
message("Firebreath solution dir: " ${PROJECT_BINARY_DIR})

add_subdirectory(${FIREBREATHWIN_SOURCE_DIR} ${FIREBREATHWIN_BINARY_DIR})
add_subdirectory(${UNITTEST_FW_SOURCE_DIR} ${UNITTEST_FW_BINARY_DIR})

