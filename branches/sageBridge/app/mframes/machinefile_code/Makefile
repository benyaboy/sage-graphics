CC=g++

OBJS= machinefile_gen.o file_parsers.o tile_config.o

machinefile_gen: $(OBJS) 
	$(CC) -g -o machinefile_gen $(OBJS) 

install: machinefile_gen
	cp machinefile_gen /home/evl/dmitr/IGRID2005/bin

%.o: %.cpp
	$(CC) -c -o $@ $< 

clean:
	rm *.o machinefile_gen
