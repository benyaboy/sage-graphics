include ../../config.mk
QUANTA_DIR=../../QUANTA
QUANTA_CFLAGS=-I${QUANTA_DIR}/include
QUANTA_LDFLAGS=-L../../lib -lquanta

CFLAGS=$(SAGE_CFLAGS) -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS=-lpthread -lm -ldl $(QUANTA_LDFLAGS) $(GLEW_LIB) -L../../lib -lsail
CC=${COMPILER}
C-Compiler=gcc
ifeq ($(MACHINE), SunOS)
C-Compiler=cc
endif

IMGLIB = ./libimg
IMAGELIB = ./libimage
IMGLIBS = -L${IMGLIB} -limg -L${IMAGELIB} -limage -ltiff -ljpeg -lm
IMGINCS = -I${IMGLIB} 

PROGS = bplay-noglut img2bmv bpio onethread

default: bplay-noglut

install: default
	/bin/cp -f bplay-noglut ../../bin

all: ${PROGS}

clean:
	rm -f ${PROGS} *.o bplay

bplay-noglut:	bplay.o bpio.o 
	${CC} -o $@ ${SAGE_LDFLAGS} bplay.o bpio.o ${GLLIBS} ${LIBS}

bplay.o: bplay.c bpio.h bpmovie.h
	${CC}  ${CFLAGS} -c bplay.c ${COPTS} ${DEFS}

img2bmv: img2bmv.c bpmovie.h ${IMGLIB}/libimg.a ${IMAGELIB}/libimage.a 
	${C-Compiler} -c ${CFLAGS} ${IMGINCS} img2bmv.c
	${C-Compiler} -o $@ $@.o ${COPTS} ${IMGLIBS}

txspeed: txspeed.c
	${CC} -o $@ $@.c -g -O ${GLINCS} ${GLLIBS}

bpio:	bpiotest.o bpio.o
	${CC} -o $@ bpiotest.o bpio.o ${COPTS} ${LIBS}

#bpio.o bpiotest.o:	bpio.h

onethread: onethread.o
	${CC} -o $@ onethread.o ${COPTS}
