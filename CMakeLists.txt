cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(termulator VERSION 0.1)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(X11 REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})

link_directories(${X11_LIBRARIES})

file(GLOB_RECURSE SRC_FILES src/*.c)
# file(GLOB_RECURSE HEADER_FILES include/*.h)
# add_executable(term ${SRC_FILES} ${HEADER_FILES})
add_executable(termulator ${SRC_FILES})

target_link_libraries(termulator ${X11_LIBRARIES})
