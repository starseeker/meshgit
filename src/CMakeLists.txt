add_library(common_lib
    camera.cpp camera.h
    color_gradient.cpp color_gradient.h
    debug.cpp debug.h
    draw_utils.h
    geom.cpp geom.h
    globals.cpp globals.h
    gls.h
    io_bin.cpp io_bin.h io_json.cpp io_json.h
    image.h
    mat.h
    mesh_utils.cpp mesh_utils.h
    light.cpp light.h
    object.h
    panel.cpp panel.h
    panelbackground.cpp panelbackground.h
    property.h
    std.h
    std_utils.cpp std_utils.h
    vec.h
    viewer.cpp viewer.h
    xform.h
    ext/lodepng/lodepng.cpp ext/lodepng/lodepng.h
)

set(meshgit_srcs
    meshgit.cpp
    meshgit.h
    meshgit_viewer.cpp
    meshgit_diff.cpp
    meshgit_merge.cpp
    meshgit_drawutils.cpp
    meshgit_drawmesh.cpp
    meshgit_globals.cpp
    meshgit_cameras.cpp
)


set( CORELIBS ${OPENGL_gl_LIBRARY} ${GLUT_LIBRARIES} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} )

target_link_libraries( common_lib ${CORELIBS} )

add_executable(meshgit ${meshgit_srcs})
target_link_libraries(meshgit common_lib meshgit_lib ${CORELIBS})

