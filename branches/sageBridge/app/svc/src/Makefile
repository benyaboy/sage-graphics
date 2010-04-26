include ../../../config.mk

CFLAGS=-fPIC -O3 -I../include -I../../../include $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS)
LIBS=-lpthread -lm -ldl $(QUANTA_LIB) $(GLEW_LIB) -L../../../lib -lsail -lraw1394 -lpthread -liec61883 -lmpeg2 -lmpeg2convert $(PAUDIO_LIB)
CC=$(COMPILER)

default:
	g++ -c $(CFLAGS) main.cpp SVC_control.cpp SVC_input_1394.cpp SVC_input_module.cpp SVC_decode_mpeg2.cpp SVC_buffer_manager.cpp SVC_output_module.cpp SVC_output_sail.cpp SVC_input_net.cpp SVC_output_null.cpp SVC_output_net.cpp SVC_input_null.cpp
	g++ $(LIBS) -o svc main.o SVC_control.o SVC_input_1394.o SVC_input_module.o SVC_decode_mpeg2.o SVC_buffer_manager.o SVC_output_module.o SVC_output_sail.o SVC_input_net.o SVC_output_null.o SVC_output_net.o SVC_input_null.o 

install: default
	cp svc  ../../../bin

clean:
	/bin/rm -f *.o svc
