file(GLOB cpp_files *.cpp)
file(GLOB hpp_files *.hpp)

add_library(eigen-utils SHARED
    ${cpp_files})

target_link_libraries(eigen-utils ${OPENGL_LIBRARIES})

set(REQUIRED_LIBS eigen3 bot2-lcmgl-client)

pods_use_pkg_config_packages(eigen-utils ${REQUIRED_LIBS})

# set the library API version.  Increment this every time the public API
# changes.
set_target_properties(eigen-utils PROPERTIES SOVERSION 1)

# make the header public
pods_install_headers(${hpp_files} DESTINATION eigen_utils)

# make the library public
pods_install_libraries(eigen-utils)

# create a pkg-config file for the library, to make it for other software to
# use it.
pods_install_pkg_config_file(eigen-utils
    CFLAGS
    LIBS -leigen-utils
    REQUIRES ${REQUIRED_LIBS}
    VERSION 0.0.1)
