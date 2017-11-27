cmake_minimum_required(VERSION 2.8.3)
project(concise_args)
find_package(catkin REQUIRED COMPONENTS
#  bot_core
#  bot_param
#  bot_frames
#bot_core_lcmtypes
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)

catkin_package(
  INCLUDE_DIRS ../../concise-args/
#  LIBRARIES concise_args
#  CATKIN_DEPENDS bot_core bot_param bot_frames
#  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

#add_definitions(-std=gnu99)

#file(GLOB c_files ../../geom-utils/src/*.c)
#file(GLOB h_files ../../geom-utils/src/*.h)

include_directories(../../concise-args/ )

#configure_file(../../concise-args/ConciseArgs.hpp ${INCLUDE_OUTPUT_PATH}/ConciseArgs COPYONLY)
install(FILES ${INCLUDE_OUTPUT_PATH}/ConciseArgs DESTINATION  ${CATKIN_PACKAGE_INCLUDE_DESTINATION})


#${catkin_INCLUDE_DIRS})

#add_library(concise_args SHARED ${c_files})
#target_link_libraries(concise_args ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
#set_property(TARGET concise_args APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

#install(TARGETS concise_args
#        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
#
#install(FILES ${h_files}
#        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
