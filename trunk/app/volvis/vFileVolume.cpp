/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
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
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

//temporarily commented for the demo
#if 0
//Class vFileVolume: Defines the 3D volume thats loaded from a file
//This class only manages the data(doesnt do the rendering)
//Shalini Venkataraman, Luc Renambot
//Sep 7 2003
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glUE.h"
#include "vVolume.h"
#include "VectorMath.h"
#include "global.h"

vFileVolume::vFileVolume(const char* fname, int c, int r, int d):vVolume(c,r,d) {
	strcpy(this->filename,fname);
	padY = padX = padZ =0;
	filename[0] = '\0';
	gradientFile = 0;
	fileBuffer = 0;
	//vVolume(c,r,d);
}

vFileVolume::~vFileVolume() {
	if (gradientFile)
		delete [] gradientFile;
}


void vFileVolume::load(int loadx, int loady, int loadz) {
	setLoadDims(loadx,loady,loadz);
	voxelData = new GLubyte[dimZ*dimY*dimX];
	loadVolumeFromFile(filename);
	if (gradientFile)
		loadGradientFromFile(gradientFile);
	else
		calcGradient(); //create grad volume
}

void vFileVolume::loadVolumeFromFile(const char* filename){
	int rc= mmap.setFilename(filename, 0);
	cout << " == " << rc <<endl;
	// Call mmap
	fileBuffer= (unsigned char *)mmap.doMmap();
	// Did the mapping work?
	if((char*)fileBuffer== (char *)-1)
	{
		cerr << "mmap failed -- No memory :-( " << endl;
		exit(1);
	}

	printf("Loading Volume %s\n",filename);
	int filePlaneDims = fileX*fileY;//plane in a volume file
	int planeDims = dimX*dimY; //plane in the volume that we want to load
	GLubyte* plane = new GLubyte[planeDims];
	padVolume(); //pad volume if necessary
	//if we padded, just fill the dimesions in the file
	int fillZ = padZ?fileZ:dimZ;
	int fillY = padY?fileY:dimY;
	int fillX = padX?fileX:dimX;
	//if there is a dimZ to skip, skip it
	unsigned char* curFilePtr = fileBuffer+filePlaneDims*offsetZ*sizeof(GLubyte);
	//load the volume one plane at a time
	for (int r=0;r<fillZ;r++) {
		readASlice(plane,curFilePtr);
		for (int t=0;t< fillY;t++) {
			for (int s=0;s< fillX;s++) {
				setValue(r+padZ,t+padY,s+padX,plane[(t*fillX+s)]);
			}
		}
	}
	printf("Loading Textures\n");
	delete [] plane;
}


//if the volume is bigger than the file size, this will pad the volume
//else do nothing
void vFileVolume::padVolume() {
	unsigned int r, s, t;
	int lastSlice;
	//pad a little with black if fileZ < dimZ
	if (dimZ>fileZ) {
		padZ = (dimZ-fileZ)/2;
		lastSlice = (dimZ-padZ);
		for (r=0;r<padZ;r++){
			for (t=0;t<dimY;t++){
				for (s=0;s<dimX;s++){
					setValue(r,t,s,0);
					setValue(r+lastSlice,t,s,0);
				}
			}
		}
	}
	if (dimY>fileY) {
		//pad a little with black if fileY doesnt match dimY
		padY = (dimY-fileY)/2;
		lastSlice = (dimY-padY);
		for (r=0;r<dimZ;r++){
			for (t=0;t<padY;t++){
				for (s=0;s<dimX;s++){
					setValue(r,t,s,0);
					setValue(r,t+lastSlice,s,0);
				}
			}
		}
	}

	if (dimX>fileX) {
		//pad a little with black if fileX doesnt match dimX
		padX = (dimX-fileX)/2;
		lastSlice = (dimX-padX);
		for (r=0;r<dimZ;r++){
			for (t=0;t<dimY;t++){
				for (s=0;s<padX;s++){
					setValue(r,t,s,0);
					setValue(r,t,s+lastSlice,0);
				}
			}
		}
	}
}

//reads a slice skipping the Y and X offsets
void vFileVolume::readASlice(GLubyte* plane, unsigned char* &curFilePtr) {
	//calculating the remaining rows and colums to skip once a sub image is read in
        int remainingX = fileX-offsetX-dimX;
	int remainingY = fileY-offsetY-dimY;

	//skip the rows on one slice to start the read
	curFilePtr += fileX*offsetY*sizeof(GLubyte);
        GLubyte* volDataPtr = plane;
        for (int i=0;i<dimY;i++) {
		//skip the offset X
		curFilePtr += offsetX*sizeof(GLubyte);
		//read the sub-cols
		memcpy(volDataPtr,curFilePtr,dimX);
		//skip the cols read
		curFilePtr += dimX*sizeof(GLubyte);
		//skip the remaining cols
		curFilePtr += remainingX*sizeof(GLubyte);
                volDataPtr += dimX;//skip in the volume
        }
	//skip the remaining rows
	curFilePtr += remainingY*fileX*sizeof(GLubyte);
	fprintf(stderr,"ok here\n");
}



void vFileVolume::loadGradientFromFile(const char* filename) {
	FILE* fp;
	if (!(fp=fopen(filename,"rb"))) {
		printf("Error: unable to open volume file %s\n",filename);
		exit(0);
	}
	else
		fprintf(stderr,"Opening file %s\n",filename);
	int volSize = dimY*dimX*dimZ;
	gradData = new unsigned char[volSize];
	int numRead = fread(gradData,sizeof(unsigned char), volSize,fp);
	if (numRead != volSize)
		fprintf(stderr,"Error: gradient data size %d doesnt match volume size %d\n",numRead,volSize);
	fclose(fp);
}

//calculate the value, gradient components of the volume
//- may be needed by the transfer functions
void vFileVolume::calcGradient()
{
	unsigned char *vdata = (unsigned char*)voxelData;
	float *gradV3 = new float[dimX*dimY*dimZ*3]; //individual components of the gradient
	float *gmag = new float[dimX*dimY*dimZ]; //gradient magnitude
	float gmmax = -100000000;
	float gmmin = 100000000;
	float dmax = -100000000;
	float dmin = 100000000;


	//compute the gradient
	cerr << "   computing 1st derivative";
	int i;
	for(i = 0; i < dimZ; ++i){
		for(int j = 0; j < dimY; ++j){
			for(int k = 0; k < dimX; ++k){
				if((k<1)||(k>dimX-2)||(j<1)||(j>dimY-2)||(i<1)||(i>dimZ-2)){
					gradV3[i*dimX*dimY*3 + j*dimX*3 + k*3 + 0] = 0;
					gradV3[i*dimX*dimY*3 + j*dimX*3 + k*3 + 1] = 0;
					gradV3[i*dimX*dimY*3 + j*dimX*3 + k*3 + 2] = 0;
					gmag[i*dimX*dimY + j*dimX + k] = 0;
				}
				else {
					float dx = (float)(vdata[i*dimX*dimY + j*dimX + (k+1)]
									- vdata[i*dimX*dimY + j*dimX + (k-1)]);
					float dy = (float)(vdata[i*dimX*dimY + (j+1)*dimX + k]
									- vdata[i*dimX*dimY + (j-1)*dimX + k]);
					float dz = (float)(vdata[(i+1)*dimX*dimY + j*dimX + k]
									- vdata[(i-1)*dimX*dimY + j*dimX + k]);

					gradV3[i*dimX*dimY*3 + j*dimX*3 + k*3 + 0] = dx;
					gradV3[i*dimX*dimY*3 + j*dimX*3 + k*3 + 1] = dy;
					gradV3[i*dimX*dimY*3 + j*dimX*3 + k*3 + 2] = dz;
					gmag[i*dimX*dimY + j*dimX + k] = (float)sqrt(dx*dx+dy*dy+dz*dz);
					gmmax = MAX(gmag[i*dimX*dimY + j*dimX + k], gmmax);
					gmmin = MIN(gmag[i*dimX*dimY + j*dimX + k], gmmin);
					dmax = MAX(vdata[i*dimX*dimY + j*dimX +k], dmax);
					dmin = MIN(vdata[i*dimX*dimY + j*dimX +k], dmin);
				}
			}
		}
	}
	cerr << " - done" << endl;
	cerr << "   quantizing";
	gradData = new unsigned char[dimX*dimY*dimZ]; //stores the 1st derivative

	for(i = 0; i < dimZ; ++i){
		for(int j = 0; j < (dimY); ++j){
			for(int k = 0; k < (dimX); ++k){
				if((k<1)||(k>dimX-2)||(j<1)||(j>dimY-2)||(i<1)||(i>dimZ-2)){
					gradData[i*dimX*dimY + j*dimX + k] = 0;
				}
				else {
					gradData[i*dimX*dimY + j*dimX + k] = (unsigned char)affine(gmmin, gmag[i*dimX*dimY + j*dimX + k], gmmax, 0, 255);
				}
			}
		}
	}
	cerr << " - done" << endl;
	delete [] gradV3;
	delete [] gmag;
}
#endif
