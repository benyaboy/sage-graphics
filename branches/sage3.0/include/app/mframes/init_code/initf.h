
/******************************************************************************
 * SAGE - Scalable Adaptive Graphics Environment
 *
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about SAGE to http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/
#ifndef ANIMVIEWER_H
#define ANIMVIEWER_H

//#include "vtkXOpenGLOffScreenRenderWindow.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

using namespace std;

/************************/

#include "tile_config.h"
#include "file_parsers.h"

// SAGE related headers
#include <sail.h>
#include <misc.h>

/************************/


/* MPI */
#include <mpi.h>

/* VTK */
#include <vtk/vtkActor.h>
#include <vtk/vtkCamera.h>
#include <vtk/vtkLight.h>
#include <vtk/vtkRenderWindow.h>
#include <vtk/vtkRenderer.h>
#include <vtk/vtkImageImport.h>
#include <vtk/vtkTexture.h>
#include <vtk/vtkTransformTextureCoords.h>
#include <vtk/vtkTextureMapToPlane.h>
#include <vtk/vtkPlaneSource.h>
#include <vtk/vtkMergeFilter.h>
#include <vtk/vtkPolyDataMapper.h>
#include <vtk/vtkImageFlip.h>
#include <vtk/vtkProperty.h>
#include <vtk/vtkSphereSource.h>
#include <vtk/vtkTextSource.h>
#include <vtk/vtkFrustumCoverageCuller.h>
#include <vtk/vtkTextActor.h>
#include <vtk/vtkTextProperty.h>

#include <vtk/vtkSphereSource.h>
#include <vtk/vtkCubeSource.h>
#include <vtk/vtkConeSource.h>
#include <vtk/vtkJPEGReader.h>
#include <vtk/vtkImageMapper.h>
#include <vtk/vtkActor2D.h>

/* QUANTA */
#include <QUANTA/QUANTAinit.hxx>
#include <QUANTA/QUANTAnet_tcp_c.hxx>
#include <QUANTA/QUANTAnet_datapack_c.hxx>

#define PANLEFT "panleft"
#define PANRIGHT "panright"
#define PANUP "panup"
#define PANDOWN "pandown"
#define BROWSERIGHT "browseright"
#define BROWSELEFT "browseleft"
#define ZOOMIN "zoomin"
#define ZOOMOUT "zoomout"
#define SELECT "select"
#define RELEASE "release"
#define PRESS "press"
#define SLIDER1 "slider1"
#define SLIDER2 "slider2"
#define SLIDER3 "slider3"
#define QUIT "quit"
#define CENTER "center"

int PORT= 5678;
#define TRANSMISSION_SIZE 16

#define ZIN 1
#define ZOUT 0

QUANTAnet_tcpServer_c* server;
QUANTAnet_tcpClient_c* client;
char* recmsg;

double CamPosition[3];
double Fpoint[3];
double PanAmount;

double Zoom = 7;
double ZoomAmount;

double StartingZ;

//SAGE STUFF
int winWidth, winHeight;
sail sageInf;

int rank;
int size;

int xWindowPos = 0;
int yWindowPos = 0;

vtkRenderer* renA = vtkRenderer::New();
vtkRenderer* renB = vtkRenderer::New();
vtkRenderWindow* renWin = vtkRenderWindow::New();
vtkLight* light = vtkLight::New();
vtkFrustumCoverageCuller* culler = vtkFrustumCoverageCuller::New();
vtkTextActor *text = vtkTextActor::New();

vtkConeSource *cone = vtkConeSource::New();
vtkCubeSource *cube = vtkCubeSource::New();
vtkSphereSource *sphere = vtkSphereSource::New();
vtkPolyDataMapper *sphereMapper = vtkPolyDataMapper::New();
vtkActor *pointer = vtkActor::New();

vtkJPEGReader *splash = vtkJPEGReader::New();
vtkImageMapper *splashMapper = vtkImageMapper::New();
vtkActor2D *splashScreen = vtkActor2D::New();

#define PANSUBDIVISIONS 10

#define GRAYSCALE 1
#define RGB 3
#define RGBA 4
int MODE;

int start = 0;
int end = 0;

#define LEFT -1
#define RIGHT 1
#define UP -2
#define DOWN 2
#define ACCEPT 0

#define X 0
#define Y 1

int CurrentLevel = 0;

//#define DEBUG

struct ZBound {
	double bound;
};

struct Viewable {
	long level;
	bool virgin;
	unsigned char*** pixels;
};

struct Point {
	long l;
	long c;
	long r;
};

struct Geometry {
	vtkImageImport *importer;
	vtkTexture* texture;
	vtkPolyDataMapper *mapper;
	vtkActor* actor;
	vtkPlaneSource *plane;
	vtkImageFlip *flip;

	vtkTextSource *textSource;
	vtkPolyDataMapper *textMapper;
	vtkActor *textActor;


	int fd;
};

struct Info {

	double scale[2];
	double position[3];
	double depth[1];
};





class AnimViewer {


public:

	AnimViewer();
	~AnimViewer();

	string IntToString(long);
	void CopyBuffer(unsigned char*, unsigned char*);
	long GetNumOfCols(long, long);
	long GetNumOfRows(long, long);
	long GetLevelBlockWidth(long);
	long GetLevelBlockHeight(long);
	long GetLevelNumOfCols(long);
	long GetLevelNumOfRows(long);
	void InitViewer(char*);
	void GetAnimFiles();
	unsigned char* GetPixelsFromFile(long, long, long);
	unsigned char* MMap(long, long, long);
	void InitRendering();
	void SetUpPipeline();
	void UpdateBlock(long, long, long);
	void ImportQueryWindowPixels();
	void FindStartingQueryWindow();
	void GivePixelsToVTK();
	void QueryWindowPanRight();
	void QueryWindowPanLeft();
	void QueryWindowPanUp();
	void QueryWindowPanDown();
	void QueryWindowZoomIn();
	void QueryWindowZoomOut();
	void PositionAndScaleBlocks();
	void ComputeStartingBlockSize();
	double GetRowPositionAtLevel(long, long);
	double GetColPositionAtLevel(long, long);
	double GetBlockWidthAtLevel(long);
	double GetBlockHeightAtLevel(long);
	void AddBlock(long, long, long);
	void RemoveBlock(long, long, long);

	void AdjustLevelMovement(int, int);
	int AdjustZoomMovement(int);
	long GetQueryWindowLevel();
	int BlockOutOfBounds(long, long, long, long);

	void ZoomIn();
	void ZoomOut();
	void PanLeft();
	void PanRight();
	void PanUp();
	void PanDown();
	void UpdateCamera(double[], double[]);
	double GetCenterX();
	double GetCenterY();
	void RepositionCamera();

	void RemoveRegion(long, long, long, long, long);
	void AddRegion(long, long, long, long, long);

	void GetCamProjection(double, double);
	void BuildMainQueryWindow();

	void BringUpStartingImage();

	bool NeedToUpdateMainWindow();

	void CleanUpMainWindow();

	void SetTimeStep(int);

	void CreateZoomBounds();

	void GetZProjection(double);

	void SetCurrentLevel(int);

	void SetInitialCamPosition();


private:

	int BlockDimensions[2];
	int GlobalDimensions[2];


	int Colorspace;

	string AnimDir;

	int NumOfLevels;

	int StartingLevel;

	unsigned char* pixelbuff;

	string*** AnimFiles;

	Geometry*** Block;

	Viewable QueryWindow;
	Info** BlockInfo;
	double BlockInitSize[2];

	Point CamProjection;

	ZBound* ZoomBounds;
	int NumOfZBounds;

	unsigned char* Blank;


	int NumOfCols;
	int NumOfRows;
};


#endif
