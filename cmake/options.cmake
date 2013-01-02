# options which affect the way Firebreath is built

set(FB_CONFIG_DIR "${FB_ROOT}/src/config" CACHE STRING "Location to find global project configuration files (.h files, etc)" )
mark_as_advanced(FB_CONFIG_DIR)

option(VERBOSE "Enable verbose logging in FireBreath cmake code" OFF)
option(WITH_DYNAMIC_MSVC_RUNTIME "Build with dynamic MSVC runtime (/MD)" OFF)
option(WITH_SYSTEM_BOOST "Build with system Boost" OFF)
