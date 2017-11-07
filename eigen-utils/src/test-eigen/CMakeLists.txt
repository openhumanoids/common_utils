add_executable(eigen-test-triangle-view test_triangle_view.cpp)
pods_use_pkg_config_packages(eigen-test-triangle-view eigen-utils)
target_link_libraries(eigen-test-triangle-view X11)

add_executable(eigen-test-linalg test_linalg.cpp)
pods_use_pkg_config_packages(eigen-test-linalg eigen-utils)
target_link_libraries(eigen-test-linalg X11)

add_executable(eigen-test-geom test_geom.cpp)
pods_use_pkg_config_packages(eigen-test-geom eigen-utils)
target_link_libraries(eigen-test-geom X11)

add_executable(eigen-test-rigidbody test_rigidbody.cpp)
pods_use_pkg_config_packages(eigen-test-rigidbody eigen-utils)
target_link_libraries(eigen-test-rigidbody X11)

add_executable(eigen-test-select-block test_select_block.cpp)
pods_use_pkg_config_packages(eigen-test-select-block eigen-utils)
target_link_libraries(eigen-test-select-block X11)

add_executable(eigen-test-lcm-marshalling test_lcm_marshalling.cpp)
pods_use_pkg_config_packages(eigen-test-lcm-marshalling eigen-utils)
target_link_libraries(eigen-test-lcm-marshalling X11)

add_executable(eigen-test-file-io test_file_io.cpp)
pods_use_pkg_config_packages(eigen-test-file-io eigen-utils)
target_link_libraries(eigen-test-file-io X11)

add_executable(eigen-test-lcm-matlab-rpc test_lcm_matlab_rpc.cpp)
pods_use_pkg_config_packages(eigen-test-lcm-matlab-rpc eigen-utils)
target_link_libraries(eigen-test-lcm-matlab-rpc X11)

