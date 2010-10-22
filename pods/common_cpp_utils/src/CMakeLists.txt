file(GLOB cpp_files *.cpp)
file(GLOB hpp_files *.hpp)
add_library(common_cpp_utils SHARED ${cpp_files})

# set the library API version.  Increment this every time the public API
# changes.
set_target_properties(common_cpp_utils PROPERTIES SOVERSION 1)

pods_use_pkg_config_packages(common_cpp_utils 
    bot2-core)

# make the shared library public
pods_install_libraries(common_cpp_utils)
#make the header public
pods_install_headers(${hpp_files} DESTINATION common_cpp_utils)

pods_install_pkg_config_file(common_cpp_utils
    LIBS -lcommon_cpp_utils 
    REQUIRES bot2-core
    VERSION 0.0.1)
