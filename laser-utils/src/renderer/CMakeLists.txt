find_package(PkgConfig REQUIRED)
pkg_check_modules(BOT2_VIS bot2-vis)
if(NOT BOT2_VIS_FOUND)
    message("\n\n")
    message("bot2-vis not found.  Not building laser renderer")
    message("\n")
    return()
endif()


add_definitions(-std=gnu99)

add_library(laser-util-renderer SHARED
    renderer_laser.c)

pods_use_pkg_config_packages(laser-util-renderer bot2-vis)
pods_install_headers(renderer_laser.h DESTINATION laser_utils)
pods_install_libraries(laser-util-renderer)
pods_install_pkg_config_file(laser-util-renderer
    CFLAGS
    LIBS -llaser-util-renderer
    REQUIRES laser-util bot2-vis
    VERSION 0.0.1)