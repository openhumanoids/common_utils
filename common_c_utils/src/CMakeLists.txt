add_definitions(-std=gnu99)

file(GLOB c_files *.c)
file(GLOB h_files *.h)
add_library(common_c_utils SHARED ${c_files})

# set the library API version.  Increment this every time the public API
# changes.
set_target_properties(common_c_utils PROPERTIES SOVERSION 1)

pods_use_pkg_config_packages(common_c_utils 
    glib-2.0)

# make the shared library public
pods_install_libraries(common_c_utils)
#make the header public
pods_install_headers(${h_files} DESTINATION common_c_utils)

pods_install_pkg_config_file(common_c_utils
    LIBS -lcommon_c_utils 
    REQUIRES glib-2.0
    VERSION 0.0.1)
