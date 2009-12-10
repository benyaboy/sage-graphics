include ../../config.mk

###################################################################
# Makefile for dxt
###################################################################

CFLAGS=-fPIC -O3 -I../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS=-lpthread -lm -ldl $(QUANTA_LIB) $(GLEW_LIB) -L../../lib -lsail
CC=$(COMPILER)

PROGRAM = dxt
#FILES= dxt.o app.o
FILES= app2.o

default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(COMPILER) -o $(PROGRAM) $(FILES) $(LIBS)

dxt.o: dxt.cpp
	$(COMPILER) $(CFLAGS) -c dxt.cpp

app.o: app.cpp
	$(COMPILER) $(CFLAGS) -c app.cpp

app2.o: app2.cpp
	$(COMPILER) $(CFLAGS) -c app2.cpp

clean:
	\rm -f *~ *.o $(PROGRAM)

