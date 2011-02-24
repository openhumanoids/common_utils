add_definitions(-std=gnu99)
# Create a shared library liblaser-util.so with a single source file
add_library(laser-util SHARED
    laser_util.c)

# make the header public
# install it to include/laser-util
pods_install_headers(laser_util.h DESTINATION laser_utils)
pods_use_pkg_config_packages(laser-util glib-2.0)
 
# make the library public
pods_install_libraries(laser-util)

# create a pkg-config file for the library, to make it easier for other
# software to use.
pods_install_pkg_config_file(laser-util
    CFLAGS
    LIBS -llaser-util 
    REQUIRES bot2-core bot2-frames bot2-param-client geom-utils
    VERSION 0.0.1)

    
    

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