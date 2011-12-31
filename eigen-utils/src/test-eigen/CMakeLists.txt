add_executable(eigen-test-triangle-view test_triangle_view.cpp)
pods_use_pkg_config_packages(eigen-test-triangle-view eigen-utils)

add_executable(eigen-test-linalg test_linalg.cpp)
pods_use_pkg_config_packages(eigen-test-linalg eigen-utils)

add_executable(eigen-test-geom test_geom.cpp)
pods_use_pkg_config_packages(eigen-test-geom eigen-utils)

add_executable(eigen-test-rigidbody test_rigidbody.cpp)
pods_use_pkg_config_packages(eigen-test-rigidbody eigen-utils)

add_executable(eigen-test-select_block test_select_block.cpp)
pods_use_pkg_config_packages(eigen-test-select_block eigen-utils)