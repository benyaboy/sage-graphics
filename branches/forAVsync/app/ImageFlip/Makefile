
SAGE_DIR = ../..

CFLAGS=`Wand-config --cflags --cppflags`  -I$(SAGE_DIR)/include -fno-stack-protector
LIBS= `Wand-config --ldflags --libs` -lpthread -lstdc++ -lltdl -L$(SAGE_DIR)/lib -lsail  -lquanta

FILES=imageflip.o util.o
TARGET=imageflip


default: $(TARGET)

install: $(TARGET)
	/bin/cp -f imageflip $(SAGE_DIR)/bin
	
imageflip: $(FILES)
	g++ -o imageflip $(FILES) $(LIBS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o *~ imageflip

