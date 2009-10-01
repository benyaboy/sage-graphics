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

#include "Fl_Main_Window.h"

/*--------------------------------------------------------------------------*/

Fl_Main_Window::Fl_Main_Window(int x, int y, int w, int h, const char *l)
  : Fl_Double_Window(x, y, w, h, l) {

  // Create menu bar
  _menuBar = new Fl_Menu_Bar(0, 0, 640, 30);

  // Create menu items
  Fl_Menu_Item menuitems[] = {
    {"&File",          0, 0, 0, FL_SUBMENU},
    {"&Connect to Server...",      FL_CTRL + 'c', NULL},
    {"&Disconnect from Server",    FL_CTRL + 'd', NULL, 0, FL_MENU_DIVIDER},
    {"&Shutdown Server",           FL_CTRL + 's', NULL, 0, FL_MENU_DIVIDER},
    {"&Load Data...",              FL_CTRL + 'l', NULL, 0, FL_MENU_DIVIDER},
    {"L&oad Script...",            FL_CTRL + 'o', NULL, 0, FL_MENU_DIVIDER},
    {"E&xit",                      FL_CTRL + 'q', NULL, 0},
    {0},
    {0}
  };

  // Add menu items to menu bar
  _menuBar -> copy(menuitems);

}

/*--------------------------------------------------------------------------*/

Fl_Main_Window::~Fl_Main_Window() {
}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::ActivateConnect() {

  // Deactivate
  _menuBar -> mode(1, 0);

}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::ActivateDisconnect() {

  // Deactivate
  _menuBar -> mode(2, 0);

}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::ActivateShutdown() {

  // Deactivate
  _menuBar -> mode(3, 0|0x80);

}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::DeactivateConnect() {

  // Deactivate
  _menuBar -> mode(1, 1);

}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::DeactivateDisconnect() {

  // Deactivate
  _menuBar -> mode(2, 1);

}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::DeactivateShutdown() {

  // Deactivate
  _menuBar -> mode(3, 1|0x80);

}

/*--------------------------------------------------------------------------*/

void Fl_Main_Window::SetMenuItems(Fl_Menu_Item* items) {

  // Copy menu items
  _menuBar -> copy(items);

}

/*--------------------------------------------------------------------------*/
