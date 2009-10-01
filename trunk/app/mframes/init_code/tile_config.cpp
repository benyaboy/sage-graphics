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
-	Module: tile_config.cpp
-	Purpose: Contains all class details/implementation for building
-	a description of a tile display system, at run time
---------------------------------------------------------------------------*/
#include "tile_config.h"
#include <string.h>


//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------

Physical_Tile::Physical_Tile():screenWidth(0),screenHeight(0),
				leftMullion(0),rightMullion(0),topMullion(0),bottomMullion(0),
				id(-1),next(NULL),source(NULL)
{
	//nothing else to do
}

Physical_Tile::~Physical_Tile()
{

	source = NULL;
	next = NULL;

}

void Physical_Tile::Source(Display_Node* dn)
{
	source = dn;
}

Display_Node* Physical_Tile::Source()
{ return source; }

float Physical_Tile::LowerLeftX(){ return lowerleftX; }
float Physical_Tile::LowerLeftY(){ return lowerleftY; }

void Physical_Tile::LowerLeftX(float lx){ lowerleftX = lx; }
void Physical_Tile::LowerLeftY(float ly){ lowerleftY = ly; }


float Physical_Tile::ScreenWidthDimension()
{ return screenWidth; }

void Physical_Tile::ScreenWidthDimension(float dim)
{ screenWidth = dim; }

float Physical_Tile::ScreenHeightDimension(void)
{ return screenHeight; }

void Physical_Tile::ScreenHeightDimension(float dim)
{ screenHeight = dim; }

float Physical_Tile::TopMullionThickness()
{ return topMullion; }

void Physical_Tile::TopMullionThickness(float t)
{ topMullion = t; }

float Physical_Tile::BottomMullionThickness()
{ return bottomMullion; }

void Physical_Tile::BottomMullionThickness(float t)
{ bottomMullion = t; }

float Physical_Tile::LeftMullionThickness()
{ return leftMullion; }

void Physical_Tile::LeftMullionThickness(float t)
{ leftMullion = t; }

float Physical_Tile::RightMullionThickness()
{ return rightMullion; }

void Physical_Tile::RightMullionThickness(float t)
{ rightMullion = t; }

int Physical_Tile::ID()
{ return id; }

void Physical_Tile::ID(int i)
{ id = i; }

Physical_Tile* Physical_Tile::Next()
{ return next; }

void Physical_Tile::Next(Physical_Tile* n)
{ next = n; }
//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------

Virtual_Desktop_Tile::Virtual_Desktop_Tile():lowerLeftX(0),lowerLeftY(0),widthRatio(0),
						heightRatio(0),id(-1),next(NULL),nonMullionLLX(0),
						nonMullionLLY(0)
{
	//do nothing
}

Virtual_Desktop_Tile::~Virtual_Desktop_Tile()
{

	next = NULL;
}

float Virtual_Desktop_Tile::LowerLeftX()
{ return lowerLeftX; }

void Virtual_Desktop_Tile::LowerLeftX(float x)
{ lowerLeftX = x; }

float Virtual_Desktop_Tile::LowerLeftY()
{ return lowerLeftY; }

void Virtual_Desktop_Tile::LowerLeftY(float y)
{ lowerLeftY = y; }

void Virtual_Desktop_Tile::LowerLeftPixelX(int p)
{ lowerLeftPixelX = p; }

void Virtual_Desktop_Tile::LowerLeftPixelY(int p)
{ lowerLeftPixelY = p; }

void Virtual_Desktop_Tile::WidthInPixels(int p)
{ widthInPixels = p; }

void Virtual_Desktop_Tile::HeightInPixels(int p)
{ heightInPixels = p; }

int Virtual_Desktop_Tile::LowerLeftPixelX()
{ return lowerLeftPixelX; }

int Virtual_Desktop_Tile::LowerLeftPixelY()
{ return lowerLeftPixelY; }

int Virtual_Desktop_Tile::WidthInPixels()
{ return widthInPixels; }

int Virtual_Desktop_Tile::HeightInPixels()
{ return heightInPixels; }

float Virtual_Desktop_Tile::WidthRatio()
{ return widthRatio; }

void Virtual_Desktop_Tile::WidthRatio(float r)
{ widthRatio = r; }

float Virtual_Desktop_Tile::HeightRatio()
{ return heightRatio; }

void Virtual_Desktop_Tile::HeightRatio(float r)
{ heightRatio = r; }

int Virtual_Desktop_Tile::ID()
{ return id; }

void Virtual_Desktop_Tile::ID(int i)
{ id = i; }

Virtual_Desktop_Tile* Virtual_Desktop_Tile::Next()
{ return next; }

void Virtual_Desktop_Tile::Next(Virtual_Desktop_Tile* vdt)
{ next = vdt; }

Display_Node* Virtual_Desktop_Tile::DisplayNode()
{ return dispNode; }

void Virtual_Desktop_Tile::DisplayNode(Display_Node* dn)
{ dispNode = dn; }

int Virtual_Desktop_Tile::NonMullionLLX()
{ return nonMullionLLX; }

int Virtual_Desktop_Tile::NonMullionLLY()
{ return nonMullionLLY; }

void Virtual_Desktop_Tile::NonMullionLLX(int x)
{ nonMullionLLX = x; }

void Virtual_Desktop_Tile::NonMullionLLY(int y)
{ nonMullionLLY = y; }

//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------
VDTtoPTmapper::VDTtoPTmapper(Physical_Tile* tpt, Virtual_Desktop_Tile* tvdt):pt(tpt),vdt(tvdt)
{ }

VDTtoPTmapper::~VDTtoPTmapper()
{ }

Physical_Tile* VDTtoPTmapper::PhysicalTile(){ return pt; }

Virtual_Desktop_Tile* VDTtoPTmapper::VirtualDesktopTile(){ return vdt; }

//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------

Display_Node::Display_Node():nodeName(NULL),ip(NULL),next(NULL),maps(NULL),numberOfMaps(0)
{

}

Display_Node::~Display_Node()
{

	if( maps && numberOfMaps)
	{


	}

	if( ip )
		delete [] ip;

	if( nodeName )
		delete [] nodeName;

	next = NULL;
}

void Display_Node::Name(char* newName)
{
	if( nodeName )
		delete [] nodeName;

	nodeName = new char[strlen(newName) + 1];
	strcpy(nodeName,newName);
}

void Display_Node::IP(char* newIP)
{
	if( ip )
		delete [] ip;

	ip = new char[strlen(newIP) + 1];
	strcpy(ip,newIP);
}

char* Display_Node::Name(){ return nodeName; }
char* Display_Node::IP(){ return ip; }
Display_Node* Display_Node::Next(){ return next; }
void Display_Node::Next(Display_Node* dn){ next = dn; }

int Display_Node::NumberOfMaps(){ return numberOfMaps; }
void Display_Node::NumberOfMaps(int n)	//need to adjust maps
{
	VDTtoPTmapper** tempMap = maps;
	maps = NULL;
	maps = new VDTtoPTmapper*[n];

	int i = 0;

	//copy over as many nodes as the new maps array can fit
	for(	; i < n; i++)
	{
		if( i < numberOfMaps && tempMap)
		{
			maps[i] = tempMap[i];
			tempMap[i] = NULL;
		}
		else
			maps[i] = NULL;
	}

	//clean up any nodes left over in the old maps array
	for(	; i < numberOfMaps && tempMap; i++)
	{
		tempMap[i]->VirtualDesktopTile()->DisplayNode(NULL);
		tempMap[i]->PhysicalTile()->Source(NULL);

	}

	delete [] tempMap;
	numberOfMaps = n;

}


VDTtoPTmapper** Display_Node::Maps(){ return maps; }

bool Display_Node::AddMap(VDTtoPTmapper* pMap)
{

	for(int i = 0; i < numberOfMaps; i++)
	{
		if(maps[i] == NULL)
		{
			maps[i] = pMap;
			maps[i]->VirtualDesktopTile()->DisplayNode(this);
			maps[i]->PhysicalTile()->Source(this);
			return true;

		}

	}

	return false;
}

void Display_Node::RemoveMap(VDTtoPTmapper* vpMap)
{
	for(int i = 0; i < numberOfMaps; i++)
	{
		if(maps[i] && maps[i] == vpMap)
		{
			delete maps[i];
			maps[i] = NULL;
		}
	}
}

void Display_Node::RemoveMap(int index)
{
	if(index < numberOfMaps && maps[index])
	{
		delete maps[index];
		maps[index] = NULL;
	}
}

//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------

DisplayCluster::DisplayCluster():displayNodeListHead(NULL)
{

}

DisplayCluster::~DisplayCluster()
{

}


void DisplayCluster::AddDisplayNode(Display_Node* dn)
{
	Display_Node* temp = displayNodeListHead;
	if(temp)
	{
		while(temp->Next())
		{
			temp = temp->Next();
		}

		temp->Next(dn);
		dn->Next(NULL);
	}
	else
	{
		displayNodeListHead = dn;
		dn->Next(NULL);

	}

}


void DisplayCluster::RemoveDisplayNode(char* name)
{

}

void DisplayCluster::RemoveDisplayNodeByName(char* name)
{
	RemoveDisplayNode(name);
}


void DisplayCluster::RemoveDisplayNodeByIP(char* ip)
{

}

Display_Node* DisplayCluster::getFirstNode()
{
	return this->displayNodeListHead;
}


//Generate and insert a mapping between virtual tile and physical tile to a display node
//Assuming the physical tile already knows the display node source
bool DisplayCluster::GenerateMapping(Virtual_Desktop_Tile* vdt, Physical_Tile* pt)
{
	//make sure the physical tile is on the physical display associated with this display cluster
	if(!pd->TileOnPhysicalDisplay(pt))
		return false;


	if(pt->Source() == NULL)
		return false;		//the physical tile must already have a source

	//make sure that the source node is on this display cluster
	if(!NodeOnCluster(pt->Source()))
		return false;

	VDTtoPTmapper* map = new VDTtoPTmapper(pt,vdt);
	if(pt->Source()->AddMap(map))
		return true;

	delete map;
	return false;

}

PhysicalDisplay* DisplayCluster::PhysicalDisplayAssoc(){ return pd; }
void DisplayCluster::PhysicalDisplayAssoc(PhysicalDisplay* tpd)
{
	pd = tpd;
}

//Generate and insert a mapping between virtual and physical tile using the given display node
bool DisplayCluster::GenerateMapping(Virtual_Desktop_Tile* vdt, Physical_Tile *pt, Display_Node *dn)
{
	//make sure the physical tile is on the physical display associated with this display cluster
	if(!pd->TileOnPhysicalDisplay(pt))
		return false;

	Display_Node *itr = displayNodeListHead;
	//make sure the node is on this display cluster
	if(!NodeOnCluster(dn))
		return false;

	VDTtoPTmapper* map = new VDTtoPTmapper(pt,vdt);
	if(dn->AddMap(map))
		return true;

	delete map;
	return false;

}

void DisplayCluster::RemoveMapping(Virtual_Desktop_Tile* vdt, Physical_Tile *pt, Display_Node* dn)
{
	if(!NodeOnCluster(dn))
		return;

	VDTtoPTmapper** maps = dn->Maps();
	int count = dn->NumberOfMaps();

	for(int i = 0; i < count; i++)
	{
		if(maps[i] && maps[i]->PhysicalTile() == pt && maps[i]->VirtualDesktopTile() == vdt)
			dn->RemoveMap(i);
	}

}

bool DisplayCluster::NodeOnCluster(Display_Node* dn)
{
	Display_Node* itr = displayNodeListHead;
	while(itr)
	{
		if(itr == dn)
		{
			return true;
		}
		itr = itr->Next();
	}

	return false;
}

void DisplayCluster::RemoveMapsWith(Virtual_Desktop_Tile* tvdt)
{
	Display_Node* itr = displayNodeListHead;
	while(itr)
	{
		VDTtoPTmapper** maps = itr->Maps();
		int count = itr->NumberOfMaps();
		for(	;count > 0; count--)
		{
			if(maps[count - 1] && maps[count - 1]->VirtualDesktopTile() == tvdt)
				itr->RemoveMap(count - 1);
		}

		itr = itr->Next();
	}
}

void DisplayCluster::RemoveMapsWith(Physical_Tile* tpt)
{
	Display_Node* itr = displayNodeListHead;
	while(itr)
	{
		VDTtoPTmapper** maps = itr->Maps();
		int count = itr->NumberOfMaps();
		for(	;count > 0; count--)
		{
			if(maps[count - 1] && maps[count - 1]->PhysicalTile() == tpt)
				itr->RemoveMap(count - 1);
		}

		itr = itr->Next();
	}
}

VirtualDesktop* DisplayCluster::VirtualDesktopAssoc(){ return vd; }
void DisplayCluster::VirtualDesktopAssoc(VirtualDesktop* tvd)
{

}

//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------


PhysicalDisplay::PhysicalDisplay():physicalTileListHead(NULL),vd(NULL),dc(NULL),
pixelsPerInch(1.0)
{


}

PhysicalDisplay::~PhysicalDisplay()
{

}

void PhysicalDisplay::VirtualDesktopAssoc(VirtualDesktop *tvd)
{
	vd = tvd;
}
VirtualDesktop* PhysicalDisplay::VirtualDesktopAssoc(){ return vd; }

int PhysicalDisplay::AddPhysicalTile(Physical_Tile* pt)
{
	int ID = 1;
	if(physicalTileListHead == NULL)
	{
		physicalTileListHead = pt;
	}
	else
	{
		Physical_Tile *itr = physicalTileListHead;
		while(itr->Next())
		{
			itr = itr->Next();
			ID++;
		}

		itr->Next(pt);
		pt->Next(NULL);
	}

	pt->ID(ID);
	return ID;
}

Physical_Tile* PhysicalDisplay::RemovePhysicalTileByID(int ID, bool delquery)
{

	Physical_Tile *itr = physicalTileListHead;

	if(itr->ID() == ID)
	{
		physicalTileListHead = itr->Next();

	}
	else
	{
		while(itr->Next())
		{
			//in order to take care of accidental redundant data, loop for each entry
			if(itr->Next()->ID() == ID)
			{
				itr->Next(itr->Next()->Next());
				if(dc)
					dc->RemoveMapsWith(itr);

			}
		}
	}

	if(delquery)
	{
		delete itr;
		return NULL;
	}
	else
		return itr;


}


DisplayCluster* PhysicalDisplay::DisplayClusterAssoc()
{ return dc; }

void PhysicalDisplay::DisplayClusterAssoc(DisplayCluster* tdc)
{
	//go through the current display cluster and remove all the mappings that were made to the
	//physical tiles associated with this display cluster
	Physical_Tile* itr = physicalTileListHead;
	while(itr)
	{
		if(dc)
			dc->RemoveMapsWith(itr);
		itr->Source(NULL);
		itr = itr->Next();
	}

	dc = tdc;


}

bool PhysicalDisplay::TileOnPhysicalDisplay(Physical_Tile* pt)
{
	Physical_Tile* itr = physicalTileListHead;
	while(itr)
	{
		if(itr == pt)
			return true;

		itr = itr->Next();
	}

	return false;
}
Physical_Tile* PhysicalDisplay::getFirstNode()
{ return physicalTileListHead; }

float PhysicalDisplay::PixelsPerInch(){ return pixelsPerInch; }
void PhysicalDisplay::PixelsPerInch(float ratio)
{ pixelsPerInch = ratio; }

//----------------------------------------------------------
//
//
//
//
//
//-----------------------------------------------------------

VirtualDesktop::VirtualDesktop():dc(NULL),pd(NULL),tileListHead(NULL),vertRes(1),horizRes(1)	//just 1 pixel by 1 pixel
{
}

VirtualDesktop::~VirtualDesktop()
{
	Virtual_Desktop_Tile* itr = tileListHead;
	while(itr)
	{
		if(dc)
			dc->RemoveMapsWith(itr);
		tileListHead = itr->Next();
		delete itr;
		itr = tileListHead;
	}
}

DisplayCluster* VirtualDesktop::DisplayClusterAssoc(){ return dc; }
void VirtualDesktop::DisplayClusterAssoc(DisplayCluster *tdc)
{
	dc = tdc;
}

PhysicalDisplay* VirtualDesktop::PhysicalDisplayAssoc(){ return pd; }
void VirtualDesktop::PhysicalDisplayAssoc(PhysicalDisplay* tpd)
{
	pd = tpd;
}

void VirtualDesktop::AddTile(Virtual_Desktop_Tile* tvdt)
{
	//the tile has to be already mapped to a physical tile,
	VDTtoPTmapper* mapping = NULL;
	if(!dc)
		return;

	Display_Node* itr = dc->getFirstNode();

	while(itr)
	{
		int count = itr->NumberOfMaps();
		VDTtoPTmapper** maps = itr->Maps();
		for( ;count > 0; count--)
		{
			if(maps[count - 1] && maps[count - 1]->VirtualDesktopTile() == tvdt)
			{
				mapping = maps[count - 1];
				itr = NULL;
				break;
			}
		}

		if(itr)
			itr = itr->Next();
	}

	if(!mapping)
		return;

	//there is a mapping...continue


	//put at end of list
	Virtual_Desktop_Tile* vdtItr = tileListHead;
	if(vdtItr == NULL)
	{
		tileListHead = tvdt;
		tvdt->Next(NULL);
		return;
	}

	while(vdtItr->Next())
	{
		vdtItr = vdtItr->Next();
	}

	vdtItr->Next(tvdt);
	tvdt->Next(NULL);

}

void VirtualDesktop::RemoveTile(Virtual_Desktop_Tile* tvdt)
{
	Virtual_Desktop_Tile* itr = tileListHead;
	while(itr)
	{
		if(itr == tvdt)
		{
			if(dc)
			{
				return dc->RemoveMapsWith(tvdt);
			}
		}
		itr = itr->Next();
	}

}

Virtual_Desktop_Tile* VirtualDesktop::getFirstNode(void)
{
	return tileListHead;
}

bool VirtualDesktop::Update()
{
	if(!pd)
		return false;


	float positiveHoriz, positiveVert, negativeHoriz, negativeVert;
	positiveHoriz = positiveVert = negativeHoriz = negativeVert = 0.0;
	float leftMullion, bottomMullion;
	//need to determine the whole virtual desktop size
	Physical_Tile* ptItr = pd->getFirstNode();
	while(ptItr)
	{
		if(ptItr->LowerLeftX() < negativeHoriz)
		{
			negativeHoriz = ptItr->LowerLeftX();
			leftMullion = ptItr->LeftMullionThickness();
		}
		else if(ptItr->LowerLeftX() + ptItr->ScreenWidthDimension() > positiveHoriz)
		{
			positiveHoriz = ptItr->LowerLeftX() + ptItr->ScreenWidthDimension();
		}

		if(ptItr->LowerLeftY() < negativeVert)
		{
			negativeVert = ptItr->LowerLeftY();
			bottomMullion = ptItr->BottomMullionThickness();

		}
		else if(ptItr->LowerLeftY() + ptItr->ScreenHeightDimension() > positiveVert)
		{
			positiveVert = ptItr->LowerLeftY() + ptItr->ScreenHeightDimension();
		}

		ptItr = ptItr->Next();
	}

	//save off the resolutions.
	horizRes = (int)((positiveHoriz - negativeHoriz) * (pd->PixelsPerInch()));
	vertRes = (int)((positiveVert - negativeVert) * (pd->PixelsPerInch()));


	//go through all the virtual tiles and adjust their properties based on their mapped physical tiles
	Virtual_Desktop_Tile* vdtItr = tileListHead;
	while(vdtItr)
	{
		VDTtoPTmapper* mapping;

		mapping = NULL;
		if(!dc)
			return false;

		Display_Node* itr = dc->getFirstNode();

		while(itr)
		{
			int count = itr->NumberOfMaps();
			VDTtoPTmapper** maps = itr->Maps();
			for( ;count > 0; count--)
			{
				if(maps[count - 1] && maps[count - 1]->VirtualDesktopTile() == vdtItr)
				{
					mapping = maps[count - 1];
					itr = NULL;
					break;
				}
			}

			if(itr)
				itr = itr->Next();
		}

		//we have an unmapped virtual tile....doesn't make sense, stop and return false
		if(!mapping)
			return false;

		//calculate the virtual tile properties
		float posXRatio, posYRatio, widthRatio, heightRatio;
		posXRatio = posYRatio = widthRatio = heightRatio = 0.0;
		Physical_Tile* ptTemp = mapping->PhysicalTile();

		vdtItr->LowerLeftPixelX( (ptTemp->LowerLeftX() + ptTemp->LeftMullionThickness())  * pd->PixelsPerInch() );
		vdtItr->LowerLeftPixelY( (ptTemp->LowerLeftY() + ptTemp->BottomMullionThickness()) * pd->PixelsPerInch() );
		vdtItr->LowerLeftX( (float)vdtItr->LowerLeftPixelX() / horizRes);
		vdtItr->LowerLeftY( (float)vdtItr->LowerLeftPixelY() / vertRes);

		vdtItr->WidthInPixels( ptTemp->ScreenWidthDimension() * pd->PixelsPerInch() );
		vdtItr->HeightInPixels( ptTemp->ScreenHeightDimension() * pd->PixelsPerInch());
		vdtItr->WidthRatio( (float)vdtItr->WidthInPixels() / horizRes);
		vdtItr->HeightRatio( (float)vdtItr->HeightInPixels() / vertRes);

		vdtItr->ID( ptTemp->ID());	//map the id of the virtual tile to the same id of the physical tile

		//move on
		vdtItr = vdtItr->Next();

	}


	return true;

}
