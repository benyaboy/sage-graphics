#
#	Makefile for sage
#	

include ../config.mk

# SAGE global revision number from SVN
SAGE_REVISION := -DSAGE_REVISION=$(shell svnversion -n ..)


QUANTA_DIR=../QUANTA
QUANTA_CFLAGS=-I${QUANTA_DIR}/include
QUANTA_LDFLAGS=-L../lib -lquanta

MYFLAGS=${SAGE_CFLAGS} -O3 -I../include $(SDL_CFLAGS) $(QUANTA_CFLAGS) $(GLEW_CFLAGS) $(GLSL_YUV_DEFINE) $(PORTAUDIO_CFLAGS) $(SUN_INCLUDE) ${SAGE_REVISION}
#-DDEBUG_RECEIVER
#-DBRIDGE_DEBUG_

ifdef AUDIO
LIBS=-L../lib $(PAUDIO_LIB) $(SDL_LIBS) $(XLIBS) -lpthread -lm -ldl $(QUANTA_LDFLAGS) $(GLEW_LIB) $(SUN_LIBS) $(SAGE_LDFLAGS)
else
LIBS=-L../lib $(SDL_LIBS) $(XLIBS) -lpthread -lm -ldl $(QUANTA_LDFLAGS) $(GLEW_LIB) $(SUN_LIBS) $(SAGE_LDFLAGS)
endif


COMMON_OBJ= misc.o sageMessage.o sageRect.o streamInfo.o sageBlock.o sageBuf.o sageBlockPool.o sageFrame.o sageBlockPartition.o

FSM_OBJ=fsManager.o fsServer.o fsCore.o sageVirtualDesktop.o \
	tileConfig.o displayInstance.o fsMain.o

RCV_OBJ=sageDisplayManager.o fsClient.o sageSync.o sageDisplay.o pixelDownloader.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o sageReceiver.o \
	sageDraw.o sageDrawObject.o overlayPointer.o overlayApp.o sageBlockQueue.o \
	sageSharedData.o sageEvent.o sdlSingleContext.o
ifeq ($(MACHINE), Darwin)
RCV_OBJ+=appleMultiContext.o
endif


ifeq ($(MACHINE), Darwin)
RCV_OBJ += SDLMain.o
endif


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

ifdef AUDIO
SAIL_OBJ= sail.o envInterface.o fsClient.o sageSync.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o \
	sageDoubleBuf.o sageConfig.o sageStreamer.o sageBlockStreamer.o \
	sageAudioCircBuf.o sageAudio.o sageAudioModule.o sageAudioSync.o \
	audioFileReader.o audioFileWriter.o audioFormatManager.o sageAppAudio.o \
	audioConverter.o wavConverter.o sageAudioStreamer.o
else
SAIL_OBJ= sail.o envInterface.o fsClient.o sageSync.o \
	streamProtocol.o sageTcpModule.o sageUdpModule.o \
	sageDoubleBuf.o sageConfig.o sageStreamer.o sageBlockStreamer.o

endif


DIRT= *.o core core.* *.so $(PROJ_EXE)

ifndef SAIL_ONLY
ifdef AUDIO
PROJ_EXE=fsManager sageDisplayManager sageAudioManager uiConsole sageBridge bridgeConsole $(FS_CONSOLE) 
else
PROJ_EXE=fsManager sageDisplayManager uiConsole sageBridge bridgeConsole $(FS_CONSOLE)
endif
endif

build: $(PROJ_EXE) $(SAIL_LIB)

install: build
	cp $(SAIL_LIB) ../lib
	for i in $(PROJ_EXE); do cp -f $$i ../bin/; done
ifeq ($(MACHINE), Darwin)
	/bin/rm -fr ../bin/sageDisplayManager.app
	/bin/cp -fr sageDisplayManager.app ../bin
endif


clean:
	rm -f $(DIRT)
	cd ../bin; rm -f $(PROJ_EXE)
	cd ../lib; rm -f $(SAIL_LIB); cd ../src

SDLMain.o: SDLMain.m
	$(COMPILER) $(SAGE_LDFLAGS) -m32 -c SDLMain.m

fsManager: $(COMMON_OBJ) $(FSM_OBJ)
	$(COMPILER) ${MYFLAGS} $(COMMON_OBJ) $(FSM_OBJ) $(LIBS) -o fsManager


sageDisplayManager: $(COMMON_OBJ) $(RCV_OBJ)
	$(COMPILER) $(MYFLAGS) $(COMMON_OBJ) $(RCV_OBJ) $(LIBS) -o sageDisplayManager
ifeq ($(MACHINE), Darwin)
	/bin/rm -fr sageDisplayManager.app
	mkdir -p sageDisplayManager.app/Contents/MacOS
	mkdir -p sageDisplayManager.app/Contents/Resources
	echo "APPL????" > sageDisplayManager.app/Contents/PkgInfo
	install sageDisplayManager sageDisplayManager.app/Contents/MacOS/
	install Info.plist sageDisplayManager.app/Contents/
	touch sageDisplayManager.app/Contents/Resources/empty.lproj
endif


ifdef AUDIO
sageAudioManager: $(COMMON_OBJ) $(ARCV_OBJ)
	$(COMPILER) $(MYFLAGS) $(COMMON_OBJ) $(ARCV_OBJ) $(LIBS) -o sageAudioManager
endif

sageBridge: $(COMMON_OBJ) $(BRIDGE_OBJ)
	$(COMPILER) $(MYFLAGS) $(COMMON_OBJ) $(BRIDGE_OBJ) $(LIBS) -o sageBridge

uiConsole: uiConsole.o fsClient.o suil.o sageMessage.o misc.o 
	$(COMPILER) ${SAGE_CFLAGS} uiConsole.o fsClient.o suil.o sageMessage.o misc.o $(LIBS) -o uiConsole

bridgeConsole: bridgeConsole.o messageInterface.o sageMessage.o misc.o 
	$(COMPILER) ${SAGE_CFLAGS} bridgeConsole.o messageInterface.o sageMessage.o misc.o $(LIBS) -o bridgeConsole

fsConsole: fsConsole.o fsClient.o suil.o  sageMessage.o misc.o
	$(COMPILER) ${SAGE_CFLAGS} fsConsole.o fsClient.o suil.o sageMessage.o misc.o $(LIBS) $(READLINE_LIB) -o fsConsole

$(SAIL_LIB): $(COMMON_OBJ) $(SAIL_OBJ)
	$(COMPILER) $(SHLD_FLAGS) $(COMMON_OBJ) $(SAIL_OBJ) $(LIBS) -o $(SAIL_LIB)

.cpp.o :
	$(COMPILER) $(MYFLAGS) -c -o $@ $<

fsConsole.o: fsConsole.cpp
	$(COMPILER) $(MYFLAGS) $(READLINE_CFLAGS) -c fsConsole.cpp

