/*=============================================================================

  Program: JuxtaView for SAGE
  Module:  JuxtaSCUI.cpp - Part of JuxtaView's UI
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
 *
 * Direct questions, comments etc about SAGE to http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/


#include "JuxtaSCUI.h"

void SkipComments(FILE* fin,char commentFlag);
#define COMMENT_FLAG '#'

JuxtaSCUIFrame::JuxtaSCUIFrame(bool ov, char* skinfilename,wxWindow* parent, int id, const wxString& title,
			   const wxPoint& pos, const wxSize& size, long style)
			   :wxFrame(parent,id,title,pos,size,style)
{
	QUANTAinit();
	client = NULL;
	num_clients = 0;


	//---------setup the ui
	wxImage* img;
	wxBitmapButton* bmpbtn;
	wxBitmap* bmp;
	wxPanel* panel;
	char buffer[100];
	char background[100];
	char upfile[100];
	char downfile[100];
	int xpos,ypos;

	gApp->SetTopWindow(this);

	panel = new wxPanel(this,-1);

	//background
	wxInitAllImageHandlers();
	FILE* skinfile = NULL;

	if(skinfilename == NULL)
		skinfile = fopen("Skins/default.jsk","r");
	else
		skinfile = fopen(skinfilename,"r");

	if(skinfile == NULL)
		return;

	SkipComments(skinfile,COMMENT_FLAG);

	//get the background filename
	fscanf(skinfile,"%s\n",background);
	img = new wxImage(background);
	bmp = new wxBitmap((*img));
	statbmp = new wxStaticBitmap(panel,20,(*bmp),wxPoint(0,0));
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS) || defined(_WINDOWS_)
	statbmp->Disable();	//to make sure the button pushes go to the buttons, not the background image
#endif

	//get the pan up info
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_PanUp,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//get the pan down info
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_PanDown,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//get the pan left info
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_PanLeft,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//get the pan right info
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_PanRight,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//get the fast pan up info
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_FastPanUp,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//load up the fast pan down button
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_FastPanDown,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//load up the fast pan left button
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_FastPanLeft,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//load up the fast pan right button
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_FastPanRight,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//load up the zoom in button
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_ZoomIn,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//load up the zoom out button
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_ZoomOut,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	//load up the Quit button
	SkipComments(skinfile,COMMENT_FLAG);
	fscanf(skinfile,"%s %s %s (%d,%d)\n",buffer,upfile,downfile,&xpos,&ypos);
	img = new wxImage(upfile);
	bmpbtn = new wxBitmapButton(panel,ID_Quit,(*img),wxPoint(xpos,ypos),
					wxSize(img->GetWidth(),img->GetHeight()),wxBORDER_NONE);
	img = new wxImage(downfile);
	bmpbtn->SetBitmapSelected((*img));
	bmpbtn->SetToolTip(buffer);

	fclose(skinfile);

	overviewframe = NULL;
	showoverview = ov;

}


void JuxtaSCUIFrame::Show()
{
    for (int i = 0 ; i < num_clients ; i++)
	if(overviewframe[i] && showoverview)
		overviewframe[i]->Show();

	wxFrame::Show();
}


BEGIN_EVENT_TABLE( JuxtaSCUIFrame, wxFrame)
	EVT_BUTTON( ID_PanUp, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_PanDown, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_PanLeft, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_PanRight, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_FastPanUp, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_FastPanDown, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_FastPanLeft, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_FastPanRight, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_ZoomIn, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_ZoomOut, JuxtaSCUIFrame::OnEvent)
	EVT_BUTTON( ID_Quit, JuxtaSCUIFrame::OnEvent)
END_EVENT_TABLE()

//------------------------------------------
JuxtaSCUIFrame::~JuxtaSCUIFrame()
{

	if(client)
	{
		PassEvent(JVQUIT);
		for (int i = 0 ; i < num_clients ; i++) {
		    client[i] -> close();
		    delete client[i];
		    client[i] = NULL;
		}
		delete client;
	}

}


//------------------------------------------
void JuxtaSCUIFrame::Init(char* configfilename)
{
	FILE* fin = fopen(configfilename,"r");
	if(!fin)
		return;

	char ip[32];
	int port;
	memset(ip,0,32);

	// get number of clients to connect to
	fscanf(fin,"%d", &num_clients);

	// allocate array of pointers to clients
	client = new QUANTAnet_tcpClient_c *[num_clients];

	// allocate array of pointers to overview frames
	overviewframe = new JuxtaOverviewFrame *[num_clients];

	// allocate QUANTA objects for each client
	for (int i = 0 ; i < num_clients ; i++) {
	    client[i] = new QUANTAnet_tcpClient_c();
	}

	cout << "*****Number of clients: " << num_clients << endl;

	for (int i = 0 ; i < num_clients ; i++) {

	    // read client info and connect
	    fscanf(fin,"%s : %d\n",ip,&port);

	    //client[i] = new QUANTAnet_tcpClient_c();
	    if(client[i] -> connectToServer(ip, port) < 0)
	    {
		client[i] -> close();
		delete client[i];
		client[i] = NULL;

#if DEBUG
		wxMessageBox("Failed To Connect To Server.\nMake sure juxtauiclient.conf file has correct server ip and port number.\nCorrect format is \"server ip\" : \"port number\"");
#endif
	    }

	    else
	    {
		//make sure the user didn't disable the overview
		if(showoverview)
		{
		    fprintf(stderr,"Attempting to load overview\n");
		    //ask for the overview!!!!
		    overviewframe[i] = new JuxtaOverviewFrame(gApp->GetTopWindow(),
							      -1,"Overview",
							      wxPoint(wxWindow::GetSize().GetWidth(),0),
							      wxSize(100,200),
							      wxMINIMIZE_BOX | wxFRAME_NO_TASKBAR | wxRESIZE_BORDER);
		    overviewframe[i]->Layout();
		    overviewframe[i]->Show();

		    GetOverview(i);
		    GetExtents(i);
		}
		else
		{
		    fprintf(stderr,"Not attempting to make overview\n");
		    overviewframe[i] = new JuxtaOverviewFrame(0,-1,"",wxPoint(0,0),wxSize(0,0),wxSIMPLE_BORDER);
		    overviewframe[i]->Layout();
		    overviewframe[i]->Show();
		}

		fflush(stderr);

	    }

	}

/*

	if(client[0] == NULL)
	{
            client[0] -> close();
	    delete client[i];
	    client[0] = NULL;

#if DEBUG
	    wxMessageBox("Failed To Connect To Server.\nMake sure juxtauicl\
ient.conf file has correct server ip and port number.\nCorrect format is \"serv\
er ip\" : \"port number\"");
#endif
	}
	else
	{
	    //make sure the user didn't disable the overview
	    if(showoverview)
	    {
		fprintf(stderr,"Attempting to load overview\n");
		//ask for the overview!!!!
		overviewframe = new JuxtaOverviewFrame(gApp->GetTopWindow()\
						       ,
						       -1,"Overview",
						       wxPoint(wxWindow::GetSize().GetWidth(),0),
						       wxSize(100,200),
                                                           wxMINIMIZE_BOX | wxFRAME_NO_TASKBAR | wxRESIZE_BORDER);
		overviewframe->Layout();
		overviewframe->Show();

		GetOverview(i);
		GetExtents(i);
	    }
	    else
	    {
		fprintf(stderr,"Not attempting to make overview\n");
		overviewframe = new JuxtaOverviewFrame(0,-1,"",wxPoint(0,0)\
						       ,wxSize(0,0),wxSIMPLE_BORDER);
		overviewframe[i]->Layout();
		overviewframe[i]->Show();
	    }

	    fflush(stderr);

	}
*/
	fclose(fin);
}

//------------------------------------------
void JuxtaSCUIFrame::OnEvent(wxCommandEvent& event)
{

	//disable all buttons but quit
	//here's the tree
	//	this
	//	|_______panel
	//		|________All the buttons!!!!!!

//	this->GetChildren().GetFirst()->GetData()->Disable();

	switch(event.GetId())
	{
	case ID_FastPanLeft:
		PassEvent(JVFAST_PAN_LEFT);
		break;
	case ID_FastPanRight:
	    	PassEvent(JVFAST_PAN_RIGHT);
		break;
	case ID_FastPanUp:
		PassEvent(JVFAST_PAN_UP);
		break;
	case ID_FastPanDown:
		PassEvent(JVFAST_PAN_DOWN);
		break;
	case ID_PanLeft:
		PassEvent(JVPAN_LEFT);
		break;
	case ID_PanRight:
		PassEvent(JVPAN_RIGHT);
		break;
	case ID_PanUp:
		PassEvent(JVPAN_UP);
		break;
	case ID_PanDown:
		PassEvent(JVPAN_DOWN);
		break;
	case ID_ZoomIn:
		PassEvent(JVZOOM_IN);
		break;
	case ID_ZoomOut:
		PassEvent(JVZOOM_OUT);
		break;
	case ID_Quit:
		PassEvent(JVQUIT);
		break;
	default:
		break;
	}	//end switch

	//ask for the extent if it's not quit
	if( event.GetId() != ID_Quit)

	    for (int i = 0 ; i < num_clients ; i++) {
		if (overviewframe[i] && showoverview )
		{
		    GetExtents(i);
		    if(overviewframe[i])
			overviewframe[i]->Refresh();
		}
	    }

	Refresh();

//	this->GetChildren().GetFirst()->GetData()->Enable();

}

//------------------------------------------
void JuxtaSCUIFrame::BigRefresh()
{
    for (int i = 0 ; i < num_clients ; i++) {
	if (overviewframe[i] && showoverview )
	{
	    GetExtents(i);
	    if(overviewframe[i])
		overviewframe[i]->Refresh();
	}
    }

    Refresh();

}

//------------------------------------------
void JuxtaSCUIFrame::PassEvent(char* strEvent)
{
#if DEBUG
	fprintf(stderr,"passing event %s\n",strEvent);
#endif
	//pass event to server
	static char sendBuffer[TRANSMISSION_SIZE];
	static int size = TRANSMISSION_SIZE;

	clientMutex.Lock();
		memset(sendBuffer,0,TRANSMISSION_SIZE);
		strcpy(sendBuffer,strEvent);
		for (int i = 0 ; i < num_clients ; i++) {
		    if(client[i])
		    {
			client[i]->write(sendBuffer,&size, QUANTAnet_tcpClient_c::BLOCKING);
		    }
		}
	clientMutex.Unlock();

}

//------------------------------------------
void JuxtaSCUIFrame::PassBuffer(void* buffer,int size)
{
	clientMutex.Lock();
	for (int i = 0 ; i < num_clients ; i++) {
	    if(client[i])
	    {
		client[i]->write((const char*)buffer,&size,QUANTAnet_tcpClient_c::BLOCKING);
	    }
	}
	clientMutex.Unlock();

}

//------------------------------------------
void JuxtaSCUIFrame::GetExtents(int cn)
{
#if DEBUG
	fprintf(stderr,"Getting extents for overview\n");
#endif
	//pass event to server
	static char sendBuffer[TRANSMISSION_SIZE];
	static int sendsize = TRANSMISSION_SIZE;
	static int readsize = sizeof(float) * 4;

	memset(sendBuffer,0,TRANSMISSION_SIZE);
	clientMutex.Lock();
		strcpy(sendBuffer,JVEXTENTS);

		float readarray[4];
		if( client[cn] )
		{
			client[cn]->write(sendBuffer,&sendsize,QUANTAnet_tcpClient_c::BLOCKING);
			//get four floats
			client[cn]->read((char*)readarray,&readsize,QUANTAnet_tcpClient_c::BLOCKING);
			if(overviewframe[cn])
				overviewframe[cn]->SetExtentRectangle(readarray[0],readarray[1],readarray[2],readarray[3]);

#if DEBUG
		fprintf(stderr,"floats recieved: %f, %f, %f, %f\n",readarray[0],readarray[1],readarray[2],readarray[3]);
#endif
		}
	clientMutex.Unlock();

}

void JuxtaSCUIFrame::GetOverview(int cn)
{
#if DEBUG
	fprintf(stderr,"Getting overview image\n");
#endif
	//pass event to server
	static char sendBuffer[TRANSMISSION_SIZE];
	static int sendsize = TRANSMISSION_SIZE;
	static int readsize;

	memset(sendBuffer,0,TRANSMISSION_SIZE);

	clientMutex.Lock();
		strcpy(sendBuffer,JVOVERVIEW);

		if(client[cn])
		{
			//ask for the overview
			client[cn]->write(sendBuffer,&sendsize,QUANTAnet_tcpClient_c::BLOCKING);

			//get two ints
			int dimarray[2];
			readsize = sizeof(int) * 2;
			client[cn]->read((char*)dimarray,&readsize,QUANTAnet_tcpClient_c::BLOCKING);

			//get pixels
			readsize = dimarray[0] * dimarray[1] * 3;
			unsigned char* img = new unsigned char[readsize];
			client[cn]->read((char*)img,&readsize,QUANTAnet_tcpClient_c::BLOCKING);

			if(overviewframe[cn])
				overviewframe[cn]->SetImage(dimarray[0],dimarray[1],img);

#if DEBUG
		fprintf(stderr,"Got overview of dims %d x %d\n",dimarray[0],dimarray[1]);
#endif

		}

	clientMutex.Unlock();

}

//------------------------------------------

void SkipComments(FILE* fin,char commentFlag)
{
	char c;
	c = fgetc(fin);
	bool skipline;
	if( c == commentFlag)
	{
		skipline = true;
		while(skipline)
		{
			while( c != '\n' && !feof(fin))
				c = fgetc(fin);

			if(feof(fin))
				return;

			long position = ftell(fin);
			c = fgetc(fin);
			if(c != commentFlag)
			{
				skipline = false;
				fseek(fin,position,0);
			}
		}
	}
	else
	{
		fseek(fin, ftell(fin) - 1, 0);
	}
}
