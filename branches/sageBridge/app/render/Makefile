
###################################################################
# Makefile for render
###################################################################
include ../../config.mk
QUANTA_DIR=../../QUANTA
QUANTA_CFLAGS=-I${QUANTA_DIR}/include
QUANTA_LDFLAGS=-L../../lib -lquanta

COMPILER_FLAGS=$(SAGE_CFLAGS) -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS) $(GLUT_CFLAGS)


ifeq ($(MACHINE), Darwin)
   COMPILER_FLAGS  += -FGLUT -FOpenGL
   LIBS=-lpthread -L../../lib -lsail -framework GLUT -framework OpenGL -lobjc -lm 
else
   LIBS=-lpthread ${SUN_LIBS} -lm -ldl $(GLUT_LDFLAGS) -lGL -lGLU $(QUANTA_LDFLAGS) $(GLEW_LIB) -L../../lib -lsail
endif



default: render

install: default
	cp render ../../bin

render: render.o
	$(COMPILER) $(SAGE_LDFLAGS) -o render render.o $(LIBS)

render-mpi: render-mpi.cpp
	mpiCC -o render-mpi render-mpi.cpp  $(COMPILER_FLAGS) $(INCLUDE_DIR)  $(LIBRARY_DIR) $(LIBRARIES)

render.o:       render.cxx
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDE_DIR) -c render.cxx

clean:
	\rm -f *~ *.o $(PROJ_EXE)

