cmake_minimum_required(VERSION 2.8)

project (Allocator)

set(PROJECT myalloc)

set(ALLOC_SOURCE alloc.cpp
		init.cpp
   )

set (WARNS -Wall -Wextra -pedantic-errors)
set (DEFINES "-DM_DEBUG")
set (OPTIMIZE_FLAG " -std=c++0x -O2 -pipe -m64 -finline-functions -finline-limit=1000 -march=core2")
set (SHARED_LINKFLAGS "-fPIC -shared")
set (LINKFLAGS "-lpthread -ldl")

set (FLAGS ${OPTIMIZE_FLAG} ${SHARED_LINKFLAGS} ${LINKFLAGS})
set (ERR_NO_UNIX "Cannot build on non Unix systems")

include_directories(${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/include) 

if (CMAKE_COMPILER_IS_GNUCXX)	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZE_FLAG} ${SHARED_LINKFLAGS} ${LINKFLAGS}")# ${DEFINES}")
else (CMAKE_COMPILER_IS_GNUCXX)
	message (FATAL_ERROR ${ERR_NO_UNIX})
endif (CMAKE_COMPILER_IS_GNUCXX)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}") 

add_library(${PROJECT} SHARED ${ALLOC_SOURCE})
