###################################################################
# Makefile for VNC
###################################################################

include ../../config.mk

CC=$(COMPILER)

CFLAGS=$(SAGE_CFLAGS) -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS+=$(SAGE_LDFLAGS) -L../../lib -lsail -lz -ljpeg $(QUANTA_LDFLAGS) $(PAUDIO_LIB)

ifeq ($(MACHINE), Darwin)
        CFLAGS+= -I/opt/local/include
        LIBS+= -L/opt/local/lib
endif


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

