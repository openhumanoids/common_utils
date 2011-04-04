cmake_minimum_required(VERSION 2.6.0)

set(POD_NAME common_image_utils)

# Necessary to build on OSX
find_package(OpenGL REQUIRED)

# This lets us use the pods cmake macros
include(cmake/pods.cmake)

add_subdirectory(src)
add_subdirectory(src/renderer)