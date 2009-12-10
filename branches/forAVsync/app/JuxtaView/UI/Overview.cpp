/*=============================================================================

  Program: JuxtaView for SAGE
  Module:  Overview.cpp - Part of JuxtaView's UI
  Authors: Arun Rao, arao@evl.uic.edu,
           Ratko Jagodic, rjagodic@evl.uic.edu,
           Nicholas Schwarz, schwarz@evl.uic.edu,
           et al.
  Date:    30 September 2004
  Modified: 28 October 2004

  Copyright (c) 2005 Electronic Visualization Laboratory,
                     University of Illinois at Chicago

  All rights reserved.
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

  Direct questions, comments, etc. to schwarz@evl.uic.edu or
  http://www.evl.uic.edu/cavern/forum/

=============================================================================*/

#include "JuxtaSCUI.h"


JuxtaOverviewFrame::JuxtaOverviewFrame(wxWindow* parent, int id, const wxString& title,
					const wxPoint& pos, const wxSize& size,
					long style)
			:wxFrame(parent,id,title,pos,size,style)
{
	data.imgArray = NULL;
	img = NULL;
	bmptodraw = NULL;
	penColor.Set(255,255,255);

}

BEGIN_EVENT_TABLE( JuxtaOverviewFrame, wxFrame )
	EVT_PAINT( JuxtaOverviewFrame::OnPaint )
	EVT_RIGHT_UP( JuxtaOverviewFrame::OnDropDownMenu )
	EVT_MENU( ID_ChangePenColor, JuxtaOverviewFrame::OnMenuSelection )
	EVT_MENU( ID_ZoomOutFromMouse, JuxtaOverviewFrame::OnMenuSelection )
	EVT_MENU( ID_ZoomInFromMouse, JuxtaOverviewFrame::OnMenuSelection )
	EVT_LEFT_UP( JuxtaOverviewFrame::OnLeftMouseUp )
END_EVENT_TABLE()

JuxtaOverviewFrame::~JuxtaOverviewFrame()
{


}

void JuxtaOverviewFrame::SetImage(int w, int h, unsigned char* pixels)
{
	SetSize(w,h);

	//get the data
	data.w = w;
	data.h = h;
	if(data.imgArray)
		delete [] data.imgArray;
	data.imgArray = pixels;

	//make an image out of it
	if(img)
		delete img;

	img = new wxImage(data.w,data.h,data.imgArray,TRUE);

	if(bmptodraw)
		delete bmptodraw;

	bmptodraw = new wxBitmap(img);

	Refresh();


}

void JuxtaOverviewFrame::SetExtentRectangle(float nx, float ny, float nw, float nh)
{
	data.normX = nx;
	data.normY = ny;
	data.normW = nw;
	data.normH = nh;
}

void JuxtaOverviewFrame::OnPaint(wxPaintEvent& event)
{
	pdc = new wxPaintDC(this);
	wxPen pen(penColor,5,wxDOT_DASH);
	pdc->SetPen(pen);
	pdc->BeginDrawing();
		if(bmptodraw)
		{
			fprintf(stderr,"Bitmap valid! Drawing\n");
			pdc->DrawBitmap((*bmptodraw),0,0,TRUE);
			//draw some lines with the extents
		//	pdc->GetPen().SetJoin(wxJOIN_ROUND);
		//	pdc->GetPen().SetWidth(10);
		//	pdc->GetPen().SetCap(wxCAP_PROJECTING);
		//	pdc->GetPen().SetColour(255,255,255);
		//	pdc->GetPen().SetStyle(wxCROSSDIAG_HATCH);

			int x0,y0,x1,y1;
			x0 = (int)(data.normX * data.w);
			y0 = (int)(data.normY * data.h);
			x1 = (int)(x0 + (data.normW * data.w));
			y1 = (int)(y0 + (data.normH * data.h));

			pdc->DrawLine( x0,y0,x1,y0);
			pdc->DrawLine( x0,y0,x0,y1);
			pdc->DrawLine( x1,y0,x1,y1);
			pdc->DrawLine( x0,y1,x1,y1);

		}
	pdc->EndDrawing();

	delete pdc;
}


char OverviewMenuLabels[OVERVIEW_MENU_ITEM_COUNT][OVERVIEW_MENU_LABEL_LENGTH] =
	{ "Change Border Color", "Zoom Out","Zoom In" };

void JuxtaOverviewFrame::OnDropDownMenu(wxMouseEvent& event)
{
	//make the drop down menu
	wxMenu *menu = new wxMenu();
	for ( int i = 0; i < OVERVIEW_MENU_ITEM_COUNT; i++)
	{
		menu->Append( i, wxString(OverviewMenuLabels[i]));
	}

	PopupMenu(menu,event.GetX(),event.GetY());
}

void JuxtaOverviewFrame::OnMenuSelection(wxCommandEvent& event)
{
	wxColourDialog *dlg;
	switch( event.GetId())
	{
	case ID_ChangePenColor:
		fprintf(stderr,"Opening Color Dialog!\n");
		//change the color of the pen
		dlg = new wxColourDialog(this);
		if( dlg->ShowModal() == wxID_OK)
		{
			unsigned char red, green, blue;
			red = dlg->GetColourData().GetColour().Red();
			green = dlg->GetColourData().GetColour().Green();
			blue = dlg->GetColourData().GetColour().Blue();

			penColor.Set(red,green,blue);
			Refresh();
		}

		dlg->Destroy();
		break;
	case ID_ZoomOutFromMouse:
		if(gMainFrame)
		{
			gMainFrame->PassEvent(JVZOOM_OUT);
			gMainFrame->GetExtents();
			Refresh();
		}
		break;
	case ID_ZoomInFromMouse:
		if(gMainFrame)
		{
			gMainFrame->PassEvent(JVZOOM_IN);
			gMainFrame->GetExtents();
			Refresh();
		}
		break;
	default:
		break;
	}
}


void JuxtaOverviewFrame::OnLeftMouseUp(wxMouseEvent& event)
{
	//figure out what normalized coordinates to send to the UI server...
	float coords[2] = { 0.0, 0.0 };
	fprintf(stderr,"Left mouse up at %d, %d\n",event.GetX(),event.GetY());
	coords[0] = ((float)event.GetX() / (float)data.w) - (data.normW / 2.0);
	coords[1] = ((float)event.GetY() / (float)data.h) - (data.normH / 2.0);

	if( coords[0] + data.normW > 1 )
	{
		coords[0] += 1 - ( coords[0] + data.normW );
	}
	else if( coords[0] < 0 )
	{
		coords[0] = 0;
	}

	if( coords[1] + data.normH > 1 )
	{
		coords[1] += 1 - ( coords[1] + data.normH );
	}
	else if( coords[1] < 0 )
	{
		coords[1] = 0;
	}

	fprintf(stderr,"Passing normalized coords %f, %f to server\n",coords[0],coords[1]);
	if( gMainFrame)
	{
		gMainFrame->PassEvent(JVTRANSLATE);
		gMainFrame->PassBuffer((void*)coords,sizeof(float) * 2);

		gMainFrame->GetExtents();
	}

	Refresh();

}

