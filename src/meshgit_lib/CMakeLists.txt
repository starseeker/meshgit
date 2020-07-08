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
# For some reason find_package(Threads) isn't giving us pthread on Linux??
target_link_libraries(meshgit_lib pthread)

#add_executable(meshgit meshgit.cpp)
#target_link_libraries(meshgit meshgit_lib)

