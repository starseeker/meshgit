add_library(meshgit_lib
    binheap.h
    kdtree.cpp
    kdtree.h
    mesh.cpp
    mesh.h
    meshgit.cpp
    meshgit.h
    meshop.cpp
    meshop.h
    quicksort.cpp
    quicksort.h
)

#add_executable(meshgit meshgit.cpp)
#target_link_libraries(meshgit meshgit_lib)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_property(TARGET meshgit_lib PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD c++11)
    set_property(TARGET meshgit_lib PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY libc++)
endif(CMAKE_GENERATOR STREQUAL "Xcode")
