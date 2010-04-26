
###################################################################
# Makefile for check
###################################################################

include ../../config.mk
QUANTA_DIR=../../QUANTA
QUANTA_CFLAGS=-I${QUANTA_DIR}/include
QUANTA_LDFLAGS=-L../../lib -lquanta

CFLAGS=$(SAGE_CFLAGS) -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS=-lpthread -lm -ldl $(SAGE_LDFLAGS) $(QUANTA_LDFLAGS) $(GLEW_LIB) -L../../lib -lsail
CC=$(COMPILER)


PROGRAM = checker
FILES= checker.o

default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(COMPILER) -o $(PROGRAM) $(FILES) $(LIBS)

checker.o: checker.cxx
	$(COMPILER) $(CFLAGS) -c checker.cxx


clean:
	\rm -f *~ *.o $(PROGRAM)
