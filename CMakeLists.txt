cmake_minimum_required(VERSION 3.12)

# project
project(meshgit)

unset(C_STANDARD_FLAGS)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(C_STANDARD_FLAGS "${CMAKE_C_FLAGS_DEFAULT} ${CMAKE_C${CMAKE_C_STANDARD}_STANDARD_COMPILE_OPTION} ${API_FLAGS}")
string(STRIP "${C_STANDARD_FLAGS}" C_STANDARD_FLAGS)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_STANDARD_FLAGS "${CMAKE_CXX_FLAGS_DEFAULT} ${CMAKE_CXX${CMAKE_CXX_STANDARD}_STANDARD_COMPILE_OPTION} ${API_FLAGS}")
string(STRIP "${CXX_STANDARD_FLAGS}" CXX_STANDARD_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/meshgit_lib)
include_directories(${CMAKE_SOURCE_DIR}/src/ext/common)
include_directories(${CMAKE_SOURCE_DIR}/src/ext/lodepng)

#opengl and glut
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )
include_directories( ${GLUT_INCLUDE_DIRS} )

find_package(Threads REQUIRED)

# subdirectory
add_subdirectory(src)
add_subdirectory(src/meshgit_lib)

