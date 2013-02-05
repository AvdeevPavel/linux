cmake_minimum_required(VERSION 2.8)

#This cmake help to build myserver with myreverse

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/myreverse)

add_subdirectory(myreverse)
add_subdirectory(myserver)
