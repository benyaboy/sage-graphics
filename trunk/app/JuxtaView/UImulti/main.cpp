/*=============================================================================

  Program: JuxtaView for SAGE
  Module:  main.cpp - Part of JuxtaView's UI
  Authors: Arun Rao, arao@evl.uic.edu,
           Ratko Jagodic, rjagodic@evl.uic.edu,
           Nicholas Schwarz, schwarz@evl.uic.edu,
           Luc Renambot, luc@evl.uic.edu,
           et al.
  Date:    30 September 2004
  Modified: 15 August 2005

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
#include <wx/wx.h>
#include <wx/frame.h>

JuxtaSCUIFrame* gMainFrame = NULL;
wxApp* gApp = NULL;

class JuxtaViewSCUIApp : public wxApp
{
public:
    bool OnInit();
	int OnExit();
};

IMPLEMENT_APP(JuxtaViewSCUIApp)


bool JuxtaViewSCUIApp::OnInit()
{
	bool overview = true;

	gApp = (wxApp*) this;
	char* skinfile = NULL;
	/*
	if( argc > 1 )
		skinfile = argv[1];
	*/
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS) || defined(_WINDOWS_)
	JuxtaSCUIFrame*  mainFrame = new JuxtaSCUIFrame(overview,skinfile,0,-1,"JuxtaView SC",wxPoint(0,0),wxSize(405,430));
#elif defined(MAC_OSX)
	JuxtaSCUIFrame*  mainFrame = new JuxtaSCUIFrame(overview,skinfile,0,-1,"JuxtaView SC",wxPoint(0,0),wxSize(400,420));
#else
	JuxtaSCUIFrame*  mainFrame = new JuxtaSCUIFrame(overview,skinfile,0,-1,"JuxtaView SC",wxPoint(0,0),wxSize(400,400));
#endif
	mainFrame->Init(wxApp::argv[1]);
	mainFrame->Layout();
	mainFrame->Show();

	gMainFrame = mainFrame;

	return true;
}

int JuxtaViewSCUIApp::OnExit()
{
	return 0;
}
