cmake_minimum_required(VERSION 2.6.0)

# pull in the pods macros. See cmake/pods.cmake for documentation
set(POD_NAME laser-utils)
include(cmake/pods.cmake)

# Necessary to build on OSX
find_package(OpenGL REQUIRED)

#tell cmake to build these subdirectories
add_subdirectory(src)
add_subdirectory(src/renderer)
add_subdirectory(src/mapping)
