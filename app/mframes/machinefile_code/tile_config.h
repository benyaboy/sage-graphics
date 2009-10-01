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
-	Module: tile_config.h
-	Purpose: Contains all class descriptions for building
-	a description of a tile display system, at run time
---------------------------------------------------------------------------*/
#ifndef SAGE_TILE_CONFIG_H
#define SAGE_TILE_CONFIG_H

#define _DEBUG 1

#include <stdio.h>
#include <stdlib.h>

//__________________________________________
class Physical_Tile;
class Display_Node;
class Virtual_Desktop_Tile;

class DisplayCluster;
class PhysicalDisplay;
class VirtualDesktop;
//__________________________________________

class Physical_Tile
{
private:
	//int horizRes;	//horizontal resolution
	//int vertRes;	//vertical resolution
	float screenWidth;	//width of screen in inches
	float screenHeight;	//height of screen in inches
	float leftMullion;	//the width of the left Mullion in inches
	float rightMullion;	// "	"	"  right " 	"
	float topMullion;		// "	"	"  top   "	"
	float bottomMullion;	// "	"	"  bottom "	"
	float lowerleftX;	//the x position of the bottom left corner of the tile (including the mullions from left)
						//with respect to some arbritrary origin
	float lowerleftY;	//the y position of the bottom left corner of the tile (including the mullions from bottom)
						//with respect to some arbritrary origin
	int id;
	Virtual_Desktop_Tile* virtDeskTile;	//the virtual desktop tile that the physical tile will display
	Physical_Tile* next;
	Display_Node*	 source;	//the display node that is the source to the physical tile
public:
	Physical_Tile();
	~Physical_Tile();

	float ScreenWidthDimension();
	float ScreenHeightDimension();
	float LeftMullionThickness();
	float RightMullionThickness();
	float TopMullionThickness();
	float BottomMullionThickness();
	float LowerLeftX();
	float LowerLeftY();
	int ID();
	Physical_Tile* Next();
	Display_Node* Source();
	Virtual_Desktop_Tile* VirtualDesktopTile();


	void ScreenWidthDimension(float);
	void ScreenHeightDimension(float);
	void RightMullionThickness(float);
	void LeftMullionThickness(float);
	void TopMullionThickness(float);
	void BottomMullionThickness(float);
	void LowerLeftX(float);
	void LowerLeftY(float);
	void ID(int);
	void Next(Physical_Tile*);
	void Source(Display_Node*);
	void VirtualDesktopTile(Virtual_Desktop_Tile*);
};

class Virtual_Desktop_Tile
{
private:
	float	lowerLeftX;	//the ratio of the lower left x position to the entire virtual desktop width
	float 	lowerLeftY;	//the ratio of the lower left y position to the entire virtual desktop height

	float   widthRatio;   //the ratio of the width of the virtual desktop tile
				//to the entire virtual desktop width

	float 	heightRatio;	//the ratio of the height of the virtual desktop tile
				//to the entire virtual desktop height

	int 	id;		//the id of the virtual desktop tile


	int		lowerLeftPixelX;
	int		lowerLeftPixelY;
	int		widthInPixels;
	int		heightInPixels;

	int 	nonMullionLLX;	//lower left X on a zero mullion display
	int		nonMullionLLY;	//lower left Y on a zero mullion display

	Display_Node* dispNode;	//the node that will display the virtual desktop tile to the associated physical tile
	Virtual_Desktop_Tile *next;


public:
	Virtual_Desktop_Tile();
	~Virtual_Desktop_Tile();
	float LowerLeftX();
	float LowerLeftY();
	int	LowerLeftPixelX();
	int LowerLeftPixelY();
	int WidthInPixels();
	int HeightInPixels();
	int NonMullionLLX();
	int NonMullionLLY();

	float WidthRatio();	//The ratio of the width of the tile to that of the whole virtual desktop
	float HeightRatio();	//The ratio of the height of the tile to that of the whole virtual desktop
	int ID();		//The unique ID of the tile
	Display_Node* DisplayNode();	//The physical tile that this virtual desktop tile resides
	Virtual_Desktop_Tile* Next();

	void LowerLeftX(float);
	void LowerLeftY(float);
	void NonMullionLLX(int);
	void NonMullionLLY(int);
	void LowerLeftPixelX(int);
	void LowerLeftPixelY(int);
	void WidthInPixels(int);
	void HeightInPixels(int);
	void WidthRatio(float);
	void HeightRatio(float);
	void ID(int);
	void DisplayNode(Display_Node*);
	void Next(Virtual_Desktop_Tile*);

};

class VDTtoPTmapper
{
	Physical_Tile* pt;
	Virtual_Desktop_Tile *vdt;

public:
	VDTtoPTmapper(Physical_Tile*, Virtual_Desktop_Tile*);
	~VDTtoPTmapper();
	Physical_Tile* PhysicalTile();
	Virtual_Desktop_Tile* VirtualDesktopTile();

};

class Display_Node
{
private:
	char* nodeName;		//the name of the computer that holds pixels to display
	char* ip;		//ip address of the computer the holds pixels to display
	VDTtoPTmapper**	maps;	//pointer to the array of VDTtoPTmapper objects
	Display_Node* next;	//next display node in list
	int numberOfMaps;	//The number of mappings from virtual desktop tile to physical tile
				//that this display node can make (i.e. how many outputs the display node has)
public:
	Display_Node();
	~Display_Node();
	void Name(char* );
	void IP(char* );
	char* Name(void);
	char* IP(void);
	int  NumberOfMaps();
	void NumberOfMaps(int);
	VDTtoPTmapper** Maps();
	bool AddMap(VDTtoPTmapper*);
	Display_Node* Next();
	void Next(Display_Node*);
	void RemoveMap(VDTtoPTmapper*);
	void RemoveMap(int);		//delete a specific mapping given an index
};


//__________________________________________

class DisplayCluster
{
protected:
	Display_Node*		displayNodeListHead;	//pointer to the display node linked list
	VirtualDesktop*		vd;
	PhysicalDisplay*	pd;

public:
	DisplayCluster();
	~DisplayCluster();
	void AddDisplayNode(Display_Node* );
	void RemoveDisplayNode(char*);		//remove a display node given the display node name
	void RemoveDisplayNodeByName(char*);
	void RemoveDisplayNodeByIP(char*);	//remove a display node given the display node ip
	Display_Node*	getFirstNode();		//return the first node in the display node list

	VirtualDesktop* VirtualDesktopAssoc();
	void VirtualDesktopAssoc(VirtualDesktop* );
	PhysicalDisplay* PhysicalDisplayAssoc();
	void PhysicalDisplayAssoc(PhysicalDisplay*);

	bool GenerateMapping(Virtual_Desktop_Tile*, Physical_Tile*);
	bool GenerateMapping(Virtual_Desktop_Tile*, Physical_Tile*, Display_Node*);
	void RemoveMapping(Virtual_Desktop_Tile*, Physical_Tile*, Display_Node*);
	void RemoveMapsWith(Virtual_Desktop_Tile*);	//Removes any mappings made witht the given virtual tile
	void RemoveMapsWith(Physical_Tile*);		//Removes any mappings made with the given physical tile
	bool NodeOnCluster(Display_Node*);

};


class PhysicalDisplay
{
protected:
	Physical_Tile* 	physicalTileListHead;	//pointer to the physical tile linked list

	VirtualDesktop*		vd;
	DisplayCluster*		dc;
	float				pixelsPerInch;		//the pixels per inch ratio

public:
	PhysicalDisplay();
	~PhysicalDisplay();
	VirtualDesktop* VirtualDesktopAssoc();
	void VirtualDesktopAssoc(VirtualDesktop* );
	DisplayCluster* DisplayClusterAssoc();
	void DisplayClusterAssoc(DisplayCluster* );
	int AddPhysicalTile(Physical_Tile*);				//adds a tile to the display and returns its unique id

	Physical_Tile* RemovePhysicalTileByID(int, bool);		//remove a physical tile from this physical display
									//the second parameter is used to have the tile deleted
									//or just removed.  If "true" the tile will be deleted
									//from memory, otherwise the address of the tile will
									//be returned

	Physical_Tile* getFirstNode();		//returns the first node in the physical tile list
	bool TileOnPhysicalDisplay(Physical_Tile*);
	float PixelsPerInch();				//get the pixels per inch that ALL the physical tiles on this display conform to
	void PixelsPerInch(float);			//set the pixels per inch that ALL the physical tiles on this display conform to
};

class VirtualDesktop
{
protected:
	int horizRes;		//horizontal resolution	of virtual desktop
	int vertRes;		//vertical resolution of virtual desktop

	//pointer to Virtual_Desktop_Tile linked list
	Virtual_Desktop_Tile* tileListHead;

	PhysicalDisplay*	pd;
	DisplayCluster*		dc;


public:
	VirtualDesktop();
	~VirtualDesktop();

	PhysicalDisplay* PhysicalDisplayAssoc();
	void PhysicalDisplayAssoc(PhysicalDisplay*);
	DisplayCluster* DisplayClusterAssoc();
	void DisplayClusterAssoc(DisplayCluster*);

	int HorizRes(void){ return horizRes; }
	int VertRes(void){ return vertRes; }

	void AddTile(Virtual_Desktop_Tile*);
	void RemoveTile(Virtual_Desktop_Tile*);

	Virtual_Desktop_Tile* getFirstNode(void);

	bool Update(void);		//update the virtual desktop with newly added/removed tile mappings

};

#endif
