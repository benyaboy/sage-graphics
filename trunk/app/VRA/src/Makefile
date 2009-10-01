##############################################################################
# Volume Rendering Application
# Copyright (C) 2006-2007 Nicholas Schwarz
#
# This software is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either Version 2.1 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser Public License along
# with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##############################################################################

MPI_FLAGS = -I/opt/mpich/ch-p4/include
MPI_LIBS = -L/opt/mpich/ch-p4/lib64 -lmpich

SDL_FLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`

SAGE_CFLAGS = -I./../../../include
SAGE_LIBS = -L./../../../lib -lsail

CXX_FLAGS = $(MPI_FLAGS) $(SDL_FLAGS) -Wno-deprecated $(SAGE_CFLAGS) -D SAGE
CXX_LIBS = $(MPI_LIBS) $(SDL_LIBS) -lGL -lCg -lCgGL -lglut $(SAGE_LIBS)

CXX_FLAGS_CO = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

CXX_FLAGS_UI = `fltk-config --cxxflags --use-gl` -Wno-deprecated -D SAGE
CXX_LIBS_UI = -lGL -L/usr/lib64 -lglut `fltk-config --ldflags --use-gl` -lpthread

CXX = g++

default: VRA VRA_UI ScriptGenerator

VRA: main.o                               \
     CacheRAM.o                           \
     CacheRAMNode.o                       \
     CacheVRAM.o                          \
     CacheVRAMNode.o                      \
     Communicator.o                       \
     CommunicatorAbstractCommand.o        \
     CommunicatorConcreteCommand.o        \
     DataPacker.o                         \
     FrustumCalculator.o                  \
     FrustumPlanes.o                      \
     Log.o                                \
     Master.o                             \
     Matrix.o                             \
     Octree.o                             \
     OctreeNode.o                         \
     OctreeNodeQueue.o                    \
     Slave.o                              \
     SynchronizerClient.o                 \
     SynchronizerServer.o                 \
     SynchronizerServerAbstractCommand.o  \
     SynchronizerServerConcreteCommand.o  \
     SynchronizerServerClientThreadInfo.o \
     SynchronizerServerListenThreadInfo.o \
     Trackball.o                          \
     UserInterfaceServer.o                \
     UserInterfaceServerAbstractCommand.o \
     UserInterfaceServerConcreteCommand.o \
     Vertex.o                             \
     VolumeRenderer.o                     \
     VolumeRendererCommand.o              \
     VolumeRendererShell.o                \
     VolumeRendererThreadInfo.o           \
     Makefile
	$(CXX) $(CXX_FLAGS) -o VRA main.o                               \
                                   CacheRAM.o                           \
                                   CacheRAMNode.o                       \
                                   CacheVRAM.o                          \
                                   CacheVRAMNode.o                      \
                                   Communicator.o                       \
                                   CommunicatorAbstractCommand.o        \
                                   CommunicatorConcreteCommand.o        \
                                   DataPacker.o                         \
                                   FrustumCalculator.o                  \
                                   FrustumPlanes.o                      \
                                   Log.o                                \
                                   Master.o                             \
                                   Matrix.o                             \
                                   Octree.o                             \
                                   OctreeNode.o                         \
                                   OctreeNodeQueue.o                    \
                                   Slave.o                              \
                                   SynchronizerClient.o                 \
                                   SynchronizerServer.o                 \
                                   SynchronizerServerAbstractCommand.o  \
                                   SynchronizerServerConcreteCommand.o  \
                                   SynchronizerServerClientThreadInfo.o \
                                   SynchronizerServerListenThreadInfo.o \
                                   Trackball.o                          \
                                   UserInterfaceServer.o                \
                                   UserInterfaceServerAbstractCommand.o \
                                   UserInterfaceServerConcreteCommand.o \
                                   Vertex.o                             \
                                   VolumeRenderer.o                     \
                                   VolumeRendererCommand.o              \
                                   VolumeRendererShell.o                \
                                   VolumeRendererThreadInfo.o           \
                                   $(CXX_LIBS)
	cp VRA ../bin
	cp ../shaders/Fragment.cg ../bin

VRA_UI: UI.o                                 \
        ColorTransferFunctionWidget.o        \
        ControlPoint.o                       \
        DataPacker.o                         \
        Fl_Color_Transfer_Function.o         \
        Fl_Connect.o                         \
        Fl_Dialog.o                          \
	Fl_Lambda.o                          \
        Fl_Load_Data.o                       \
        Fl_Main_Window.o                     \
        Fl_Opacity_Transfer_Function.o       \
        Fl_Overview.o                        \
        Fl_Slice_Frequency.o                 \
        Fl_Transfer_Function.o               \
        Matrix.o                             \
        OpacityTransferFunctionWidget.o      \
        Trackball.o                          \
        UserInterfaceClient.o                \
        UserInterfaceClientAbstractCommand.o \
        UserInterfaceClientConcreteCommand.o \
        Vertex.o                             \
        Makefile
	$(CXX) $(CXX_FLAGS_UI) -o VRA_UI UI.o                                 \
                                         ColorTransferFunctionWidget.o        \
                                         ControlPoint.o                       \
                                         DataPacker.o                         \
                                         Fl_Color_Transfer_Function.o         \
                                         Fl_Connect.o                         \
                                         Fl_Dialog.o                          \
                                         Fl_Lambda.o                          \
                                         Fl_Load_Data.o                       \
                                         Fl_Main_Window.o                     \
                                         Fl_Opacity_Transfer_Function.o       \
                                         Fl_Overview.o                        \
                                         Fl_Slice_Frequency.o                 \
                                         Fl_Transfer_Function.o               \
                                         Matrix.o                             \
                                         OpacityTransferFunctionWidget.o      \
                                         Trackball.o                          \
                                         UserInterfaceClient.o                \
                                         UserInterfaceClientAbstractCommand.o \
                                         UserInterfaceClientConcreteCommand.o \
                                         Vertex.o                             \
                                         $(CXX_LIBS_UI)
	cp VRA_UI ../bin

ScriptGenerator: ScriptGenerator.o   \
                 FrustumCalculator.o \
                 Matrix.o            \
                 Vertex.o            \
                 Makefile
	$(CXX) -o ScriptGenerator ScriptGenerator.o   \
                                  FrustumCalculator.o \
                                  Matrix.o            \
                                  Vertex.o
	cp ScriptGenerator ../bin

main.o: main.cpp Communicator.h Master.h Slave.h SynchronizerServer.h
	$(CXX) $(CXX_FLAGS) -c main.cpp

CacheRAM.o: CacheRAM.cpp CacheRAM.h CacheRAMNode.h OctreeNode.h
	$(CXX) $(CXX_FLAGS) -c CacheRAM.cpp

CacheRAMNode.o: CacheRAMNode.cpp CacheRAMNode.h OctreeNode.h
	$(CXX) $(CXX_FLAGS) -c CacheRAMNode.cpp

CacheVRAM.o: CacheVRAM.cpp CacheVRAM.h CacheVRAMNode.h OctreeNode.h
	$(CXX) $(CXX_FLAGS) -c CacheVRAM.cpp

CacheVRAMNode.o: CacheVRAMNode.cpp CacheVRAMNode.h OctreeNode.h
	$(CXX) $(CXX_FLAGS) -c CacheVRAMNode.cpp

ColorTransferFunctionWidget.o: ColorTransferFunctionWidget.cpp \
                               ColorTransferFunctionWidget.h
	$(CXX) $(CXX_FLAGS_UI) -c ColorTransferFunctionWidget.cpp

CommunicatorAbstractCommand.o: CommunicatorAbstractCommand.cpp \
                               CommunicatorAbstractCommand.h	
	$(CXX) $(CXX_FLAGS) -c CommunicatorAbstractCommand.cpp

CommunicatorConcreteCommand.o: CommunicatorConcreteCommand.cpp \
                               CommunicatorConcreteCommand.h   \
                               CommunicatorAbstractCommand.h
	$(CXX) $(CXX_FLAGS) -c CommunicatorConcreteCommand.cpp

Communicator.o: Communicator.cpp Communicator.h \
                CommunicatorAbstractCommand.h
	$(CXX) $(CXX_FLAGS) -c Communicator.cpp

ControlPoint.o: ControlPoint.cpp ControlPoint.h
	$(CXX) -c ControlPoint.cpp

DataPacker.o: DataPacker.cpp DataPacker.h
	$(CXX) -c DataPacker.cpp

Fl_Color_Transfer_Function.o: Fl_Color_Transfer_Function.cpp \
                              Fl_Color_Transfer_Function.h   \
                              ColorTransferFunctionWidget.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Color_Transfer_Function.cpp

Fl_Connect.o: Fl_Connect.cpp Fl_Connect.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Connect.cpp

Fl_Dialog.o: Fl_Dialog.cpp Fl_Dialog.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Dialog.cpp

Fl_Lambda.o: Fl_Lambda.cpp Fl_Lambda.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Lambda.cpp

Fl_Load_Data.o: Fl_Load_Data.cpp Fl_Load_Data.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Load_Data.cpp

Fl_Main_Window.o: Fl_Main_Window.cpp Fl_Main_Window.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Main_Window.cpp

Fl_Opacity_Transfer_Function.o: Fl_Opacity_Transfer_Function.cpp \
                                Fl_Opacity_Transfer_Function.h   \
                                OpacityTransferFunctionWidget.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Opacity_Transfer_Function.cpp

Fl_Overview.o: Fl_Overview.cpp Fl_Overview.h Matrix.h Trackball.h Vertex.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Overview.cpp

Fl_Transfer_Function.o: Fl_Transfer_Function.cpp Fl_Transfer_Function.h \
                        Fl_Color_Transfer_Function.h                    \
                        Fl_Opacity_Transfer_Function.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Transfer_Function.cpp

Fl_Slice_Frequency.o: Fl_Slice_Frequency.cpp Fl_Slice_Frequency.h
	$(CXX) $(CXX_FLAGS_UI) -c Fl_Slice_Frequency.cpp

FrustumCalculator.o: FrustumCalculator.cpp FrustumCalculator.h
	$(CXX) $(CXX_FLAGS) -c FrustumCalculator.cpp

FrustumPlanes.o: FrustumPlanes.cpp FrustumPlanes.h
	$(CXX) $(CXX_FLAGS) -c FrustumPlanes.cpp

Log.o: Log.cpp Log.h
	$(CXX) $(CXX_FLAGS) -c Log.cpp

Master.o: Master.cpp Master.h                  \
          Communicator.h                       \
          FrustumCalculator.h                  \
          Matrix.h                             \
          Octree.h                             \
          SynchronizerServer.h                 \
	  Trackball.h                          \
          UserInterfaceServer.h                \
          UserInterfaceServerAbstractCommand.h \
          Vertex.h
	$(CXX) $(CXX_FLAGS) -c Master.cpp

Matrix.o: Matrix.cpp Matrix.h
	$(CXX) -c Matrix.cpp

Octree.o: Octree.cpp Octree.h OctreeNode.h
	$(CXX) $(CXX_FLAGS) -c Octree.cpp

OctreeNode.o: OctreeNode.cpp OctreeNode.h Vertex.h
	$(CXX) $(CXX_FLAGS) -c OctreeNode.cpp

OctreeNodeQueue.o: OctreeNodeQueue.cpp OctreeNodeQueue.h OctreeNode.h
	$(CXX) $(CXX_FLAGS) -c OctreeNodeQueue.cpp

OpacityTransferFunctionWidget.o: OpacityTransferFunctionWidget.cpp \
                                 OpacityTransferFunctionWidget.h
	$(CXX) $(CXX_FLAGS_UI) -c OpacityTransferFunctionWidget.cpp

ScriptGenerator.o: ScriptGenerator.cpp \
                   FrustumCalculator.h \
                   Matrix.h            \
                   Vertex.h
	$(CXX) -c ScriptGenerator.cpp

Slave.o: Slave.cpp Slave.h             \
         Communicator.h                \
         CommunicatorAbstractCommand.h \
         Octree.h                      \
         VolumeRendererShell.h
	$(CXX) $(CXX_FLAGS) -c Slave.cpp

SynchronizerServerAbstractCommand.o: SynchronizerServerAbstractCommand.cpp \
                                     SynchronizerServerAbstractCommand.h
	$(CXX) $(CXX_FLAGS) -c SynchronizerServerAbstractCommand.cpp

SynchronizerServerConcreteCommand.o: SynchronizerServerConcreteCommand.cpp \
                                     SynchronizerServerConcreteCommand.h
	$(CXX) $(CXX_FLAGS) -c SynchronizerServerConcreteCommand.cpp

SynchronizerClient.o: SynchronizerClient.cpp SynchronizerClient.h
	$(CXX) $(CXX_FLAGS) -c SynchronizerClient.cpp

SynchronizerServerClientThreadInfo.o: SynchronizerServerClientThreadInfo.cpp \
                                      SynchronizerServerClientThreadInfo.h
	$(CXX) $(CXX_FLAGS) -c SynchronizerServerClientThreadInfo.cpp

SynchronizerServer.o: SynchronizerServer.cpp SynchronizerServer.h
	$(CXX) $(CXX_FLAGS) -c SynchronizerServer.cpp

SynchronizerServerListenThreadInfo.o: SynchronizerServerListenThreadInfo.cpp \
                                      SynchronizerServerListenThreadInfo.h
	$(CXX) $(CXX_FLAGS) -c SynchronizerServerListenThreadInfo.cpp

Trackball.o: Trackball.cpp Trackball.h
	$(CXX) -c Trackball.cpp

UI.o: UI.cpp                               \
      DataPacker.h                         \
      Fl_Connect.h                         \
      Fl_Dialog.h                          \
      Fl_Load_Data.h                       \
      Fl_Main_Window.h                     \
      Fl_Overview.h                        \
      Fl_Transfer_Function.h               \
      Trackball.h                          \
      UserInterfaceClient.h                \
      UserInterfaceClientConcreteCommand.h \
      Vertex.h
	$(CXX) $(CXX_FLAGS_UI) -c UI.cpp

UserInterfaceClientAbstractCommand.o: UserInterfaceClientAbstractCommand.cpp \
                                      UserInterfaceClientAbstractCommand.h
	$(CXX) $(CXX_FLAGS_UI) -c UserInterfaceClientAbstractCommand.cpp

UserInterfaceClientConcreteCommand.o: UserInterfaceClientConcreteCommand.cpp \
                                      UserInterfaceClientConcreteCommand.h   \
                                      DataPacker.h                           \
                                      UserInterfaceClientAbstractCommand.h
	$(CXX) $(CXX_FLAGS_UI) -c UserInterfaceClientConcreteCommand.cpp

UserInterfaceClient.o: UserInterfaceClient.cpp UserInterfaceClient.h \
                       DataPacker.h                                  \
                       UserInterfaceClientAbstractCommand.h
	$(CXX) $(CXX_FLAGS_UI) -c UserInterfaceClient.cpp

UserInterfaceServerAbstractCommand.o: UserInterfaceServerAbstractCommand.cpp \
                                      UserInterfaceServerAbstractCommand.h
	$(CXX) $(CXX_FLAGS) -c UserInterfaceServerAbstractCommand.cpp

UserInterfaceServerConcreteCommand.o: UserInterfaceServerConcreteCommand.cpp \
                                      UserInterfaceServerConcreteCommand.h   \
                                      UserInterfaceServerAbstractCommand.h
	$(CXX) $(CXX_FLAGS) -c UserInterfaceServerConcreteCommand.cpp

UserInterfaceServer.o: UserInterfaceServer.cpp UserInterfaceServer.h \
                       DataPacker.h                                  \
                       UserInterfaceServerAbstractCommand.h
	$(CXX) $(CXX_FLAGS) -c UserInterfaceServer.cpp

Vertex.o: Vertex.cpp Vertex.h
	$(CXX) -c Vertex.cpp

VolumeRendererCommand.o: VolumeRendererCommand.cpp VolumeRendererCommand.h
	$(CXX) $(CXX_FLAGS) -c VolumeRendererCommand.cpp

VolumeRenderer.o: VolumeRenderer.cpp VolumeRenderer.h \
                  CacheRAM.h                          \
                  CacheVRAM.h                         \
                  FrustumPlanes.h                     \
                  Octree.h                            \
                  OctreeNodeQueue.h                   \
                  VolumeRendererCommand.h
	$(CXX) $(CXX_FLAGS) -c VolumeRenderer.cpp

VolumeRendererShell.o: VolumeRendererShell.cpp VolumeRenderer.h \
                       Octree.h                                 \
                       VolumeRenderer.h                         \
                       VolumeRendererThreadInfo.h
	$(CXX) $(CXX_FLAGS) -c VolumeRendererShell.cpp

VolumeRendererThreadInfo.o: VolumeRendererThreadInfo.cpp \
                            VolumeRendererThreadInfo.h   \
                            Octree.h
	$(CXX) $(CXX_FLAGS) -c VolumeRendererThreadInfo.cpp

clean:
	rm -rf *.o *~ VRA VRA_UI ScriptGenerator
	rm -rf ../bin/VRA             \
               ../bin/Fragment.cg     \
               ../bin/VRA_UI          \
               ../bin/ScriptGenerator
