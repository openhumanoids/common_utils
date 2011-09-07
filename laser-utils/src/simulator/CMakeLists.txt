find_package(PkgConfig REQUIRED)
pkg_check_modules(OCC_MAP occ-map)
if(OCC_MAP_FOUND)
    add_executable(laser-simulator laser_simulator.cpp)
    pods_use_pkg_config_packages(laser-simulator 
                    bot2-core bot2-param-client bot2-frames
                    occ-map geom-utils)   
    pods_install_executables(laser-simulator) 
else()
    message("occ-map not found. not building laser-simulator")              
endif()