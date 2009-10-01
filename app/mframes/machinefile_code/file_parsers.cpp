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

/*------------------------------------------------------------------------
-	Author: Arun Rao
-	Library: Tile Config
-	Revision Date: 4/22/2004
-	Module: file_parsers.cpp
-	Purpose: Contains function implementation for file i/o to load in
-	and write out tiled display system descriptions
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "file_parsers.h"

#define UP_MULLION 0
#define DOWN_MULLION 1
#define LEFT_MULLION 2
#define RIGHT_MULLION 3

#ifdef _cplusplus
extern "C" {
#endif

//--------------------------------------------------------
int stdInputFileParser(char* filename,PhysicalDisplay*& pd,DisplayCluster*& dc,VirtualDesktop*& vd)
{
	if(filename == NULL)
		return -1;

	FILE* fin = fopen(filename,"r");

	if(fin == NULL)
		return -1;

	//create objects
	cout << "Creating High Level Objects" << endl;

	if(pd == NULL)
		pd = new PhysicalDisplay();
	if(dc == NULL)
		dc = new DisplayCluster();
	if(vd == NULL)
		vd = new VirtualDesktop();

	//setup associations
	cout << "Setting Associations" << endl;
	dc->VirtualDesktopAssoc(vd);
	dc->PhysicalDisplayAssoc(pd);
	vd->PhysicalDisplayAssoc(pd);
	vd->DisplayClusterAssoc(dc);
	pd->VirtualDesktopAssoc(vd);
	pd->DisplayClusterAssoc(dc);

	Virtual_Desktop_Tile** tempVDT = NULL;
	Physical_Tile** tempPT = NULL;
	Display_Node** tempDN = NULL;
	VDTtoPTmapper*** tempMapper = NULL;
	char buffer1[32];

	cout << "Gathering: Dimensions - ";
	//First is Dimension Data
	fscanf(fin,"%s",buffer1);
	while( !strcmp( buffer1, "#" ))
	{
		while(fgetc(fin) != '\n');
		fscanf(fin,"%s",buffer1);
	}

	if(strcmp( buffer1, "TileDisplay"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Dimensions"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}
	int dimx, dimy;
	fscanf(fin, "%d %d\n", &dimx, &dimy);

	//Next is the Mullions for every screen
	cout << "Mullions - ";
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Mullions"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	float mullions[4];
	fscanf( fin, "%f %f %f %f\n", &mullions[UP_MULLION], &mullions[DOWN_MULLION],
					&mullions[LEFT_MULLION], &mullions[RIGHT_MULLION]);

	cout << "Resolution - ";
	//Next is Resolution of each screen
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Resolution"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	float resolution[2];
	fscanf( fin, "%f %f\n", &resolution[0], &resolution[1]);

	cout << "Pixels Per Inch - ";
	//Pixels Per Inch Is Next
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "PPI"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	float pixelsPerInch;
	fscanf(fin, "%f\n", &pixelsPerInch);
	pd->PixelsPerInch( pixelsPerInch );

	//Determine Screen Dimensions from Pixels Per Inch & Screen Resolutions
	float screenWidth, screenHeight;
	screenWidth = resolution[0] / (float)pixelsPerInch;
	screenHeight = resolution[1] / (float)pixelsPerInch;
	cout << "ScreenWidth x ScreenHeight: " << screenWidth << 'x' << screenHeight << endl;
	//figure out the "total dimensions" of a tile
	float totalWidth = screenWidth + mullions[LEFT_MULLION] + mullions[RIGHT_MULLION];
	float totalHeight = screenHeight + mullions[UP_MULLION] + mullions[DOWN_MULLION];

	cout << "Machine Count" << endl;
	//Number of Machines is Next
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Machines"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	int machineCount;
	fscanf(fin,"%d\n",&machineCount);


	//Create the Virtual Desktop Tiles, Physical Tiles & Mappers to connect each one
	tempVDT = new Virtual_Desktop_Tile*[dimx];
	tempPT = new Physical_Tile*[dimx];
//	tempMapper = new VDTtoPTmapper**[dimx];
	cout << "Generating Tiles & Virtual Tiles" << endl;
	for(int i = 0; i < dimx; i++)
	{
		tempVDT[i] = new Virtual_Desktop_Tile[dimy];
		tempPT[i] = new Physical_Tile[dimy];
//		tempMapper[i] = new VDTtoPTMapper*[dimy];

		for(int j = 0; j < dimy; j++)
		{

			tempPT[i][j].ScreenHeightDimension( screenHeight );
			tempPT[i][j].ScreenWidthDimension( screenWidth );
			tempPT[i][j].BottomMullionThickness( mullions[DOWN_MULLION] );
			tempPT[i][j].LeftMullionThickness( mullions[LEFT_MULLION] );
			tempPT[i][j].RightMullionThickness( mullions[RIGHT_MULLION] );
			tempPT[i][j].TopMullionThickness( mullions[UP_MULLION] );

			tempPT[i][j].LowerLeftX( i * totalWidth );
			tempPT[i][j].LowerLeftY( j * totalHeight );
			pd->AddPhysicalTile( &(tempPT[i][j]));

		}
	}

	//Go through and get each machine
	cout << "Generating Machines & Creating Mappings" << endl;
	for(int k = 0; k < machineCount; k++)
	{
		char ip[16];
		char name[32];
		int maps;
		int tileX, tileY;

		cout << k << ' ';
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "DisplayNode"))
		{
			cout << "Read: " << buffer1 << " instead of \"DisplayNode\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}
		//Name
		cout << "Name ";
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "Name"))
		{
			cout << "Read: " << buffer1 << " instead of \"Name\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}

		fscanf(fin,"%s",name);
		cout << name << ' ';
		//ip
		cout << "IP: ";
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "IP"))
		{
			cout << "Read: " << buffer1 << " instead of \"IP\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}

		fscanf(fin,"%s",ip);
		cout << ip << ' ';
		//Monitors
		cout << "Monitors: ";
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "Monitors"))
		{
			cout << "Read: " << buffer1 << " instead of \"Monitors\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}

		fscanf(fin,"%d", &maps );
		cout << maps << ' ';;
		Display_Node* dn;
		dn = new Display_Node();
		dn->NumberOfMaps( maps );
		dn->Name( name );
		dn->IP( ip );

		dc->AddDisplayNode( dn );

		for( int i = 0; i < maps; i++)
		{
			cout << '.';
			fscanf(fin," (%d,%d)", &tileX, &tileY);
			cout << '(' <<  tileX << ',' << tileY << ')';
			dc->GenerateMapping( &(tempVDT[tileX][tileY]), &(tempPT[tileX][tileY]), dn );
			vd->AddTile( &(tempVDT[tileX][tileY]) );

		}

		//fscanf(fin,"%s\n", buffer1);
		cout << endl;

#ifdef _DEBUG


#endif

	}

	cout << endl << "Updating Virutal Desktop" << endl;

	vd->Update();

	fclose(fin);

	return 1;
}

//--------------------------------------------------------
void stdOutputFileWriter(char* filename,PhysicalDisplay* pd,DisplayCluster* dc,VirtualDesktop* vd)
{

}


//--------------------------------------------------------
int corewallInputFileParser(char* filename, PhysicalDisplay*& pd, DisplayCluster*& dc, VirtualDesktop*& vd, int*& startPos)
{

		if(filename == NULL)
		return -1;

	FILE* fin = fopen(filename,"r");

	if(fin == NULL)
		return -1;

	//create objects
	cout << "Creating High Level Objects" << endl;
	if(pd == NULL)
		pd = new PhysicalDisplay();
	if(dc == NULL)
		dc = new DisplayCluster();
	if(vd == NULL)
		vd = new VirtualDesktop();

	//setup associations
	cout << "Setting Associations" << endl;

	dc->VirtualDesktopAssoc(vd);
	dc->PhysicalDisplayAssoc(pd);
	vd->PhysicalDisplayAssoc(pd);
	vd->DisplayClusterAssoc(dc);
	pd->VirtualDesktopAssoc(vd);
	pd->DisplayClusterAssoc(dc);

	Virtual_Desktop_Tile** tempVDT = NULL;
	Physical_Tile** tempPT = NULL;
	Display_Node** tempDN = NULL;
	VDTtoPTmapper*** tempMapper = NULL;
	char buffer1[32];

	//cout << "Gathering: Dimensions - ";
	printf("getting dimensions\n");
	//First is Dimension Data
	fscanf(fin,"%s",buffer1);
	while( !strcmp( buffer1, "#" ))
	{
		while(fgetc(fin) != '\n');
		fscanf(fin,"%s",buffer1);
	}

	if(strcmp( buffer1, "TileDisplay"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Dimensions"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}
	int dimx, dimy;
	fscanf(fin, "%d %d\n", &dimx, &dimy);

	//Next is the Mullions for every screen
	cout << "Mullions - ";
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Mullions"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	float mullions[4];
	fscanf( fin, "%f %f %f %f\n", &mullions[UP_MULLION], &mullions[DOWN_MULLION],
					&mullions[LEFT_MULLION], &mullions[RIGHT_MULLION]);

	cout << "Resolution - ";
	//Next is Resolution of each screen
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Resolution"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	float resolution[2];
	fscanf( fin, "%f %f\n", &resolution[0], &resolution[1]);

	cout << "Pixels Per Inch - ";
	//Pixels Per Inch Is Next
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "PPI"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	float pixelsPerInch;
	fscanf(fin, "%f\n", &pixelsPerInch);
	pd->PixelsPerInch( pixelsPerInch );

	//Determine Screen Dimensions from Pixels Per Inch & Screen Resolutions
	float screenWidth, screenHeight;
	screenWidth = resolution[0] / (float)pixelsPerInch;
	screenHeight = resolution[1] / (float)pixelsPerInch;
	cout << "ScreenWidth x ScreenHeight: " << screenWidth << 'x' << screenHeight << endl;
	//figure out the "total dimensions" of a tile
	float totalWidth = screenWidth + mullions[LEFT_MULLION] + mullions[RIGHT_MULLION];
	float totalHeight = screenHeight + mullions[UP_MULLION] + mullions[DOWN_MULLION];

	cout << "Machine Count" << endl;
	//Number of Machines is Next
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "Machines"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	int machineCount;
	fscanf(fin,"%d\n",&machineCount);

	//get the offsets
	cout << "OffsetX" << endl;
	//OffsetX is Next
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "OffsetX"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	fscanf(fin,"%d\n",&(startPos[0]));


	cout << "OffsetY" << endl;
	//OffsetY is Next
	fscanf(fin,"%s",buffer1);
	if(strcmp( buffer1, "OffsetY"))
	{
		delete pd;
		delete dc;
		delete vd;
		pd = NULL;
		dc = NULL;
		vd = NULL;
		return -1;
	}

	fscanf(fin,"%d\n",&(startPos[1]));


	//Create the Virtual Desktop Tiles, Physical Tiles & Mappers to connect each one
	tempVDT = new Virtual_Desktop_Tile*[dimx];
	tempPT = new Physical_Tile*[dimx];
//	tempMapper = new VDTtoPTmapper**[dimx];
	cout << "Generating Tiles & Virtual Tiles" << endl;
	for(int i = 0; i < dimx; i++)
	{
		tempVDT[i] = new Virtual_Desktop_Tile[dimy];
		tempPT[i] = new Physical_Tile[dimy];
//		tempMapper[i] = new VDTtoPTMapper*[dimy];

		for(int j = 0; j < dimy; j++)
		{

			tempPT[i][j].ScreenHeightDimension( screenHeight );
			tempPT[i][j].ScreenWidthDimension( screenWidth );
			tempPT[i][j].BottomMullionThickness( mullions[DOWN_MULLION] );
			tempPT[i][j].LeftMullionThickness( mullions[LEFT_MULLION] );
			tempPT[i][j].RightMullionThickness( mullions[RIGHT_MULLION] );
			tempPT[i][j].TopMullionThickness( mullions[UP_MULLION] );

			tempPT[i][j].LowerLeftX( i * totalWidth );
			tempPT[i][j].LowerLeftY( j * totalHeight );
			pd->AddPhysicalTile( &(tempPT[i][j]));

		}
	}

	//Get the starting offsets
	//Go through and get each machine
	cout << "Generating Machines & Creating Mappings" << endl;
	for(int k = 0; k < machineCount; k++)
	{
		char ip[16];
		char name[32];
		int maps;
		int tileX, tileY;

		cout << k << ' ';
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "DisplayNode"))
		{
			cout << "Read: " << buffer1 << " instead of \"DisplayNode\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}
		//Name
		cout << "Name ";
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "Name"))
		{
			cout << "Read: " << buffer1 << " instead of \"Name\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}

		fscanf(fin,"%s",name);
		cout << name << ' ';
		//ip
		cout << "IP: ";
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "IP"))
		{
			cout << "Read: " << buffer1 << " instead of \"IP\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}

		fscanf(fin,"%s",ip);
		cout << ip << ' ';
		//Monitors
		cout << "Monitors: ";
		fscanf(fin,"%s",buffer1);
		if(strcmp( buffer1, "Monitors"))
		{
			cout << "Read: " << buffer1 << " instead of \"Monitors\"" << endl;
			delete pd;
			delete dc;
			delete vd;
			pd = NULL;
			dc = NULL;
			vd = NULL;
			return -1;
		}

		fscanf(fin,"%d", &maps );
		cout << maps << ' ';;
		Display_Node* dn;
		dn = new Display_Node();
		dn->NumberOfMaps( maps );
		dn->Name( name );
		dn->IP( ip );

		dc->AddDisplayNode( dn );

		for( int i = 0; i < maps; i++)
		{
			cout << '.';
			fscanf(fin," (%d,%d)", &tileX, &tileY);
//			cout << '(' <<  tileX << ',' << tileY << ')';
			dc->GenerateMapping( &(tempVDT[tileX][tileY]), &(tempPT[tileX][tileY]), dn );
			tempVDT[tileX][tileY].NonMullionLLX( resolution[0] * tileX );
			tempVDT[tileX][tileY].NonMullionLLY( resolution[1] * tileY );
			cout << "NonMullion x,y: " << screenWidth * tileX << ", "
				<< screenHeight * tileY << endl;
			vd->AddTile( &(tempVDT[tileX][tileY]) );
		}

		//fscanf(fin,"%s\n", buffer1);
//		cout << endl;

#ifdef _DEBUG


#endif

	}

	cout << endl << "Updating Virutal Desktop" << endl;

	vd->Update();

	fclose(fin);

	return 1;
}

#ifdef _cplusplus
}
#endif
