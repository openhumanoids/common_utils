add_executable(eigen-test-triangle-view test_triangle_view.cpp)
pods_use_pkg_config_packages(eigen-test-triangle-view eigen-utils)

add_executable(eigen-test-linalg test_linalg.cpp)
pods_use_pkg_config_packages(eigen-test-linalg eigen-utils)
