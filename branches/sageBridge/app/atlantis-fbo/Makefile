# QUANTA settings
QUANTA_DIR=/usr/local
QUANTA_CFLAGS=-I${QUANTA_DIR}/include -DQUANTA_USE_PTHREADS -DQUANTA_THREAD_SAFE
QUANTA_LIB=-L${QUANTA_DIR}/lib

INCLUDE_DIR 	= -I/usr/local/include -I/usr/X11R6/include -I../../include
LIBRARY_DIR 	= -L/usr/X11R6/lib -L../../lib
LIBRARIES	= -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm -lpthread -lsail
COMPILER_FLAGS 	= -O3 -Wno-deprecated -fomit-frame-pointer -ffast-math
COMPILER 	= g++

CC=$(COMPILER)

# Machine specific settings
MACHINE=$(shell uname -p)
   #powerpc  MacOSX
   #x86_64   Opteron Linux
   #ia64     Itanium Linux (SGI)
   #else     32bit Linux

ifeq ($(MACHINE), powerpc)

   LIBRARIES       = -framework GLUT -framework OpenGL -lobjc -lm -lpthread -lsail
   COMPILER_FLAGS  += -FGLUT -FOpenGL

else

ifeq ($(MACHINE), x86_64)
   LIBRARY_DIR = -L/usr/X11R6/lib64 -L../../lib
else

ifeq ($(MACHINE), ia64)
   LIBRARY_DIR = -L/usr/X11R6/lib64 -L../../lib
else

   # anything else is 32bit 

   LIBRARIES       += $(QUANTA_LIB) -lquanta_32


endif
endif
endif

CFLAGS=$(COMPILER_FLAGS) $(INCLUDE_DIR)
LDFLAGS=$(LIBRARY_DIR) $(LIBRARIES)

PROGRAM = atlantis

FILES = atlantis.o dolphin.o shark.o swim.o whale.o

default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(CC) -o $(PROGRAM) $(FILES) $(LDFLAGS)

clean:
	/bin/rm -f *~ *.o $(PROGRAM)

