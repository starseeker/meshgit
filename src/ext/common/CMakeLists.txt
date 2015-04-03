add_library(common
array.h
list.h
debug.h
json.cpp
json.h
binio.cpp
binio.h
frame.h
func.h
geom.h
montecarlo.h
pam.cpp
pam.h
random_engine.cpp
random_engine.h
random.cpp
random.h
range.h
ray.h
raw.cpp
raw.h
stdmath.cpp
stdmath.h
stddir.h
timer.h
vec.h
quaternion.h
)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_property(TARGET common PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD c++0x)
    set_property(TARGET common PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY libc++)
endif(CMAKE_GENERATOR STREQUAL "Xcode")