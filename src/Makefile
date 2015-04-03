# http://mrbook.org/tutorials/make/

CPP = g++-4.7
CFLAGS = -g -O3 -Wno-deprecated -Wunused-result -fopenmp -std=c++11 -L. -lglut -lGL -lGLU
#CFLAGS = -g -O3 -fopenmp -Wall -I. -Wno-deprecated  -Wunused-result


# http://www.cs.duke.edu/~ola/courses/programming/libraries.html
LIBCOMMON =	camera.cpp camera.h \
		color_gradient.cpp color_gradient.h \
		debug.cpp debug.h \
		draw_utils.h \
		geom.cpp geom.h \
		globals.cpp globals.h \
		gls.h \
		io_bin.cpp io_bin.h io_json.cpp io_json.h \
		image.h \
		mat.h \
		mesh_utils.cpp mesh_utils.h \
		light.cpp light.h \
		object.h \
		panel.cpp panel.h panelbackground.cpp panelbackground.h property.h \
		std.h std_utils.cpp std_utils.h \
		vec.h \
		viewer.cpp viewer.h \
		xform.h \
		ext/lodepng/lodepng.cpp ext/lodepng/lodepng.h \
		ext/common/frame.h ext/common/ray.h ext/common/vec.h \
		ext/common/quaternion.h
LIBCOMMON_O =	$(LIBCOMMON:%.cpp=%.o)

LIBMESHGIT =	meshgit_lib/binheap.h meshgit_lib/kdtree.cpp meshgit_lib/kdtree.h \
		meshgit_lib/mesh.cpp meshgit_lib/mesh.h \
		meshgit_lib/meshgit.cpp meshgit_lib/meshgit.h \
		meshgit_lib/meshop.cpp meshgit_lib/meshop.h \
		meshgit_lib/quicksort.cpp meshgit_lib/quicksort.h
LIBMESHGIT_O =	$(LIBMESHGIT:%.cpp=%.o)

MESHGITSRC = meshgit.cpp meshgit.h \
		meshgit_viewer.cpp meshgit_diff.cpp meshgit_merge.cpp \
		meshgit_drawutils.cpp meshgit_drawmesh.cpp \
		meshgit_globals.cpp meshgit_cameras.cpp
MESHGITSRC_O = ${MESHGITSRC:%.cpp=%.o}



all: libcommon_lib.a libmeshgit_lib.a meshgit

%.o: %.cpp $(MESHGITSRC:%.h) $(LIBCOMMON:%.h) $(LIBMESHGIT:%.h)
	$(CPP) -c -o $@ $< $(CFLAGS)

libcommon_lib.a: $(LIBCOMMON_O)
	rm -f $@
	ar cq $@ $(LIBCOMMON_O)

libmeshgit_lib.a: $(LIBMESHGIT_O)
	rm -f $@
	ar cq $@ $(LIBMESHGIT_O)

meshgit: $(MESHGITSRC_O) libcommon_lib.a libmeshgit_lib.a
	$(CPP) -o $@ $(MESHGITSRC_O) -lcommon_lib -lmeshgit_lib $(CFLAGS)

clean:
	rm -f *.o *.a meshgit
	rm -f ext/lodepng/*.o
	rm -f meshgit_lib/*.o



