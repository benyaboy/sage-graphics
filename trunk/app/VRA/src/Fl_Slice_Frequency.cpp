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

#include "Fl_Slice_Frequency.h"

/*--------------------------------------------------------------------------*/

Fl_Slice_Frequency::Fl_Slice_Frequency(int x, int y, int w, int h, 
                                       const char *l)
  : Fl_Double_Window(x, y, w, h, l) {

  // Resize to the correct size
  size(225, 80);

  // Set modal
  //set_modal();

  // Create slice frequency counter
  _counter = new Fl_Counter(10, 10, 200, 25, "");
  _counter -> type(FL_SIMPLE_COUNTER);
  _counter -> minimum(0.0);
  _counter -> maximum(100.0);

  // Create connect button
  _okButton = new Fl_Return_Button(10, 45, 95, 25, "OK");

  // Create exit button
  _cancelButton = new Fl_Button(115, 45, 95, 25, "Cancel");

}

/*--------------------------------------------------------------------------*/

Fl_Slice_Frequency::~Fl_Slice_Frequency() {
}

/*--------------------------------------------------------------------------*/

double Fl_Slice_Frequency::GetSliceFrequency() {

  // Return host
  return _counter -> value();

}

/*--------------------------------------------------------------------------*/

void Fl_Slice_Frequency::SetOKButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _okButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/

void Fl_Slice_Frequency::SetCancelButtonCallback(Fl_Callback* cb, void* v) {

  // Set callback
  _cancelButton -> callback(cb, v);

}

/*--------------------------------------------------------------------------*/

void Fl_Slice_Frequency::SetSliceFrequency(double frequency) {

  // Set counter value
  _counter -> value(frequency);

}

/*--------------------------------------------------------------------------*/
