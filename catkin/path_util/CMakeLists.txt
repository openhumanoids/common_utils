cmake_minimum_required(VERSION 2.8.3)
project(path_util)
find_package(catkin REQUIRED COMPONENTS
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)

catkin_package(
  INCLUDE_DIRS ../../path-util/src ${GLIB2_INCLUDE_DIRS}
  LIBRARIES path_util
#  CATKIN_DEPENDS bot_core bot_param bot_frames
  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

add_definitions(-std=gnu99)

file(GLOB c_files ../../path-util/src/*.c)
file(GLOB h_files ../../path-util/src/*.h)

include_directories(../../path-util/src/ ${GLIB2_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})

add_definitions(-DBASE_PATH="${CMAKE_INSTALL_PREFIX}")

add_library(path_util SHARED ${c_files})
target_link_libraries(path_util ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
set_property(TARGET path_util APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS path_util
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
