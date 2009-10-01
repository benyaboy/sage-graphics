
###################################################################
# Makefile for yuv
###################################################################

include ../../config.mk

CFLAGS=-fPIC -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS=-lpthread -lm -ldl $(QUANTA_LIB) $(GLEW_LIB) -L../../lib -lsail
CC=$(COMPILER)


PROGRAM = yuv
FILES= yuv.o

default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(COMPILER) -o $(PROGRAM) $(FILES) $(LIBS)

yuv.o: yuv.cxx
	$(COMPILER) $(CFLAGS) -c yuv.cxx


clean:
	\rm -f *~ *.o $(PROGRAM)
