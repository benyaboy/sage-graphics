#
#	Makefile for QUANTA SAGE version
#	

include ../../config.mk

#MYFLAGS=-g -Wno-deprecated -fPIC -O3 -I../include $(SDL_CFLAGS) $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
MYFLAGS=${SAGE_CFLAGS} -O3 -I../include  

LIBS= -lpthread -lm -ldl $(SUN_LIBS)


OBJS=QUANTAnet_tcp_c.o QUANTAnet_socketbase_c.o QUANTAnet_iovec_c.o QUANTAglobals.o QUANTAnet_perfMonitor_c.o QUANTAnet_perfDaemon_c.o QUANTAnet_perfClient_c.o QUANTAnet_datapack_c.o

DIRT= *.o core core.* *.so 

build: $(QUANTA_LIB)

install: build
	cp $(QUANTA_LIB) ../../lib

clean:
	rm -f $(DIRT)
	cd ../../lib; rm -f $(QUANTA_LIB); cd ../QUANTA/src


$(QUANTA_LIB): $(OBJS) 
	$(COMPILER) $(SHLD_FLAGS) $(OBJS) $(LIBS) -o $(QUANTA_LIB)

%.o : %.cxx 
	$(COMPILER) $(MYFLAGS) -c -o $@ $<
