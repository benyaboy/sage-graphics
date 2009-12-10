MACHINE=$(shell uname -s)
ifeq ($(MACHINE), SunOS)
COPTS = -O2 -xchar=u
else
COPTS = -O2 -funsigned-char
endif

INCL = -I.
CFLAGS = ${COPTS} ${INCL} 
OBJS = close.o filbuf.o flsbuf.o name.o open.o pix.o rdwr.o rle.o row.o # dispimg.o
TARGET = libimage.a

libimage.a:	${OBJS}
	rm -f $@
	ar r $@ ${OBJS}
	ranlib $@

clean:
	rm -f a.out core *.o ${TARGET}
