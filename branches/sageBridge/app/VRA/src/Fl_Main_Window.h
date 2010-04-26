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

#ifndef FL_MENU_H
#define FL_MENU_H

/*--------------------------------------------------------------------------*/

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Return_Button.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/

class Fl_Main_Window : public Fl_Double_Window {

public:

  // Constructor
  Fl_Main_Window(int x, int y, int w, int h, const char* l = 0);

  // Destructor
  ~Fl_Main_Window();
  
  // Activate connect
  void ActivateConnect();

  // Activate disconnect
  void ActivateDisconnect();

  // Activate shutdown
  void ActivateShutdown();

  // Deactivate connect
  void DeactivateConnect();

  // Deactivate disconnect
  void DeactivateDisconnect();

  // Deactivate shutdown
  void DeactivateShutdown();

  // Set menu items
  void SetMenuItems(Fl_Menu_Item* items);

private:

  // Menu bar
  Fl_Menu_Bar* _menuBar;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
