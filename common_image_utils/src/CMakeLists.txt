add_definitions(-std=gnu99 -fPIC -msse -msse2 -msse3)

file(GLOB c_files *.c)
file(GLOB h_files *.h)
add_library(common_image_utils SHARED ${c_files})

target_link_libraries(common_image_utils -ljpeg)

# set the library API version.  Increment this every time the public API
# changes.
set_target_properties(common_image_utils PROPERTIES SOVERSION 1)

#pods_use_pkg_config_packages(common_image_utils 
#    glib-2.0)

# make the shared library public
pods_install_libraries(common_image_utils)
#make the header public
pods_install_headers(${h_files} DESTINATION common_image_utils)

pods_install_pkg_config_file(common_image_utils
    LIBS -lcommon_image_utils 
    REQUIRES glib-2.0
    VERSION 0.0.1)
