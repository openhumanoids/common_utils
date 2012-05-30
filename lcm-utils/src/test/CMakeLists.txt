add_executable(lcm-matlab-rpc-test lcm-matlab-rpc-test.cpp)
pods_use_pkg_config_packages(lcm-matlab-rpc-test lcm-utils bot2-core eigen3)
