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

#include "wx/wx.h"
#include "wx/statline.h"

#include "sail.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
//#include <Carbon/Carbon.h>

#include <sys/time.h>

#include "libdxt.h"


// headers for SAGE
GLubyte *rgbBuffer = NULL;
GLubyte *dxtBuffer = NULL;
sail sageInf; // sail object

class MyApp: public wxApp
{
virtual bool OnInit();
};


class MyFrame: public wxFrame
{
public:

  MyFrame(const wxString& title);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnStream(wxCommandEvent& event);
  void OnStop(wxCommandEvent& event);
  void OnSlide(wxScrollEvent& event);
  void OnAddress(wxCommandEvent& event);
  void OnCompressionNo(wxCommandEvent& event);
  void OnCompression(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()

private:
  double startt;
  int count;

  wxTimer m_timer;
  int FrameRate;
  wxString Address;
  int started;
  wxCheckBox *rb1, *rb2;
  int Compression;
  CGLContextObj  glContextObj;
  int WW, HH;
  void capture(char* m_pFrameRGB,int x,int y,int cx,int cy);

};


enum
{
	ID_Quit = 100,
	ID_About,
	TIMER_ID,
	STREAM_ID,
	STOP_ID,
	SLIDER_ID,
	ADDRESS_ID,
	COMPRESS_NO_ID, COMPRESS_ID
};


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_Quit, MyFrame::OnQuit)
	EVT_MENU(ID_About, MyFrame::OnAbout)
	EVT_TIMER(TIMER_ID, MyFrame::OnTimer)
	EVT_BUTTON(STREAM_ID,MyFrame::OnStream)
	EVT_BUTTON(STOP_ID,MyFrame::OnStop)
	EVT_COMMAND_SCROLL(SLIDER_ID,MyFrame::OnSlide)
	EVT_TEXT(ADDRESS_ID, MyFrame::OnAddress)
	EVT_CHECKBOX(COMPRESS_NO_ID, MyFrame::OnCompressionNo)
	EVT_CHECKBOX(COMPRESS_ID, MyFrame::OnCompression)
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame( _T("iShare") );
  frame->Show(true);
  SetTopWindow(frame);

   char *sageDir = getenv("SAGE_DIRECTORY");
   if (!sageDir) {
      std::cout << "sageVirtualDesktop : cannot find the environment variable SAGE_DIRECTORY" << std::endl;
      return -1;
   }
   int len;
   char *dir;
   len = strlen(sageDir);
   dir = (char*)malloc(len+64);
   memset(dir, 0, len+64);
   sprintf(dir, "%s/bin", sageDir);
   chdir(dir);


  return true;
}

void MyFrame::capture(char* m_pFrameRGB,int x,int y,int cx,int cy)
{
  CGLSetCurrentContext( glContextObj ) ;
  //CGLSetFullScreen( glContextObj ) ;///UUUUUUUUUUnbelievable
  //glReadBuffer(GL_FRONT);

  if (Compression)
	  glReadPixels(x,y,cx,cy,GL_RGBA,GL_UNSIGNED_BYTE,m_pFrameRGB);
  else
	  glReadPixels(x,y,cx,cy,GL_RGB,GL_UNSIGNED_BYTE,m_pFrameRGB);

  CGLSetCurrentContext( NULL );
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
  if (started) {
      if (Compression)
	{
	      capture((char*)rgbBuffer,0,0,WW,HH);
	      CompressDXT(rgbBuffer, dxtBuffer, WW, HH, FORMAT_DXT1, 1);
	      sageInf.swapBuffer();
	      dxtBuffer = (GLubyte *)sageInf.getBuffer();
	}
	else
	{
	      capture((char*)rgbBuffer,0,0,WW,HH);
	      sageInf.swapBuffer();
	      rgbBuffer = (GLubyte *)sageInf.getBuffer();
	}

      sageMessage msg;
      if (sageInf.checkMsg(msg, false) > 0) {
	switch (msg.getCode()) {
	case APP_QUIT:
	  Close(true);
	  break;
	}
      }


	wxString str;
	double nowt = aTime();
	double fps = 1.0 / (nowt - startt);
	startt = nowt;
	str.Printf(wxT("Interval %d ms / Rate %.2f / Frame %d / Compression %d / Desktop %dx%d"),
			event.GetInterval(),fps,count++, Compression, WW, HH);
	SetStatusText( str );

  }
}


void MyFrame::OnStream(wxCommandEvent& event)
{
  if (!started) {

	FindWindow(ADDRESS_ID)->Disable();
	FindWindow(STREAM_ID)->Disable();
	FindWindow(COMPRESS_NO_ID)->Disable();
	FindWindow(COMPRESS_ID)->Disable();

  wxString str;
  str.Printf(wxT("Stream !"));
  SetStatusText( str );
  started = 1;


  sageRect ishareImageMap;
  ishareImageMap.left = 0.0;
  ishareImageMap.right = 1.0;
  ishareImageMap.bottom = 0.0;
  ishareImageMap.top = 1.0;

  sailConfig scfg;
  scfg.init("ishare.conf");
  scfg.setAppName("ishare");
  scfg.rank = 0;

  scfg.resX = WW;
  scfg.resY = HH;
  scfg.winWidth  = 2*WW;
  scfg.winHeight = 2*HH;
  scfg.imageMap = ishareImageMap;

  if (Compression)
	  scfg.pixFmt = PIXFMT_DXT; // PIXFMT_888;
  else
	  scfg.pixFmt = PIXFMT_888;

  scfg.rowOrd = BOTTOM_TO_TOP;
  scfg.master = true;

  sageInf.init(scfg);

  if (Compression) {
	  dxtBuffer = (GLubyte *)sageInf.getBuffer();
	  if (rgbBuffer) delete [] rgbBuffer;
	  rgbBuffer = (byte*)memalign(16, WW*HH*4);
	  memset(rgbBuffer, 0,  WW*HH*4);
  }
  else {
	  rgbBuffer = (GLubyte *)sageInf.getBuffer();
  }

  // xx second interval
  m_timer.Start(1000.0/FrameRate, wxTIMER_CONTINUOUS);
  }
}

void MyFrame::OnStop(wxCommandEvent& event)
{
  wxString str;
  str.Printf(wxT("Stop !"));
  SetStatusText( str );
  started = 0;
}

void MyFrame::OnSlide(wxScrollEvent& event)
{
  FrameRate = event.GetPosition();
  m_timer.Start(1000.0/FrameRate, wxTIMER_CONTINUOUS);
}

void MyFrame::OnAddress(wxCommandEvent& event)
{
  Address = event.GetString();
  SetStatusText( Address );
}

void MyFrame::OnCompression(wxCommandEvent& event)
{
  if (!started) {
  if (event.IsChecked()) {
    rb2->SetValue(false);
    Compression = 1;
  }
  else {
    rb2->SetValue(true);
    Compression = 0;
  }
  }
}

void MyFrame::OnCompressionNo(wxCommandEvent& event)
{
  if (event.IsChecked()) {
    rb1->SetValue(false);
    Compression = 0;
  }
  else {
    rb1->SetValue(true);
    Compression = 1;
  }
}


MyFrame::MyFrame(const wxString& title)
  : wxFrame((wxFrame *)NULL,wxID_ANY, title, wxDefaultPosition, wxSize(450, 330)),
    m_timer(this, TIMER_ID), FrameRate(5), started(0), Compression(1)
{
        // Initialize some timing functions and else (DXT)
  aInitialize();

  startt = aTime();
  count = 0;

  wxColour col1, col2;
  col1.Set(wxT("#e8e8e8"));
  col2.Set(wxT("#e8e8e8"));

  // Main panel
  wxPanel *panel = new wxPanel(this, -1);
  panel->SetBackgroundColour(col1);

  // Vertical sizer
  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

  // Panel inside the sizer
  wxPanel *midPan = new wxPanel(panel, wxID_ANY);
  midPan->SetBackgroundColour(col2);

  vbox->Add(midPan, 1, wxEXPAND | wxALL, 20);
  panel->SetSizer(vbox);


  // Labels with text box
  wxGridSizer *hbox1 = new wxGridSizer(2,2, 10,10);

  Address = wxT("131.193.78.203");

  wxStaticText *st1 =  new wxStaticText(midPan, wxID_ANY, wxT("SAGE IP address"), wxDefaultPosition, wxSize(128,20), wxALIGN_RIGHT);
  hbox1->Add(st1, 1, wxRIGHT);
  wxTextCtrl *tc1 = new wxTextCtrl(midPan, ADDRESS_ID, Address, wxDefaultPosition, wxSize(128,20));
  hbox1->Add(tc1, 1, wxLEFT|wxRIGHT);
  hbox1->Add(-1, 0);
  hbox1->Add(-1, 0);

  // Slider with text box
  wxStaticText *st2 =  new wxStaticText(midPan, wxID_ANY, wxT("Frame rate"), wxDefaultPosition, wxSize(128,20), wxALIGN_RIGHT);
  hbox1->Add(st2, 1, wxRIGHT);
  wxSlider *tc2 = new wxSlider(midPan, SLIDER_ID, FrameRate, 1, 30, wxDefaultPosition, wxSize(128,20),
			       wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS);
  hbox1->Add(tc2, 1, wxLEFT);

  // Label with text box
  wxStaticText *st3 =  new wxStaticText(midPan, wxID_ANY, wxT("Compression"), wxDefaultPosition, wxSize(128,20), wxALIGN_RIGHT);
  hbox1->Add(-1, 4);
  hbox1->Add(-1, 4);
  hbox1->Add(st3, 1, wxRIGHT);

  // Radio buttons
  wxBoxSizer *hbox4 = new wxBoxSizer(wxHORIZONTAL);
  rb1 = new wxCheckBox(midPan, COMPRESS_ID, wxT("On"));
  rb1->SetValue(true);
  rb2 = new wxCheckBox(midPan, COMPRESS_NO_ID, wxT("Off"));
  hbox4->Add(rb1, 1, wxEXPAND);
  hbox4->AddSpacer(20);
  hbox4->Add(rb2, 1, wxEXPAND);
  hbox1->Add(hbox4, 1, wxALIGN_LEFT);



  // Line
  wxStaticLine *line1 = new wxStaticLine(midPan, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);


  // Two buttons
  wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);

  wxButton *ok = new wxButton(midPan, STREAM_ID, wxT("Stream"));
  wxButton *cancel = new wxButton(midPan, STOP_ID, wxT("Stop"));

  hbox3->Add(ok, 1, wxEXPAND | wxALL, 20);
  hbox3->Add(cancel, 1, wxEXPAND | wxALL, 20);

  // Vertical sizer for elements
  wxBoxSizer *vbox2 = new wxBoxSizer(wxVERTICAL);

  vbox2->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
  vbox2->Add(-1, 40);

  vbox2->Add(line1, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
  vbox2->Add(-1, 10);

  vbox2->Add(hbox3, 1, wxEXPAND);

  midPan->SetSizer(vbox2);


  // Menu
  wxMenu *menuFile = new wxMenu;

  menuFile->Append( ID_About, _T("&About...") );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, _T("E&xit") );

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, _T("&File") );

  SetMenuBar( menuBar );

  CreateStatusBar();
  SetStatusText( _T("iShare, SAGE desktop sharing for Mac") );

  wxMessageBox(_T("iShare desktop sharing for Mac v1.0"),
	       _T("iShare"), wxOK | wxICON_INFORMATION, this);

  /////////////////////////
  // Get the OpenGL context
  CGLPixelFormatObj pixelFormatObj ;
  GLint numPixelFormats ;
  CGDirectDisplayID displayId = CGMainDisplayID();
  CGRect dRect = CGDisplayBounds( displayId );
  WW = dRect.size.width;
  HH = dRect.size.height;
  CGOpenGLDisplayMask displayMask = CGDisplayIDToOpenGLDisplayMask(displayId);

  CGLPixelFormatAttribute attribs[] =
    {
      (CGLPixelFormatAttribute)kCGLPFAFullScreen,
      (CGLPixelFormatAttribute)kCGLPFADisplayMask,
      (CGLPixelFormatAttribute)displayMask,
      (CGLPixelFormatAttribute)0
    };
  CGLChoosePixelFormat( attribs, &pixelFormatObj, &numPixelFormats );
  CGLCreateContext( pixelFormatObj, NULL, &glContextObj ) ;
  CGLDestroyPixelFormat( pixelFormatObj ) ;
  CGLSetCurrentContext( glContextObj ) ;
  glReadBuffer(GL_FRONT);
  CGLSetFullScreen( glContextObj ) ;///UUUUUUUUUUnbelievable
  CGLSetCurrentContext( NULL );
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  CGLClearDrawable( glContextObj );
  CGLDestroyContext( glContextObj );

  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_T("iShare desktop sharing for Mac v1.0"),
	       _T("iShare"), wxOK | wxICON_INFORMATION, this);
}


