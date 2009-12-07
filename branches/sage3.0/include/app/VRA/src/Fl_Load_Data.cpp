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

#include "Fl_Load_Data.h"

/*--------------------------------------------------------------------------*/

Fl_Load_Data::Fl_Load_Data(int x, int y, int w, int h, const char *l)
  : Fl_Double_Window(x, y, w, h, l) {

  // Resize to the correct size
  size(220, 305);

  // Set modal
  //set_modal();

  // Create data browser
  _dataBrowser = new Fl_Hold_Browser(10, 10, 200, 250);

  // Create ok button
  _okButton = new Fl_Return_Button(10, 270, 95, 25, "OK");

  // Create cancel button
  _cancelButton = new Fl_Button(115, 270, 95, 25, "Cancel");

}

/*--------------------------------------------------------------------------*/

Fl_Load_Data::~Fl_Load_Data() {
}

/*--------------------------------------------------------------------------*/

void Fl_Load_Data::Add(const char* entry) {

  // Add entry to browser
  _dataBrowser -> add(entry);

}

/*--------------------------------------------------------------------------*/

void Fl_Load_Data::Clear() {

  // Clear all entries in browser
  _dataBrowser -> clear();

}

/*--------------------------------------------------------------------------*/

int Fl_Load_Data::GetSelected() {

  // Return selected line
  return _dataBrowser -> value();

}

/*--------------------------------------------------------------------------*/

void Fl_Load_Data::SetOKButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _okButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/

void Fl_Load_Data::SetCancelButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _cancelButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/
