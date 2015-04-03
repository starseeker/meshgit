cmake_minimum_required(VERSION 2.8.3) #8

# project
project(meshgit)

# output directories
if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    #set(CMAKE_CXX_COMPILER /usr/bin/g++-4.7)
    #set(CMAKE_C_COMPILER /usr/bin/g++-4.7)
    
    set(BIN_DIR_SUFFIX mk)
    set(MAKEFILES ON)
    set(CMAKE_VERBOSE_MAKEFILE false)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -g")
    
    find_library(OpenGL_LIBRARY OpenGL)
    
elseif(CMAKE_GENERATOR STREQUAL "Xcode")
    set(BIN_DIR_SUFFIX xcode)
    set(XCODE ON)
    set_property(TARGET PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD c++11)
    
endif(CMAKE_GENERATOR STREQUAL "Unix Makefiles")

#find_package(OpenMP)
#message( "${OpenMP_C_FLAGS} ${OpenMP_CXX_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS -lgomp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${BIN_DIR_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${BIN_DIR_SUFFIX})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${BIN_DIR_SUFFIX})

#include directories
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/meshgit_lib)
include_directories(${PROJECT_SOURCE_DIR}/src/ext/common)
include_directories(${PROJECT_SOURCE_DIR}/src/ext/lodepng)

#opengl and glut
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )
include_directories( ${GLUT_INCLUDE_DIRS} )

# subdirectory
#add_subdirectory(src/ext/common)
add_subdirectory(src)
add_subdirectory(src/meshgit_lib)

