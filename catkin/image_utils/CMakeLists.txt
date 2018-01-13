add_definitions(-std=gnu99 -fPIC -msse -msse2 -msse3)

cmake_minimum_required(VERSION 2.8.3)
project(image_utils)
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
  INCLUDE_DIRS ../../image-utils/src ${GLIB2_INCLUDE_DIRS}
  LIBRARIES image_utils image_utils_renderer
  CATKIN_DEPENDS bot_core bot_param bot_frames geom_utils
  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

find_package (JPEG REQUIRED)

add_definitions(-std=gnu99)

file(GLOB c_files ../../image-utils/src/jpeg.c ../../image-utils/src/pixels.c)
file(GLOB h_files ../../image-utils/src/jpeg.h ../../image-utils/src/pixels.h)

include_directories(../../image-utils/src/   ../../image-utils/src/renderer ${GLIB2_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR})

add_library(image_utils SHARED ${c_files})
target_link_libraries(image_utils ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m
  ${JPEG_LIBRARIES})
set_property(TARGET image_utils APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS image_utils
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})


file(GLOB c_renderer_files ../../image-utils/src/renderer/renderer_cam_thumb.c)
file(GLOB h_renderer_files ../../image-utils/src/renderer/cam_thumb.h)

add_library(image_utils_renderer SHARED ${c_renderer_files})
target_link_libraries(image_utils_renderer ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
set_property(TARGET image_utils_renderer APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

install(TARGETS image_utils
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${h_renderer_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
