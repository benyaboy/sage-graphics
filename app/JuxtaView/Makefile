include ../../config.mk

CXXFLAGS= -Wno-deprecated  -fPIC -O3 -I../../include -I/opt/mpich/include  $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS=-lpthread -lm -ldl $(QUANTA_LIB) $(GLEW_LIB) -L../../lib -lsail -ltiff -L/opt/mpich/ch-p4/lib64 -lmpich
CC=$(COMPILER)

default: JuxtaView

install: JuxtaView
	cp JuxtaView bin

JuxtaView: JuxtaUIServer.o FileInfo.o GlobalCoordinatesToFileMapper.o main.cpp Makefile
	$(CXX) $(CXXFLAGS) -o JuxtaView main.cpp JuxtaUIServer.o GlobalCoordinatesToFileMapper.o FileInfo.o $(LIBS)

UIServer: JuxtaUIServer.o servermain.o Makefile
	g++ -o JuxtaUIServer JuxtaUIServer.o servermain.o -L$(QUANTA_DIR)/lib64 -ltiff -lquanta_64 -lpthread

JuxtaUIServer.o: JuxtaUIServer.cpp
servermain.o: servermain.cpp
FileInfo.o: FileInfo.cpp
GlobalCoordinatesToFileMapper.o: GlobalCoordinatesToFileMapper.cpp

clean:
	rm -rf *.o *~ JuxtaView

