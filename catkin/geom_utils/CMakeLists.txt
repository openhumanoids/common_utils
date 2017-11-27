cmake_minimum_required(VERSION 2.8.3)
project(geom_utils)
find_package(catkin REQUIRED COMPONENTS
  bot_core
  bot_param
  bot_frames
#bot_core_lcmtypes
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)

catkin_package(
  INCLUDE_DIRS ../../geom-utils/src ${GLIB2_INCLUDE_DIRS}
  LIBRARIES geom_utils
  CATKIN_DEPENDS bot_core bot_param bot_frames
  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

add_definitions(-std=gnu99)

file(GLOB c_files ../../geom-utils/src/*.c)
file(GLOB h_files ../../geom-utils/src/*.h)

include_directories(../../geom-utils/src/bot_core/ ${GLIB2_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})

add_library(geom_utils SHARED ${c_files})
target_link_libraries(geom_utils ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
set_property(TARGET geom_utils APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS geom_utils
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
