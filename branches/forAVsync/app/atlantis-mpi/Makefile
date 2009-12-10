PROGRAM = atlantis-mpi
CC=g++ -I/opt/mpich/include -Wno-deprecated

FILES = atlantis.o dolphin.o shark.o swim.o whale.o

LDFLAGS += -lglut -lGLU -lGL -L/usr/X11R6/lib64 -lXmu -lXi -lX11 -lm -ldl -L../../lib -lsail -L/opt/mpich/ch-p4/lib64 -lmpich
CFLAGS += -O -fomit-frame-pointer -ffast-math -I../../include


default: $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) ../../bin

$(PROGRAM): $(FILES)
	$(CC) -o $(PROGRAM) $(FILES) $(LDFLAGS)

clean:
	/bin/rm -f *~ *.o $(PROGRAM)
