cmake_minimum_required(VERSION 2.6.0)

# pull in the pods macros. See cmake/pods.cmake for documentation
set(POD_NAME path-util)
include(cmake/pods.cmake)

# require python
find_package(PythonInterp REQUIRED)
# write python file in python/
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/python/path_util.py "def getBasePath(): return '${CMAKE_INSTALL_PREFIX}'\n"
                               "def getDataPath(): return '${CMAKE_INSTALL_PREFIX}/data'\n"
                               "def getConfigPath(): return '${CMAKE_INSTALL_PREFIX}/config'\n"
                               "def getModelsPath(): return '${CMAKE_INSTALL_PREFIX}/models'\n")
# install the python file
pods_install_python_packages(${CMAKE_CURRENT_SOURCE_DIR}/python)

#tell cmake to build these subdirectories
add_subdirectory(src)
