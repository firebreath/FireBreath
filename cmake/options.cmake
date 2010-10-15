# options which affect the way Firebreath is built

set(PROJECTS_DIR "${CMAKE_SOURCE_DIR}/projects" CACHE STRING "Location to search for project directories")
set(FBLIBS_DIR "${CMAKE_SOURCE_DIR}/src/libs" CACHE STRING "Location to search for 3rd party FireBreath compatible libraries")

option(BUILD_EXAMPLES "Build example projects" OFF)
option(WITH_DYNAMIC_MSVC_RUNTIME "Build with dynamic MSVC runtime (/MD)" OFF)
option(WITH_SYSTEM_BOOST "Build with system Boost" OFF)
