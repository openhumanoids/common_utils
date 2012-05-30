add_executable(lcm-utils-channel-changer channel-changer.cpp)
pods_use_pkg_config_packages(lcm-utils-channel-changer lcm)
pods_install_executables(lcm-utils-channel-changer)


pods_install_headers(lcm_utils.hpp LcmMatlabRpc.hpp DESTINATION lcm_utils)

set(REQUIRED_LIBS lcm lcmtypes_lcm-utils)

# create a pkg-config file for the library, to make it for other software to
# use it.
pods_install_pkg_config_file(lcm-utils
    CFLAGS
    LIBS
    REQUIRES ${REQUIRED_LIBS}
    VERSION 0.0.1)