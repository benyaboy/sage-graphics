/*--------------------------------------------------------------------------*/
/* Volume Rendering Application                                             */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                 */
/*                                                                          */
/* This software is free software; you can redistribute it and/or modify it */
/* under the terms of the GNU Lesser General Public License as published by */
/* the Free Software Foundation; either Version 2.1 of the License, or      */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This software is distributed in the hope that it will be useful, but     */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser  */
/* General Public License for more details.                                 */
/*                                                                          */
/* You should have received a copy of the GNU Lesser Public License along   */
/* with this library; if not, write to the Free Software Foundation, Inc.,  */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                    */
/*--------------------------------------------------------------------------*/

#ifndef FL_CONNECT_H
#define FL_CONNECT_H

/*--------------------------------------------------------------------------*/

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/

class Fl_Connect : public Fl_Double_Window {

public:

  // Constructor
  Fl_Connect(int x, int y, int w, int h, const char* l = 0);

  // Destructor
  ~Fl_Connect();

  // Get host
  const char* GetHost();

  // Get port
  int GetPort();

  // Set connect button callback
  void SetConnectButtonCallback(Fl_Callback* cb, void* v);

  // Set exit button callback
  void SetExitButtonCallback(Fl_Callback* cb, void* v);

private:

  // Connect button
  Fl_Return_Button* _connectButton;

  // Exit button
  Fl_Button* _exitButton;

  // Host input field
  Fl_Input* _hostInput;

  // Port input field
  Fl_Input* _portInput;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
