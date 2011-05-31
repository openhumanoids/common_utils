find_package(PkgConfig REQUIRED)
pkg_check_modules(OCC_MAP occ-map)
if(OCC_MAP_FOUND)
    add_executable(laser-voxmapper laser_voxmapper.cpp)
    pods_use_pkg_config_packages(laser-voxmapper 
                    bot2-core bot2-param-client bot2-frames
                    occ-map laser-util)   
    pods_install_executables(laser-voxmapper) 
else()
    message("occ-map not found. not building laser-voxmapper")              
endif()

pkg_check_modules(OCTOMAP_UTIL octomap-util)
if(OCTOMAP_UTIL_FOUND) 
    add_executable(laser-octomapper laser_octomapper.cpp)
    pods_use_pkg_config_packages(laser-octomapper 
                    bot2-core bot2-param-client bot2-frames
                    octomap-util laser-util)
                   
    pods_install_executables(laser-octomapper)
else()
    message("octomap-util not found. not building laser-octomapper")
endif()