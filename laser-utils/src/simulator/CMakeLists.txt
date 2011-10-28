find_package(PkgConfig REQUIRED)
pkg_check_modules(OCC_MAP occ-map)
if(NOT OCC_MAP_FOUND)
	message("occ-map not found. not building laser-simulator")
	return()              
endif()


add_library(laser-sim SHARED
    LaserSim.cpp)
pods_install_headers(LaserSim.hpp DESTINATION laser_utils)

set(REQUIRED_LIBS bot2-core geom-utils occ-map)
pods_use_pkg_config_packages(laser-sim ${REQUIRED_LIBS})
 
# make the library public
pods_install_libraries(laser-sim)

# create a pkg-config file for the library, to make it easier for other
# software to use.
pods_install_pkg_config_file(laser-sim
    CFLAGS
    LIBS -llaser-sim 
    REQUIRES ${REQUIRED_LIBS}
    VERSION 0.0.1)


add_executable(laser-simulator simulator.cpp)
pods_use_pkg_config_packages(laser-simulator bot2-core bot2-param-client bot2-frames laser-sim)   
pods_install_executables(laser-simulator) 

  