cmake_minimum_required(VERSION 2.8.3)
project(stl_utils)
find_package(catkin REQUIRED COMPONENTS
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)

catkin_package(
  INCLUDE_DIRS ../../stl-utils/src 
#  LIBRARIES stl_utils
#  CATKIN_DEPENDS bot_core bot_param bot_frames
#  DEPENDS GLIB2
#  CFG_EXTRAS bot-core-includes.cmake
)

add_definitions(-std=gnu99)

#file(GLOB c_files ../../stl-utils/src/*.c)
file(GLOB hpp_files ../../stl-utils/src/*.hpp)

include_directories(../../stl-utils/src/ ${GLIB2_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})

install(FILES ${INCLUDE_OUTPUT_PATH}/stl_utils DESTINATION  ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

#add_library(stl_utils SHARED ${c_files})
#target_link_libraries(stl_utils ${catkin_LIBRARIES} ${GLIB2_LIBRARIES} m)
#set_property(TARGET stl_utils APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-unused-result )

#install(TARGETS stl_utils
#        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

install(FILES ${hpp_files}
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
