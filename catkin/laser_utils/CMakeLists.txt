cmake_minimum_required(VERSION 2.8.3)
project(laser_utils)
find_package(catkin REQUIRED COMPONENTS
  bot_core
  bot_param
  bot_frames
  geom_utils
#bot_core_lcmtypes
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)

catkin_package(
  INCLUDE_DIRS ../../laser-utils/src ${GLIB2_INCLUDE_DIRS}
  LIBRARIES laser_utils laser_utils_renderer
  CATKIN_DEPENDS bot_core bot_param bot_frames geom_utils
  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

add_definitions(-std=gnu99)

file(GLOB c_files ../../laser-utils/src/laser_util.c)
file(GLOB h_files ../../laser-utils/src/laser_util.h)

include_directories(../../laser-utils/src/   ../../laser-utils/src/renderer ${GLIB2_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})

add_library(laser_utils SHARED ${c_files})
target_link_libraries(laser_utils ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
set_property(TARGET laser_utils APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS laser_utils
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})


file(GLOB c_renderer_files ../../laser-utils/src/renderer/renderer_laser.c)
file(GLOB h_renderer_files ../../laser-utils/src/renderer/renderer_laser.h)

add_library(laser_utils_renderer SHARED ${c_renderer_files})
target_link_libraries(laser_utils_renderer ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
set_property(TARGET laser_utils_renderer APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS laser_utils
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_renderer_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
