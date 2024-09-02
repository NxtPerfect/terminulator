cmake_minimum_required(VERSION 3.1)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(terminulator VERSION 0.1)

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
file(GLOB_RECURSE HEADER_FILES inc/*.h)
add_executable(terminulator ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(terminulator ${X11_LIBRARIES})
