add_executable(lcm-utils-channel-changer channel-changer.cpp)
pods_use_pkg_config_packages(lcm-utils-channel-changer lcm)
pods_install_executables(lcm-utils-channel-changer)

pods_install_headers(lcm_utils.hpp DESTINATION lcm_utils)

