find_package(PkgConfig REQUIRED)
pkg_check_modules(BOT2_VIS bot2-vis)
if(NOT BOT2_VIS_FOUND)
    message("\n\n")
    message("bot2-vis not found.  Not building cam thumb renderer")
    message("\n")
    return()
endif()


add_definitions(-std=gnu99)
add_library(image-util-renderer SHARED
    renderer_cam_thumb.c)

pods_use_pkg_config_packages(image-util-renderer bot2-vis)
pods_install_headers(renderer_cam_thumb.h DESTINATION image_utils)
pods_install_libraries(image-util-renderer)
pods_install_pkg_config_file(image-util-renderer
    CFLAGS
    LIBS -limage-util-renderer
    REQUIRES image-utils bot2-vis
    VERSION 0.0.1)