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

#include "Fl_Dialog.h"

/*--------------------------------------------------------------------------*/

Fl_Dialog::Fl_Dialog(int x, int y, int w, int h, const char* l)
  : Fl_Double_Window(x, y, w, h, l) {

  // Resize to correct size
  size(195, 75);

  // Set modal
  //set_modal();
  
  // Create box
  _box = new Fl_Box(10, 10, 175, 20);
  
  // Create ok button
  _okButton = new Fl_Return_Button(90, 40, 95, 25, "OK");

}

/*--------------------------------------------------------------------------*/

Fl_Dialog::~Fl_Dialog() {
}

/*--------------------------------------------------------------------------*/

void Fl_Dialog::SetMessage(char* message) {

  // Set message
  _box -> copy_label(message);

}

/*--------------------------------------------------------------------------*/

void Fl_Dialog::SetOKButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _okButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/
