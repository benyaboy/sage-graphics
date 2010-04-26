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

#include "Fl_Lambda.h"

/*--------------------------------------------------------------------------*/

Fl_Lambda::Fl_Lambda(int x, int y, int w, int h, const char *l)
  : Fl_Double_Window(x, y, w, h, l) {

  // Resize to the correct size
  size(255, 115);

  // Set modal
  //set_modal();

  // Create port input field
  _portInput = new Fl_Input(45, 10, 200, 25, "Port:");
  _portInput -> value("7000");

  // Create connect button
  _listenButton = new Fl_Return_Button(45, 80, 95, 25, "Listen");

  // Create exit button
  _cancelButton = new Fl_Button(150, 80, 95, 25, "Cancel");

}

/*--------------------------------------------------------------------------*/

Fl_Lambda::~Fl_Lambda() {
}

/*--------------------------------------------------------------------------*/

int Fl_Lambda::GetPort() {

  // Return port
  return atoi(_portInput -> value());

}

/*--------------------------------------------------------------------------*/

void Fl_Lambda::SetCancelButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _cancelButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/

void Fl_Lambda::SetListenButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _listenButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/
