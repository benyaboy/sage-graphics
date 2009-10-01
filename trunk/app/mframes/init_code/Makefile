QUANTA_DIR=/usr/local
QUANTA_CFLAGS=-I$(QUANTA_DIR)/include -DQUANTA_USE_PTHREADS -DQUANTA_THREAD_SAFE
QUANTA_LIB=-L$(QUANTA_DIR)/lib64 -lquanta_64

SAGE_CFLAGS=-I../../../include
SAGE_LIB=-L../../../lib -lsail

VTK_INCLUDE=/usr/local/VTK-4.4/include
VTK_LIB=/usr/local/VTK-4.4/lib/vtk

SDL_INCLUDE=/usr/include
SDL_LIB=/usr/lib64

CFLAGS=-Wno-deprecated -I/opt/mpich/ch-p4/include -I$(VTK_INCLUDE) -I$(SDL_INCLUDE)

LDFLAGS=-L/opt/mpich/ch-p4/lib64 -lmpich \
-L$(VTK_LIB) -lvtkRendering -lvtkImaging -lvtkHybrid \
-L$(SDL_LIB) -lSDL

CXX=g++

OBJS= initf.o file_parsers.o tile_config.o

init_magic_frames: $(OBJS) 
	$(CXX) -g -O -o init_magic_frames $(OBJS) $(SAGE_LIB) $(QUANTA_LIB) $(LDFLAGS)


install: init_magic_frames
	cp init_magic_frames ../../../bin/MAGIC 

%.o: %.cpp
	$(CXX) -g -c -O $(CFLAGS) $(SAGE_CFLAGS) $(QUANTA_CFLAGS) -o $@ $< 

clean:
	rm *.o init_magic_frames 
