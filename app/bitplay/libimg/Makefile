COPTS = -O2
INCL  = -I. -I/opt/local/include/
CFLAGS = ${INCL} ${COPTS}
OBJS = softdisp.o tiffdisp.o rasdisp.o sgidisp.o jpegdisp.o pnmdisp.o gifdisp.o getimginfo.o getimgdata.o

LIB = libimg.a

${LIB}: ${OBJS}
	ar rv $@ ${OBJS}
	ranlib $@

.PRECIOUS: ${LIB}

clean:
	/bin/rm -f *~ libimg.a *.o

