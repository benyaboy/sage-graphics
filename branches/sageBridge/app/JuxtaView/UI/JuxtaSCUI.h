/*=============================================================================

  Program: JuxtaView for SAGE
  Module:  JuxtaSCUI.h - Part of JuxtaView's UI
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

#ifndef JUXTASCUI_H
#define JUXTASCUI_H

#include <wx/wx.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include <QUANTA/QUANTAinit.hxx>
#include <QUANTA/QUANTAnet_tcp_c.hxx>
#include <QUANTA/QUANTAnet_datapack_c.hxx>

#include <wx/frame.h>
#include <iostream>
using namespace std;

#include <wx/panel.h>
#include <wx/button.h>
#include <wx/image.h>	//for loading in images
#include <wx/bitmap.h>	//for drawing images
#include <wx/bmpbuttn.h>	//for placing images on buttons...deprecaited method
#include <wx/statbmp.h>		//new header for drawing images...

#include "JuxtaSCUICommon.h"
#define DEBUG 1
//list ID's for buttons
enum
{
	ID_PanLeft = 1000,
	ID_PanRight,
	ID_PanDown,
	ID_PanUp,
	ID_FastPanLeft,
	ID_FastPanRight,
	ID_FastPanDown,
	ID_FastPanUp,
	ID_ZoomOut,
	ID_ZoomIn,
	ID_Quit
};

struct SOverview
{
	unsigned char* imgArray;
	int w;
	int h;
	float normX;
	float normY;
	float normW;
	float normH;

};

#include <wx/dcclient.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

#define OVERVIEW_MENU_ITEM_COUNT 3
#define OVERVIEW_MENU_LABEL_LENGTH 21
enum{
	ID_ChangePenColor = 0,
	ID_ZoomOutFromMouse,
	ID_ZoomInFromMouse
};

class JuxtaOverviewFrame : public wxFrame
{
private:
	SOverview data;
	wxImage* img;
	wxBitmap* bmptodraw;
	wxPaintDC* pdc;
	wxColour penColor;
public:
	JuxtaOverviewFrame( wxWindow* parent, int id, const wxString& title,
				const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize,
				long style=wxDEFAULT_FRAME_STYLE);
	~JuxtaOverviewFrame();
	void SetImage(int w, int h, unsigned char* pixels);
	void SetExtentRectangle(float nx, float ny, float nw, float nh);
	void OnPaint(wxPaintEvent& event);
	void OnDropDownMenu(wxMouseEvent& event);
	void OnMenuSelection(wxCommandEvent& event);
	void OnLeftMouseUp(wxMouseEvent& event);

	DECLARE_EVENT_TABLE();
};

//-------------------------------------------
class JuxtaSCUIFrame : public wxFrame
{
private:
	QUANTAnet_tcpClient_c *client;
	wxStaticBitmap* statbmp;
	JuxtaOverviewFrame* overviewframe;
	bool showoverview;
	wxMutex clientMutex;
public:
	JuxtaSCUIFrame( bool ov, char* skinfilename, wxWindow* parent, int id, const wxString& title,
					const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize,
					long style=wxDEFAULT_FRAME_STYLE);

	~JuxtaSCUIFrame();
	void Init(char* );	//pass config filename
	void OnEvent(wxCommandEvent& event);
	void GetExtents();
	void GetOverview();
	virtual void Show();
	void PassEvent(char*);
	void PassBuffer(void*,int);

	DECLARE_EVENT_TABLE();
};

extern JuxtaSCUIFrame *gMainFrame;
extern wxApp* gApp;

#endif
