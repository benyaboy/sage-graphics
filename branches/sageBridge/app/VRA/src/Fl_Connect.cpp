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

#include "Fl_Connect.h"

/*--------------------------------------------------------------------------*/

Fl_Connect::Fl_Connect(int x, int y, int w, int h, const char *l)
  : Fl_Double_Window(x, y, w, h, l) {

  // Resize to the correct size
  size(255, 115);

  // Set modal
  //set_modal();

  // Create host input field
  _hostInput = new Fl_Input(45, 10, 200, 25, "Host:");
  _hostInput -> value("localhost");

  // Create port input field
  _portInput = new Fl_Input(45, 45, 200, 25, "Port:");
  _portInput -> value("10001");

  // Create connect button
  _connectButton = new Fl_Return_Button(45, 80, 95, 25, "Connect");

  // Create exit button
  _exitButton = new Fl_Button(150, 80, 95, 25, "Exit");

}

/*--------------------------------------------------------------------------*/

Fl_Connect::~Fl_Connect() {
}

/*--------------------------------------------------------------------------*/

const char* Fl_Connect::GetHost() {

  // Return host
  return _hostInput -> value();

}

/*--------------------------------------------------------------------------*/

int Fl_Connect::GetPort() {

  // Return port
  return atoi(_portInput -> value());

}

/*--------------------------------------------------------------------------*/

void Fl_Connect::SetConnectButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _connectButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/

void Fl_Connect::SetExitButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _exitButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/
