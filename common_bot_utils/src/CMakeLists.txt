add_definitions(-std=gnu99)

file(GLOB c_files *.c)
file(GLOB h_files *.h)

# Create a shared library libhello.so with a single source file
add_library(common_bot_utils SHARED
    ${c_files})

pods_use_pkg_config_packages(common_bot_utils 
    bot2-core)

# set the library API version.  Increment this every time the public API
# changes.
set_target_properties(common_bot_utils PROPERTIES SOVERSION 1)

# make the header public
# install it to include/hello
pods_install_headers(${h_files} DESTINATION common_bot_utils)

# make the library public
pods_install_libraries(common_bot_utils)

# create a pkg-config file for the library, to make it for other software to
# use it.
pods_install_pkg_config_file(common_bot_utils
    CFLAGS -std=c99
    LIBS -lcommon_bot_utils 
    REQUIRES bot2-core bot2-lcmgl-client bot2-param-client
    VERSION 0.0.1)
