cmake_minimum_required(VERSION 2.8.3)
project(eigen_utils)
find_package(catkin REQUIRED COMPONENTS
eigen_utils_lcmtypes
bot_lcmgl
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)

find_package(Eigen3 REQUIRED)

catkin_package(
  INCLUDE_DIRS ../../eigen-utils/src ${GLIB2_INCLUDE_DIRS}
  LIBRARIES eigen_utils
  CATKIN_DEPENDS bot_lcmgl eigen_utils_lcmtypes
  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

file(GLOB c_files ../../eigen-utils/src/*.cpp)
file(GLOB h_files ../../eigen-utils/src/*.hpp)

include_directories(../../eigen-utils/src/ ${GLIB2_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS})

add_library(eigen_utils SHARED ${c_files})
target_link_libraries(eigen_utils ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
set_property(TARGET eigen_utils APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS eigen_utils
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
