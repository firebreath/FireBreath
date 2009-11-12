
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

get_filename_component (CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include (${CMAKE_DIR}/paths.cmake)

if (GEN)
    set(GEN_ARG "-G '${GEN}'")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}"
    ${GEN_ARG}
    -D "CMAKE_DIR:PATH=${CMAKE_DIR}"

    ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${BUILD_DIR}
    )
