#
#	Makefile for sage
#	

include ../config.mk

MYFLAGS=-g ${SAGE_CFLAGS} -O3 -I../include $(SDL_CFLAGS) $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS) $(SUN_INCLUDE)
#-DBRIDGE_DEBUG_

LIBS=$(SDL_LIBS) $(XLIBS) -lpthread -lm -ldl $(QUANTA_LDFLAGS) $(GLEW_LIB) $(SUN_LIBS) $(SAGE_LDFLAGS)


COMMON_OBJ= misc.o sageMessage.o sageRect.o streamInfo.o sageBlock.o sageBuf.o sageBlockPool.o sageFrame.o sageBlockPartition.o

FSM_OBJ=fsManager.o fsServer.o fsCore.o sageVirtualDesktop.o \
	tileConfig.o displayInstance.o fsMain.o

RCV_OBJ=sageDisplayManager.o fsClient.o sageSync.o sageDisplay.o pixelDownloader.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o sageReceiver.o \
	sageDraw.o sageDrawObject.o overlayPointer.o overlayApp.o sageBlockQueue.o \
	sageSharedData.o sageEvent.o sdlSingleContext.o \
	appleMultiContext.o

ifdef AUDIO
ARCV_OBJ=sageAudioManager.o fsClient.o sageSync.o sageEvent.o \
	sageReceiver.o sageAudioReceiver.o sageBlockQueue.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o \
        sageAudioCircBuf.o sageAudio.o sageAudioModule.o sageAudioSync.o \
	audioFileReader.o audioFileWriter.o audioFormatManager.o \
	audioConverter.o wavConverter.o
endif

BRIDGE_OBJ=sageBridge.o sageAudioBridge.o messageInterface.o sageSync.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o sageSharedData.o \
	appInstance.o sageStreamer.o bridgeStreamer.o sageBlockQueue.o \
	sageEvent.o sageConfig.o sageReceiver.o sageDoubleBuf.o

SAIL_OBJ= sail.o envInterface.o fsClient.o sageSync.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o \
	sageDoubleBuf.o sageConfig.o sageStreamer.o sageBlockStreamer.o

ifdef AUDIO
SAIL_OBJ+=  sageAudioCircBuf.o sageAudio.o sageAudioModule.o sageAudioSync.o \
   audioFileReader.o audioFileWriter.o audioFormatManager.o sageAppAudio.o \
	audioConverter.o wavConverter.o sageAudioStreamer.o
endif


DIRT= *.o core core.* *.so $(PROJ_EXE)
PROJ_EXE=fsManager sageDisplayManager uiConsole $(FS_CONSOLE) sageBridge bridgeConsole
ifdef AUDIO
PROJ_EXE+=sageAudioManager
endif

build: $(PROJ_EXE) $(SAIL_LIB)

install: build
	cp $(SAIL_LIB) ../lib
	for i in $(PROJ_EXE); do cp -f $$i ../bin/; done

clean:
	rm -f $(DIRT)
	cd ../bin; rm -f $(PROJ_EXE)
	cd ../lib; rm -f $(SAIL_LIB); cd ../src


fsManager: $(COMMON_OBJ) $(FSM_OBJ)
	$(COMPILER) $(COMMON_OBJ) $(FSM_OBJ) $(LIBS) -o fsManager


sageDisplayManager: $(COMMON_OBJ) $(RCV_OBJ)
	$(COMPILER) $(COMMON_OBJ) $(RCV_OBJ) $(LIBS) -o sageDisplayManager

ifdef AUDIO
sageAudioManager: $(COMMON_OBJ) $(ARCV_OBJ)
	$(COMPILER) $(COMMON_OBJ) $(ARCV_OBJ) $(LIBS) $(PAUDIO_LIB) -o sageAudioManager
endif

sageBridge: $(COMMON_OBJ) $(BRIDGE_OBJ)
	$(COMPILER) $(COMMON_OBJ) $(BRIDGE_OBJ) $(LIBS) -o sageBridge

uiConsole: uiConsole.o fsClient.o suil.o sageMessage.o misc.o 
	$(COMPILER) uiConsole.o fsClient.o suil.o sageMessage.o misc.o $(LIBS) -o uiConsole

bridgeConsole: bridgeConsole.o messageInterface.o sageMessage.o misc.o 
	$(COMPILER) bridgeConsole.o messageInterface.o sageMessage.o misc.o $(LIBS) -o bridgeConsole

fsConsole: fsConsole.o fsClient.o suil.o  sageMessage.o misc.o
	$(COMPILER) fsConsole.o fsClient.o suil.o sageMessage.o misc.o $(LIBS) $(READLINE_LIB) -o fsConsole

ifdef AUDIO
$(SAIL_LIB): $(COMMON_OBJ) $(SAIL_OBJ)
	$(COMPILER) $(SHLD_FLAGS) $(FONT_LIB) $(COMMON_OBJ) $(SAIL_OBJ) $(LIBS) $(PAUDIO_LIB) -o $(SAIL_LIB)
else
$(SAIL_LIB): $(COMMON_OBJ) $(SAIL_OBJ)
	$(COMPILER) $(SHLD_FLAGS) $(FONT_LIB) $(COMMON_OBJ) $(SAIL_OBJ) $(LIBS) -o $(SAIL_LIB)
endif

.cpp.o :
	$(COMPILER) $(MYFLAGS) -g -c -o $@ $<

fsConsole.o: fsConsole.cpp
	$(COMPILER) $(MYFLAGS) $(READLINE_CFLAGS) -c fsConsole.cpp

