cmake_minimum_required(VERSION 2.6.0)

set(POD_NAME common_bot_utils)
include(cmake/pods.cmake)


#if we can find the bot-core library, build the scan-matcher 
find_library(BOT2_CORE_FOUND bot2-core)
if (BOT2_CORE_FOUND)
     add_subdirectory(src)
else(BOT2_CORE_FOUND)
     message(STATUS "bot2-core not found, not building common_bot_utils!")
endif(BOT2_CORE_FOUND)

