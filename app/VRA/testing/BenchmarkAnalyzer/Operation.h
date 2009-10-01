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

#ifndef OPERATION_H
#define OPERATION_H

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------*/

class Operation {

public:

  // Default constructor
  Operation();

  // Constructor that parses a message string
  Operation(char* string);

  // Default destructor
  ~Operation();

  // Get class name
  char* GetClassName();

  // Get operation information
  char* GetInfo();

  // Get log number
  int GetLogNumber();

  // Get process number
  int GetProcessNumber();

  // Get operation time
  long long GetTime();

private:

  // Class name
  char _className[1024];

  // Operation information
  char _info[4096];

  // Log number
  int _logNumber;

  // Process number
  int _processNumber;

  // Operation time
  long long _time;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
