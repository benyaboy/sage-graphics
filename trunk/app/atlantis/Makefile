include ../../config.mk
QUANTA_DIR=../../QUANTA
QUANTA_CFLAGS=-I${QUANTA_DIR}/include
QUANTA_LDFLAGS=-L../../lib -lquanta


CFLAGS=-fPIC -O3 -I../../include $(QUANTA_CFLAGS) 
CC=$(COMPILER)

ifeq ($(MACHINE), Darwin)
   CFLAGS+= $(SAGE_CFLAGS) -FGLUT -FOpenGL
   LIBS= $(SAGE_LDFLAGS) -lpthread -L../../lib -lsail -framework GLUT -framework OpenGL -lobjc -lm 
else
ifeq ($(MACHINE), SunOS)
   CFLAGS=$(SAGE_CFLAGS) -O3 -I../../include $(QUANTA_CFLAGS) $(GLUT_CFLAGS) 
   LIBS=-lpthread -lm -lGL -lGLU $(QUANTA_LDFLAGS) -L../../lib -lsail $(GLUT_LDFLAGS) 
else
   LIBS=-lpthread -lm -lGL -lGLU $(GLUT_LDFLAGS) $(QUANTA_LDFLAGS) $(GLEW_LIB) -L../../lib -lsail
endif
endif

PROGRAM = atlantis

FILES = atlantis.o dolphin.o shark.o swim.o whale.o

default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(COMPILER) -o $(PROGRAM) $(FILES) $(SAGE_LDFLAGS) $(CFLAGS) $(LIBS)

clean:
	/bin/rm -f *~ *.o $(PROGRAM)

