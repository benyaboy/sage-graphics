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
#include "initf.h"

//#define SAGE

#define HORIZONTAL 0
#define VERTICAL 1


AnimViewer animviewer;

double PointerPosition[2];

int uiaction;
double pos[3];

double cw0, cw1, ch0, ch1;

int window_w, window_h;

// default is 1
int ScreensPerNode = 1;



char* hostname;

string appname;



bool SAGE;
string sageStr;

AnimViewer::AnimViewer() {

	QueryWindow.virgin = true;

	text->SetInput("Loading!!!");
	text->SetDisplayPosition(300, 350);
	text->GetTextProperty()->SetFontFamilyToArial();
	text->GetTextProperty()->SetFontSize(150);
	text->GetTextProperty()->SetColor(0.0,0.0,0.0);

	sphere->SetThetaResolution(32);
	sphere->SetPhiResolution(32);

	sphereMapper->SetInput(sphere->GetOutput());
	pointer->SetMapper(sphereMapper);
	pointer->GetProperty()->SetColor(1,0,0);
	pointer->GetProperty()->SetOpacity(0.6);
	pointer->GetProperty()->BackfaceCullingOn();

}

AnimViewer::~AnimViewer() {

	delete [] this->Blank;
	this->Blank = NULL;

}

string AnimViewer::IntToString(long num) {

	ostringstream o;
	if (!(o << num))
		printf("Error in conversion from int to string \n");

	return o.str();

}

long AnimViewer::GetNumOfCols(long wtotal, long wblock) {


	return (long) ceil(double(wtotal) / double(wblock));

}

long AnimViewer::GetNumOfRows(long wtotal, long wblock) {

	return (long) ceil(double(wtotal) / double(wblock));

}

void AnimViewer::GetAnimFiles() {

	long cols = this->NumOfCols;
	long rows = this->NumOfRows;

	string dirname, filename;

	this->AnimFiles = new string**[this->NumOfLevels];

	for(int i=0; i<this->NumOfLevels; i++) {

		this->AnimFiles[i] = new string*[cols];

		dirname.clear();
		dirname += "/frame";
		dirname += this->IntToString(i);

		for(int c=0 ; c<cols; c++) {

			this->AnimFiles[i][c] = new string[rows];

			for(int r=0; r<rows; r++) {

				filename.clear();
				filename += dirname;
				filename += "/b_";
				filename += this->IntToString(c);
				filename += "_";
				filename += this->IntToString(r);


				this->AnimFiles[i][c][r] += filename;

				/*
				printf("%i %i %i ... ", i, c, r);
				printf("%s\n", this->AnimFiles[i][c][r].c_str());
				*/
			}
		}
	}

	printf("Created file locator.\n");

}

unsigned char* AnimViewer::MMap(long l, long c, long r) {
	string filename;
	filename += AnimDir;
	filename += this->AnimFiles[l][c][r];

	size_t len = this->BlockDimensions[0] * this->BlockDimensions[1] * MODE;

	// Open file
	this->Block[l][c][r].fd = open(filename.c_str(), O_RDONLY);

	if(this->Block[l][c][r].fd < 0) {
		printf("File does not exist. Could not map.\n");
		printf("Assigning a blank tile...\n");

		this->pixelbuff = this->Blank;
	}
	else {
		//printf("Mapping %s.\n", filename.c_str());
		this->pixelbuff = (unsigned char*) mmap(0,len,PROT_READ,MAP_SHARED,
								this->Block[l][c][r].fd,0);

		madvise(0, len, MADV_SEQUENTIAL);
	}

	// Close file
	close(this->Block[l][c][r].fd);

	return this->pixelbuff;
}

/* Takes the filename and returns its pixels content */
unsigned char* AnimViewer::GetPixelsFromFile(long l, long c, long r) {
	string filename;
	filename += AnimDir;
	filename += this->AnimFiles[l][c][r];

	this->pixelbuff =
			new unsigned char[this->BlockDimensions[0] * this->BlockDimensions[1] * MODE];

	//fprintf(stderr, "Opening [%s]\n", filename.c_str() );

	FILE* fptr = fopen(filename.c_str(),"rb");
	//setvbuf(fptr, NULL, _IOFBF, 3*1024*1024);

	if(!fptr) {
		if(rank != 0)
			printf("Can't open %s\n", filename.c_str());

		this->pixelbuff = this->Blank;
	}
	else {
		//printf("Opening file %s\n", name.c_str());
		fread(this->pixelbuff, this->BlockDimensions[0] *
				this->BlockDimensions[1] * MODE, sizeof(unsigned char),
					fptr);

		fclose(fptr);
	}
	//fprintf(stderr, "Closing [%s]\n", filename.c_str() );

	return this->pixelbuff;
}

void AnimViewer::InitRendering() {

	long cols = this->NumOfCols;
	long rows = this->NumOfRows;


	this->Block = new Geometry**[this->NumOfLevels];

	for(int i=0 ; i<this->NumOfLevels; i++) {

	this->Block[i] = new Geometry*[cols];

	for(int c=0 ; c<cols; c++) {

		this->Block[i][c] = new Geometry[rows];

		for(int r=0; r<rows; r++) {

			this->Block[i][c][r].importer = vtkImageImport::New();
			this->Block[i][c][r].texture = vtkTexture::New();
			this->Block[i][c][r].actor = vtkActor::New();
			this->Block[i][c][r].plane = vtkPlaneSource::New();
			this->Block[i][c][r].mapper = vtkPolyDataMapper::New();
			this->Block[i][c][r].flip = vtkImageFlip::New();

			this->Block[i][c][r].textSource = vtkTextSource::New();
			this->Block[i][c][r].textMapper = vtkPolyDataMapper::New();
			this->Block[i][c][r].textActor = vtkActor::New();

			this->Block[i][c][r].importer->ReleaseDataFlagOn();
			this->Block[i][c][r].flip->ReleaseDataFlagOn();

		}
	}

	}


}

void AnimViewer::SetUpPipeline() {

	long cols = this->NumOfCols;
	long rows = this->NumOfRows;

	for(int i=0 ; i<this->NumOfLevels; i++) {

	for(int c=0 ; c<cols; c++) {

		for(int r=0; r<rows; r++) {

			this->Block[i][c][r].plane->SetResolution(1,1);

			this->Block[i][c][r].texture->RepeatOff();

			this->Block[i][c][r].texture->InterpolateOn();

			this->Block[i][c][r].mapper->SetInput(
					(vtkPolyData*) this->Block[i][c][r].plane->GetOutput());

			this->Block[i][c][r].actor->SetMapper(
					this->Block[i][c][r].mapper);

			this->Block[i][c][r].actor->GetProperty()->BackfaceCullingOn();

			this->Block[i][c][r].textSource->SetForegroundColor(1,0,0);
			this->Block[i][c][r].textSource->BackingOff();
			this->Block[i][c][r].textMapper->SetInput(
						this->Block[i][c][r].textSource->GetOutput());
			this->Block[i][c][r].textActor->SetMapper(this->Block[i][c][r].textMapper);
				this->Block[i][c][r].textActor->GetProperty()->SetOpacity(0.5);

		}
	}
	}

}

void AnimViewer::CopyBuffer(unsigned char* dest, unsigned char* origin) {

	for(int i=0; i <
		this->BlockDimensions[0] * this->BlockDimensions[1] * MODE; i++) {

		dest[i] = origin[i];
	}

}




void AnimViewer::ImportQueryWindowPixels() {

	long cols = this->NumOfCols;
	long rows = this->NumOfRows;

	if(QueryWindow.virgin == true) {
		this->QueryWindow.pixels = new unsigned char**[cols];

		for(int c = 0; c < cols; c++) {

			this->QueryWindow.pixels[c] = new unsigned char*[rows];
		}
	}
	else {
		for(int c=0; c < cols; c++) {

			for(int r=0; r < rows; r++) {

				munmap(this->QueryWindow.pixels[c][r],
					this->BlockDimensions[0] * this->BlockDimensions[1] * MODE);
				//delete [] this->QueryWindow.pixels[c][r];
				//this->QueryWindow.pixels[c][r] = NULL;
			}
		}
	}

	for(int c = 0; c < cols; c++ ) {
		for(int r = 0; r < rows; r++) {

				this->QueryWindow.pixels[c][r] = //GetPixelsFromFile(this->QueryWindow.level, c, r);
				this->MMap(this->QueryWindow.level, c, r);

				string label;
				label += IntToString(this->QueryWindow.level);
				label += " ";
				label += IntToString(c);
				label += " ";
				label += IntToString(r);

				this->Block[this->QueryWindow.level][c][r].textSource->SetText(label.c_str());
		}
	}

	QueryWindow.virgin = false;

	printf("IMPORTED PIXELS\n");

}

void AnimViewer::GivePixelsToVTK() {

	long l = this->QueryWindow.level;
	long cols = this->NumOfCols;
	long rows = this->NumOfRows;

	for(int c = 0; c < cols; c++ ) {
		for(int r = 0; r < rows; r++) {

			this->Block[l][c][r].importer->
							SetDataScalarTypeToUnsignedChar();
			this->Block[l][c][r].importer->
							SetNumberOfScalarComponents(MODE);
			this->Block[l][c][r].importer->
							SetDataOrigin(0,0,0);
			this->Block[l][c][r].importer->
							SetWholeExtent(0, this->BlockDimensions[0] - 1, 0,
											  this->BlockDimensions[1] - 1, 0, 0);
			this->Block[l][c][r].importer->
							SetDataExtentToWholeExtent();

			this->Block[l][c][r].importer->
							SetImportVoidPointer(this->QueryWindow.pixels[c][r]);

			this->UpdateBlock(l, c, r);

		}
	}

	this->AddRegion(l, 0, 0, cols, rows);

}

void AnimViewer::UpdateBlock(long l, long c, long r) {

	/* flip the image */
	this->Block[l][c][r].flip->
			SetInput(this->Block[l][c][r].importer->GetOutput());

	this->Block[l][c][r].flip->SetFilteredAxis(1);
	this->Block[l][c][r].flip->FlipAboutOriginOn();

	this->Block[l][c][r].texture->
			SetInput(this->Block[l][c][r].flip->GetOutput());

	this->Block[l][c][r].actor->
			SetTexture(this->Block[l][c][r].texture);
}

void AnimViewer::AddBlock(long l, long c, long r) {

	renA->AddActor(this->Block[l][c][r].actor);
	if(ScreensPerNode > 1)
		renB->AddActor(this->Block[l][c][r].actor);
#ifdef DEBUG
	renA->AddActor(this->Block[l][c][r].textActor);
#endif
}

void AnimViewer::RemoveBlock(long l, long c, long r) {

	renA->RemoveActor(this->Block[l][c][r].actor);

	if(ScreensPerNode > 1)
		renB->RemoveActor(this->Block[l][c][r].actor);
#ifdef DEBUG
	renA->RemoveActor(this->Block[l][c][r].textActor);
#endif
}


void AnimViewer::RemoveRegion(long l, long c, long r, long w, long h) {

	for(int i = c; i < c+w; i++) {
		for(int j = r; j < r+h; j++) {
			this->RemoveBlock(l, i, j);
		}
	}
}

void AnimViewer::AddRegion(long l, long c, long r, long w, long h) {

	for(int i = c; i < c+w; i++) {
		for(int j = r; j < r+h; j++) {
			this->AddBlock(l, i, j);
		}
	}
}


void AnimViewer::CleanUpMainWindow() {

	this->RemoveRegion(this->QueryWindow.level, 0, 0, this->NumOfCols, this->NumOfCols);
}



void AnimViewer::ComputeStartingBlockSize() {

	double sw, sh;

	double dw = double(this->BlockDimensions[0]) /
						double(this->BlockDimensions[1]);
	double dh = double(this->BlockDimensions[1]) /
						double(this->BlockDimensions[0]);

	if(dw > dh) {

		sw = double(this->BlockDimensions[0]) /
						double(this->BlockDimensions[0]);
		sh = double(this->BlockDimensions[1]) /
						double(this->BlockDimensions[0]);

	}
	else {

		sw = double(this->BlockDimensions[0]) /
						double(this->BlockDimensions[1]);
		sh = double(this->BlockDimensions[1]) /
						double(this->BlockDimensions[1]);
	}

	this->BlockInitSize[0] = sw;
	this->BlockInitSize[1] = sh;
}




void AnimViewer::PositionAndScaleBlocks() {


	long cols = this->NumOfCols;
	long rows = this->NumOfRows;

	this->ComputeStartingBlockSize();

	this->BlockInfo = new Info*[cols];

	for(int c=0 ; c<cols; c++) {

		this->BlockInfo[c] = new Info[rows];

		for(int r=0 ; r<rows; r++) {

			this->BlockInfo[c][r].scale[0] =
						this->BlockInitSize[0];

			this->BlockInfo[c][r].scale[1] =
						this->BlockInitSize[1];

			this->BlockInfo[c][r].position[0] =
					double(c) * this->BlockInitSize[0];

			this->BlockInfo[c][r].position[1] =
					double(r) * this->BlockInitSize[1];

			this->BlockInfo[c][r].position[2] = 0.1;

		}
	}

	for(int l = 0; l<this->NumOfLevels; l++) {
		for(int c=0 ; c<cols; c++) {
			for(int r=0 ; r<rows; r++) {


				this->Block[l][c][r].actor->
					SetPosition(this->BlockInfo[c][r].position[0],
						-this->BlockInfo[c][r].position[1],
							-this->BlockInfo[c][r].position[2]);

				this->Block[l][c][r].actor->
					SetScale(this->BlockInfo[c][r].scale[0],
						this->BlockInfo[c][r].scale[1], 0);


				this->Block[l][c][r].textActor->
					SetPosition(this->BlockInfo[c][r].position[0],
						-this->BlockInfo[c][r].position[1],
							-this->BlockInfo[c][r].position[2]);

				this->Block[l][c][r].textActor->
					SetScale(this->BlockInfo[c][r].scale[0] / 70,
						this->BlockInfo[c][r].scale[1] / 40, 0);

			}
		}

	}
}


void AnimViewer::AdjustLevelMovement(int direction, int axis) {

	PanAmount = this->BlockInfo[0][0].scale[axis] / PANSUBDIVISIONS;

}


void AnimViewer::InitViewer(char* name) {

	ifstream file(name);

	if (!file.is_open()) {
		printf("Can not read dat file.\n");
		exit(0);
    }


	file >> sageStr >> appname >> xWindowPos >> yWindowPos >>
			 this->GlobalDimensions[0] >> this->GlobalDimensions[1]
		 >> this->BlockDimensions[0] >> this->BlockDimensions[1]
		 >> this->NumOfLevels >> this->AnimDir >> this->Colorspace;

	if(!strcmp(sageStr.c_str(), "SAGE")) {
		SAGE = true;
		printf("Render to sage.   %s\n", sageStr.c_str());
	}
	else {

		SAGE == false;
		printf("No sage  %s\n", sageStr.c_str());
	}


	end = this->NumOfLevels;
	MODE = this->Colorspace;

	this->NumOfCols = this->GetNumOfCols(GlobalDimensions[0], BlockDimensions[0]);
	this->NumOfRows = this->GetNumOfRows(GlobalDimensions[1], BlockDimensions[1]);

	printf("Columns = %i Rows = %i \n", this->NumOfCols, this->NumOfRows);

	this->Blank =
		new unsigned char[this->BlockDimensions[0] *
						this->BlockDimensions[1] * MODE];

	for(int i=0; i < this->BlockDimensions[0] *
			this->BlockDimensions[1] * MODE; i++) {
		this->Blank[i] = (unsigned char) 0;
	}

	memset(this->Blank,
		0, this->BlockDimensions[0] * this->BlockDimensions[1] * MODE);

	/*
	printf("------------------------\n");
	printf("Global dim: %i %i\nBlock dim: %i %i\nNum of levels: %i\n",
		this->GlobalDimensions[0], this->GlobalDimensions[1],
		this->BlockDimensions[0], this->BlockDimensions[1],
		this->NumOfLevels);
	printf("------------------------\n");
	*/
}


void AnimViewer::PanLeft() {
	PointerPosition[0] -= PanAmount;
	pointer->SetPosition(PointerPosition[0], PointerPosition[1], 0);
	//CamPosition[0] -= PanAmount;
	//Fpoint[0] -= PanAmount;
	//UpdateCamera(CamPosition, Fpoint);
}

void AnimViewer::PanRight() {

	PointerPosition[0] += PanAmount;
	pointer->SetPosition(PointerPosition[0], PointerPosition[1], 0);

	//CamPosition[0] += PanAmount;
	//Fpoint[0] += PanAmount;
	//UpdateCamera(CamPosition, Fpoint);
}

void AnimViewer::PanUp() {

	PointerPosition[1] += PanAmount;
	pointer->SetPosition(PointerPosition[0], PointerPosition[1], 0);

	//CamPosition[1] += PanAmount;
	//Fpoint[1] += PanAmount;
	//UpdateCamera(CamPosition, Fpoint);
}

void AnimViewer::PanDown() {
	PointerPosition[1] -= PanAmount;
	pointer->SetPosition(PointerPosition[0], PointerPosition[1], 0);

	//CamPosition[1] -= PanAmount;
	//Fpoint[1] -= PanAmount;
	//UpdateCamera(CamPosition, Fpoint);
}

void AnimViewer::UpdateCamera(double c[], double f[]) {

	pointer->SetScale(CamPosition[2]/20, CamPosition[2]/20, 0);
	pointer->SetPosition(CamPosition[0], CamPosition[1], 0);

	PointerPosition[0] = CamPosition[0];
	PointerPosition[1] = CamPosition[1];


	renA->GetActiveCamera()->SetPosition(c);
	renA->GetActiveCamera()->SetFocalPoint(f);

	if(ScreensPerNode > 1) {
		renB->GetActiveCamera()->SetPosition(c);
		renB->GetActiveCamera()->SetFocalPoint(f);
	}
}



int AnimViewer::BlockOutOfBounds(long c, long r, long cols, long rows) {
	//printf("Available cols and rows: %i %i\n", cols, rows);

	if(c >= 0 && c < cols && r >= 0 && r < rows) {
		printf("Accepted %i %i\n", c, r);
		return ACCEPT;
	}
	else {
		printf("This one is off: %i %i\n", c, r);
		if( c < 0 )
			return RIGHT;
		if( c >= cols )
			return LEFT;
		if( r < 0 )
			return UP;
		if( r >= rows )
			return DOWN;
	}

}

void AnimViewer::CreateZoomBounds() {

	ZoomBounds = new ZBound[this->StartingLevel + 1];
	int i;
	double Z = StartingZ;
	for(i = this->StartingLevel; i >= 0; i--) {
		ZoomBounds[i].bound = Z;
		Z /=2;
	}

	this->NumOfZBounds = this->StartingLevel + 1;
}

void AnimViewer::GetZProjection(double zpos) {

	for(int i = 0 ; i < this->NumOfZBounds; i++) {
		if(ZoomBounds[i].bound >= zpos) {
			CamProjection.l = i;
			cout<<i<<endl;
			return;
		}
	}
}

void AnimViewer::SetInitialCamPosition() {
	long cols = this->NumOfCols;
	long rows = this->NumOfRows;


	CamPosition[0] = this->BlockInfo[cols/2][rows/2].position[0] -
			this->BlockInfo[0][0].scale[0]/2;
	CamPosition[1] = -this->BlockInfo[cols/2][rows/2].position[1] +
			this->BlockInfo[0][0].scale[1]/2;


	Fpoint[0] = CamPosition[0];
	Fpoint[1] = CamPosition[1];


	CamPosition[2] = (cols * this->BlockInfo[0][0].scale[0]) * 0.3;
	//CamPosition[2] = 3.2;
}

char* WaitForMessage()
{
	if(recmsg == NULL)
		recmsg = new char[TRANSMISSION_SIZE];
	memset(recmsg,0,TRANSMISSION_SIZE);
	static int size = TRANSMISSION_SIZE;

	if(client == NULL && server != NULL)
	{
		fprintf(stderr,"Waiting For Client To Connect.\n");
		client = server->waitForNewConnection();
		fprintf(stderr,"Ready For Messages.\n");


		int size;
		size = sizeof(float) * 3;
		float array[3];
		array[0] = CamPosition[0];
		array[1] = CamPosition[1];
		array[2] = CamPosition[2];
		client->write((const char*)array,&size,QUANTAnet_tcpClient_c::BLOCKING);
	}

	if(client)
	{
		int status;
		status = client->read(recmsg,&size,QUANTAnet_tcpClient_c::NON_BLOCKING);
	    if (status == QUANTAnet_tcpClient_c::OK)
			return recmsg;
		else
			{
				if (status == QUANTAnet_tcpClient_c::NON_BLOCKING_HAS_NO_DATA) usleep(1000);
				return NULL;
			}
	}

	return NULL;
}

void AnimViewer::SetTimeStep(int direction) {
	int step = CurrentLevel + direction;

	if(step == this->NumOfLevels) {
		step = 0;
	}

	if(step >= 0 && step < this->NumOfLevels) {
		this->CleanUpMainWindow();

		CurrentLevel = step;
		this->QueryWindow.level = CurrentLevel;
		cout<<"TIMESTEP *************"<<step<<endl;;
	}
}

void AnimViewer::SetCurrentLevel(int frame) {
	CurrentLevel = frame;
	this->QueryWindow.level = CurrentLevel;
}



void vtkWindowUp(int resx, int resy, double cw0, double ch0, double cw1, double ch1) {

	culler->SetMinimumCoverage(0.0);
	culler->SetMaximumCoverage(0.0);
	culler->SetSortingStyleToFrontToBack();

	light->SetLightTypeToSceneLight();
	light->SetPosition(0,0,10);

	renA->AddLight(light);
	renA->SetBackground(0,0,0);

	renB->AddLight(light);
	renB->SetBackground(0,0,0);

	if(rank > 0) {
		if(resx > resy) {
			renA->SetViewport(0,0,0.5,1);
			renB->SetViewport(0.5,0,1,1);
		}
		else {

			renA->SetViewport(0,0,1,0.5);
			renB->SetViewport(0,0.5,1,1);
		}

		renA->GetActiveCamera()->SetWindowCenter(cw0, ch0);
		renB->GetActiveCamera()->SetWindowCenter(cw1, ch1);
	}

	renA->AddCuller(culler);
	renB->AddCuller(culler);

	renWin->AddRenderer(renA);
	renWin->AddRenderer(renB);

	//renA->ResetCamera();
	//renB->ResetCamera();

	renWin->BordersOff();
	renWin->SetSize(resx, resy);


}



void vtkWindowUp(int resx, int resy, double cw, double ch) {

	culler->SetMinimumCoverage(0.0);
	culler->SetMaximumCoverage(0.0);
	culler->SetSortingStyleToFrontToBack();

	light->SetLightTypeToSceneLight();
	light->SetPosition(0,0,10);

	renA->AddLight(light);
	renA->SetBackground(0,0,0);

	if(rank > 0)
		renA->GetActiveCamera()->SetWindowCenter(cw, ch);

	renA->AddCuller(culler);

	renWin->AddRenderer(renA);

	//renA->ResetCamera();

	renWin->BordersOff();
	renWin->SetSize(resx, resy );
}

// Bad way to figure out location of global image center
// BAD!!!!!!!! BAD!!!!!!!!??? BAD!!!!!!!!
void UpCam() {

	renWin->AddRenderer(renA);
	renA->ResetCamera();

	if(ScreensPerNode > 1) {
		renWin->AddRenderer(renB);
		renB->ResetCamera();
	}
}



int main(int argc, char* argv[]) {

	char temp[HOST_NAME_MAX];
	gethostname(temp, HOST_NAME_MAX);
	hostname = temp;
	printf("Hostname: %s \n", hostname);

	if(rank == 0) {

		PORT = atoi(argv[3]);
	}



	unsigned char* windowBuf = NULL;

	MPI_Datatype action;
	MPI_Datatype position;

	// MPI Initialization
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Type_contiguous(1, MPI_LONG, &action);
	MPI_Type_contiguous(3, MPI_DOUBLE, &position);
	MPI_Type_commit(&position);


	animviewer.InitViewer(argv[2]);
	printf("App name: %s\n",appname.c_str());
	printf("X: %i Y: %i\n", xWindowPos, yWindowPos);


	// SAGE --------------------------------------------------------------------

	VirtualDesktop *vd = new VirtualDesktop();
	PhysicalDisplay *pd = new PhysicalDisplay();
	DisplayCluster *dc = new DisplayCluster();

	if(rank > 0) {

		int retcode;
		retcode = stdInputFileParser( argv[1], pd, dc, vd);
		if( retcode < 0 )
		{
			cout << "Node " << rank << " Failed to parse config file!" << endl;
			return 1;
		}

		//find the display node that matches our hostname
		char hostname[128] = "";
		if( gethostname( hostname, 128 ) )
		{
			cout << "Node " << rank << " Could not retrieve hostname!" << endl;
			return 1;
		}

		Display_Node* dn = dc->getFirstNode();

		while( dn && strcmp( dn->Name(), hostname ))
		{
			dn = dn->Next();
		}

		if( !dn || dn->NumberOfMaps() <= 0)
		{
			cout << "Node " << rank << "Could not match a display node with hostname: "
				<< hostname << endl;
			return 1;
		}

		//assume only one mapping per node for the app
		VDTtoPTmapper** maps = dn->Maps();

		// initialize SAGE
		sailConfig scfg;

//#ifdef SAGE
if(SAGE == true) {
		//sailConfig scfg;
		scfg.init("mframes.conf");
		cout<<"~~~~~~~~~~~~~~"<<endl;
		scfg.setAppName((char*)appname.c_str());
		scfg.rank = rank-1;
		scfg.nwID = 1;
		scfg.resX = maps[0]->VirtualDesktopTile()->WidthInPixels();
		scfg.resY = maps[0]->VirtualDesktopTile()->HeightInPixels();

		sageRect renderImageMap;
		renderImageMap.left = maps[0]->VirtualDesktopTile()->LowerLeftX();
		renderImageMap.bottom = maps[0]->VirtualDesktopTile()->LowerLeftY();
		renderImageMap.top = maps[0]->VirtualDesktopTile()->LowerLeftY() +
			maps[0]->VirtualDesktopTile()->HeightRatio();
		renderImageMap.right = maps[0]->VirtualDesktopTile()->LowerLeftX() +
			maps[0]->VirtualDesktopTile()->WidthRatio();

		scfg.imageMap = renderImageMap;
		scfg.pixFmt = PIXFMT_888;
		scfg.rowOrd = BOTTOM_TO_TOP;

		cout << "NODE: " << rank << " = w: " << scfg.resX
			<< " h: " << scfg.resY
			<< " coords: " << renderImageMap.left << ", "
			<< renderImageMap.top << " to "
			<< renderImageMap.right << ", "
			<< renderImageMap.bottom << endl;

}
//#endif
		int resX = window_w = maps[0]->VirtualDesktopTile()->WidthInPixels();
		int resY = window_h = maps[0]->VirtualDesktopTile()->HeightInPixels();


//#ifdef SAGE
if(SAGE == true) {
		sageInf.init(scfg);
		windowBuf = (unsigned char*)sageInf.getBuffer();
		cout << "sail initialized " << endl;
}
//#endif


		cout << "NODE " << rank << " WAITING TO QUIT" << endl;

		// Viewport window center calculations
		cw0 = - (vd->HorizRes() - 2*vd->HorizRes() *
					maps[0]->VirtualDesktopTile()->LowerLeftX()) /
					maps[0]->VirtualDesktopTile()->WidthInPixels() +
					(1.0 - maps[0]->PhysicalTile()->LeftMullionThickness() / 10);

		ch0 = - (vd->VertRes() - 2*vd->VertRes() *
					maps[0]->VirtualDesktopTile()->LowerLeftY()) /
					maps[0]->VirtualDesktopTile()->HeightInPixels() +
					(1.0 - maps[0]->PhysicalTile()->BottomMullionThickness() / 10);


		if(dn->NumberOfMaps() > 1) {
				ScreensPerNode = 2;

				cw1 = - (vd->HorizRes() - 2*vd->HorizRes() *
					maps[1]->VirtualDesktopTile()->LowerLeftX()) /
					maps[1]->VirtualDesktopTile()->WidthInPixels() +
					(1.0 - maps[1]->PhysicalTile()->LeftMullionThickness() / 10);

				ch1 = - (vd->VertRes() - 2*vd->VertRes() *
					maps[1]->VirtualDesktopTile()->LowerLeftY()) /
					maps[1]->VirtualDesktopTile()->HeightInPixels() +
					(1.0 - maps[1]->PhysicalTile()->BottomMullionThickness() / 10);


				if(maps[0]->VirtualDesktopTile()->LowerLeftY() ==
							maps[1]->VirtualDesktopTile()->LowerLeftY())
					vtkWindowUp(resX*2, resY, cw0, ch0, cw1, ch1);
				else
					vtkWindowUp(resX, resY*2, cw0, ch0, cw1, ch1);
		}

		else {

			vtkWindowUp(resX, resY, cw0, ch0);
		}
	}

	// SAGE --------------------------------------------------------------------

	//UpCam();


	renWin->SetPosition(xWindowPos, yWindowPos);

	animviewer.GetAnimFiles();
	animviewer.InitRendering();
	animviewer.SetUpPipeline();

	animviewer.PositionAndScaleBlocks();
	animviewer.ImportQueryWindowPixels();
	animviewer.GivePixelsToVTK();


	animviewer.SetInitialCamPosition();
	/*
	renA->InteractiveOff();
	renA->GetActiveCamera()->GetPosition(CamPosition);
	renA->GetActiveCamera()->SetClippingRange(-500,500);
	renA->GetActiveCamera()->GetFocalPoint(Fpoint);



	if(ScreensPerNode > 1) {

		renB->InteractiveOff();
		//renB->GetActiveCamera()->GetPosition(CamPosition);
		renB->GetActiveCamera()->SetClippingRange(-500,500);
		//renB->GetActiveCamera()->GetFocalPoint(Fpoint);
	}

	CamPosition[2] = 3.2;

	*/

	StartingZ = CamPosition[2];
	animviewer.UpdateCamera(CamPosition, Fpoint);


	if(rank == 0) {


		QUANTAinit();
		server = new QUANTAnet_tcpServer_c();
		client = NULL;
		recmsg = NULL;

		if( server->init(PORT) <  0)
		{
			printf("Bad port: %i", PORT);
			server->close();
			delete server;
			server = NULL;
			exit(0);
		}

		printf("Running on port %i\n", PORT);


		while(true) {

			uiaction = -100;

			char* msg = WaitForMessage();

			if (msg != NULL)
			{
				printf("%s\n", msg);

				if(!strcmp(msg, SLIDER1)) {
					uiaction = 0;

					static int readsize = sizeof(float) * 1;
					float readarray[1];
					if( client ) {
						client->read((char*)readarray,&readsize,QUANTAnet_tcpClient_c::BLOCKING);
					}

					pos[0] = readarray[0];
				}

				else if(!strcmp(msg, SLIDER2)) {
					uiaction = 1;

					static int readsize = sizeof(float) * 1;
					float readarray[1];
					if( client ) {
						client->read((char*)readarray,&readsize,QUANTAnet_tcpClient_c::BLOCKING);
					}

					pos[1] = readarray[0];
				}

				else if(!strcmp(msg, SLIDER3)) {
					uiaction = 2;

					static int readsize = sizeof(float) * 1;
					float readarray[1];
					if( client ) {
						client->read((char*)readarray,&readsize,QUANTAnet_tcpClient_c::BLOCKING);
					}

					pos[2] = readarray[0];
				}

				else if(!strcmp(msg, RELEASE)) {
					uiaction = 3;
				}

				else if(!strcmp(msg, PRESS)) {
					uiaction = 4;
				}

				else if(!strcmp(msg, BROWSELEFT)) {
					uiaction = 5;
				}

				else if(!strcmp(msg, BROWSERIGHT)) {
					uiaction = 6;
				}

				else if(!strcmp(msg, PANLEFT)) {
					uiaction = 7;
				}

				else if(!strcmp(msg, PANRIGHT)) {
					uiaction = 8;
				}

				else if(!strcmp(msg, PANUP)) {
					uiaction = 9;
				}

				else if(!strcmp(msg, PANDOWN)) {
					uiaction = 10;
				}

				else if(!strcmp(msg, QUIT)) {
					if(client)
						client->close();

					if(server)
						server->close();

					if(client)
						delete client;

					if(server)
						delete server;

					if(msg)
						delete [] msg;

					uiaction = 11;
				}

				else if(!strcmp(msg, CENTER)) {
					uiaction = 12;
				}

			}

			for (int i = 0 ; i < size - 1 ; i++) {
				MPI_Send(&uiaction, 1, action, i+1, 1, MPI_COMM_WORLD);
			}

			for (int i = 0 ; i < size - 1 ; i++) {
				MPI_Send(&pos, 1, position, i+1, 1, MPI_COMM_WORLD);
			}

			if(uiaction == 11) break;

			// synchronize
			MPI_Barrier(MPI_COMM_WORLD);
		}

	}

	else if (rank > 0) {
		// MPI status variable
    	MPI_Status status;
		MPI_Status status1;

		animviewer.CreateZoomBounds();

		cout<<"Cam "<<CamPosition[0]<<" "<<CamPosition[1]
					<<" "<<CamPosition[2]<<endl;

		/*
		if(rank == 5) {
			renA->AddActor2D(text);
			renWin->Render();
		}

		int preload = 0;
		if(end < 20) preload = end;
		else preload = 20;

		for(int i=start; i<preload; i++) {
			animviewer.SetTimeStep(1);
			animviewer.ImportQueryWindowPixels();
			animviewer.GivePixelsToVTK();
			renWin->Render();
		}

		if(rank == 5) {
			renA->RemoveActor2D(text);
		}
		*/

		pointer->SetPosition(CamPosition[0], CamPosition[1], 0);
		renA->AddActor(pointer);
		if(ScreensPerNode > 1) renB->AddActor(pointer);

		pointer->SetScale(CamPosition[2]/20, CamPosition[2]/20, 0);


		// Reset to first frame
		animviewer.SetCurrentLevel(start);
		//animviewer.ImportQueryWindowPixels();
		//animviewer.GivePixelsToVTK();
		renWin->Render();

		while(true) {

			MPI_Recv(&uiaction, 1, action, 0, 1, MPI_COMM_WORLD, &status);

			MPI_Recv(&pos, 1, position, 0, 1, MPI_COMM_WORLD, &status1);

			if(uiaction == 0) {

				CamPosition[0] = pos[0];
				Fpoint[0] = pos[0];

				animviewer.UpdateCamera(CamPosition, Fpoint);

			}

			if(uiaction == 1) {

				CamPosition[1] = pos[1];
				Fpoint[1] = pos[1];

				animviewer.UpdateCamera(CamPosition, Fpoint);

			}

			if(uiaction == 2) {
				if(pos[2] > 0) {
					CamPosition[2] = pos[2];

					animviewer.UpdateCamera(CamPosition, Fpoint);
				}
			}

			if(uiaction == 5) {

				animviewer.SetTimeStep(-1);

				//if(rank == 5) {
				//	renA->AddActor2D(text);
				//	renWin->Render();
				//}

				animviewer.ImportQueryWindowPixels();
				animviewer.GivePixelsToVTK();

				//if(rank == 5) {
				//	renA->RemoveActor2D(text);
				//}

			}

			if(uiaction == 6) {

				animviewer.SetTimeStep(1);

				//if(rank == 5) {
				//	renA->AddActor2D(text);
				//	renWin->Render();
				//}

				animviewer.ImportQueryWindowPixels();
				animviewer.GivePixelsToVTK();

				//if(rank == 5) {
				//	renA->RemoveActor2D(text);
				//}

			}


			if(uiaction == 7) {

				animviewer.AdjustLevelMovement(LEFT,X);

				animviewer.PanLeft();


			}

			if(uiaction == 8) {

				animviewer.AdjustLevelMovement(RIGHT,X);

				animviewer.PanRight();


			}

			if(uiaction == 10) {


				animviewer.AdjustLevelMovement(DOWN,Y);

				animviewer.PanDown();

			}

			if(uiaction == 9) {

				animviewer.AdjustLevelMovement(UP,Y);

				animviewer.PanUp();

			}


			if(uiaction == 3) {

			}

			if(uiaction == 4) {
				//animviewer.CleanUpMainWindow();
			}

			if(uiaction == 12) {
				CamPosition[0] = PointerPosition[0];
				CamPosition[1] = PointerPosition[1];
				Fpoint[0] = PointerPosition[0];
				Fpoint[1] = PointerPosition[1];

				animviewer.UpdateCamera(CamPosition, Fpoint);


			}

			if(uiaction == 11) {
				if(SAGE == true) {
					//if (windowBuf) delete [] windowBuf;
				}
				break;
			}




			renWin->Render();
//#ifdef SAGE
			if(SAGE == true) {

			//if (windowBuf) delete [] windowBuf;


			memcpy(windowBuf, renWin->GetPixelData(0, 0, window_w-1, window_h-1, 1), window_w*window_h*3);
}
//#endif

			// synchronize
			MPI_Barrier(MPI_COMM_WORLD);
//#ifdef SAGE
if(SAGE == true) {
			sageInf.swapBuffer();
			windowBuf = (unsigned char *)sageInf.getBuffer();
}
//#endif

		}
	}

	// finalize
	MPI_Finalize();

	renA->Delete();
	renB->Delete();
	renWin->Delete();
	light->Delete();
	culler->Delete();
	text->Delete();

	cone->Delete();
	cube->Delete();
	sphere->Delete();
	sphereMapper->Delete();
	pointer->Delete();

	splash->Delete();
	splashMapper->Delete();
	splashScreen->Delete();

	// exit
	return EXIT_SUCCESS;
}
