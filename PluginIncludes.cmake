include(${FB_ROOT}/cmake/common.cmake)
fb_check_boost()
add_boost_library(thread)
include(${FB_ROOT}/cmake/CommonPluginConfig.cmake)
