add_library(meshgit_lib SHARED
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
target_link_libraries(meshgit_lib Threads::Threads)

#add_executable(meshgit meshgit.cpp)
#target_link_libraries(meshgit meshgit_lib)

