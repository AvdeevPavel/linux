cmake_minimum_required(VERSION 2.8)

set(PROJECT myserver)

set(MYSERVER_SOURCES 
	)

set (WARNS "-Wall -Wextra -pedantic-errors")
#set (DEFINES "-DM_DEBUG")
set (OPTIMIZE_FLAGS "-std=gnu99") 

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${WARNS} ${OPTIMIZE_FLAGS}")

add_executable(${PROJECT} ${MYSERVER_SOURCES})
