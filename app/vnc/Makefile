
###################################################################
# Makefile for VNC
###################################################################

include ../../config.mk

CFLAGS=-fPIC -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
#LIBS=-lpthread -lm -ldl $(QUANTA_LIB) $(GLEW_LIB) -L../../lib -lsail -lz -ljpeg
LIBS+= -L../../lib -lsail -lz -ljpeg -lquanta
CC=$(COMPILER)


PROGRAM = VNCViewer

FILES = d3des.o vncauth.o argsresources.o colour.o rfbproto.o sockets.o \
	sgVNCViewer.o main.o

default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(CC) -o $(PROGRAM) $(FILES) $(LIBS)

.cpp.o :
	$(COMPILER) $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *~ *.o $(PROGRAM)

