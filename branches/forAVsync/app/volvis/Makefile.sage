#Makefile for a Geowall type configuration
#ie the volvis runs on the geowall and the transfer-fn UI runs on a remote machine
#for evl
#QUANTA_HOME=${HOME}/Q2
QUANTA_HOME=/usr/local
#for ncmir
#QUANTA_HOME=/home/cavern/Q2
#for igpp
#QUANTA_HOME=/home/luc/Q2

INCLUDE_DIR 	= -I/usr/local/include -I../OptiStore   
LIBRARY_DIR 	= -L/usr/lib -L/usr/X11R6/lib64 -L${QUANTA_HOME}/lib -Wl,-rpath ${QUANTA_HOME}/lib -L../../lib
LIBRARIES 	= -lCgGL -lCg -lglut -lGLU -lGL -lXi -lXmu -lXext -lX11 -lm -lquanta_64 -lsail
CXXFLAGS 	= -O3 -DADDCGGL -DV_NETTF -DV_NETDATA -I${QUANTA_HOME}/include/QUANTA -I${QUANTA_HOME}/include $(COMPILER_FLAGS) $(INCLUDE_DIR)  -DV_SAGE -I../../include
CXXINCLUDE	= $(QUANTA_CFLAGS) 
COMPILER 	= g++ -O3

VOLOBJ_FILES 	= CCmdLine.o glUE.o main.o Trackball.o vCG.o vCut.o mmap.o vFPSText.o vNet.o vSliceView.o \
		 vPrimitive.o vRenderer.o vTF.o vUI.o vVolume.o vOptiVolume.o vFileVolume.o vGeometry.o

OBJ_FILES	= $(VOLOBJ_FILES) directions.o caDataManagerClient.o

EXEC 		= volvis

$(EXEC):	$(OBJ_FILES)
		$(COMPILER) $(COMPILER_FLAGS) -o $(EXEC) $(OBJ_FILES) \
			$(LIBRARY_DIR) $(LIBRARIES) -lpthread


clean:
		\rm -f $(VOLOBJ_FILES) $(EXEC)

