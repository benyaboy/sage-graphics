/******************************************************************************
 * Program: GlobalCoordinatesToFileMapper
 * Module:  FileInfo.cpp
 * Authors: Nicholas Schwarz, schwarz@evl.uic.edu
 * Date:    21 September 2004
 *
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
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
 * Direct questions, comments etc to schwarz@evl.uic.edu or
 * http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/


// Include header file for this class
#include "FileInfo.h"


//-----------------------------------------------------------------------------
FileInfo::FileInfo()
{
  this -> Dimensions[0] = 0;
  this -> Dimensions[1] = 0;
  this -> FileName = NULL;
  this -> Extent[0] = 0;
  this -> Extent[1] = 0;
  this -> Extent[2] = 0;
  this -> Extent[3] = 0;
  this -> LocalPosition[0] = 0;
  this -> LocalPosition[1] = 0;
}


//-----------------------------------------------------------------------------
void FileInfo::GetDimensions(long* w, long* h)
{
  *w = this -> Dimensions[0];
  *h = this -> Dimensions[1];

  return;
}


//-----------------------------------------------------------------------------
void FileInfo::GetExtent(long* x, long* y,
			 long* w, long* h)
{
  *x = this -> Extent[0];
  *y = this -> Extent[1];
  *w = this -> Extent[2];
  *h = this -> Extent[3];

  return;
}


//-----------------------------------------------------------------------------
const char* FileInfo::GetFileName()
{
  return this -> FileName;
}


//-----------------------------------------------------------------------------
void FileInfo::GetLocalPosition(long* col, long* row)
{
  *col = this -> LocalPosition[0];
  *row = this -> LocalPosition[1];

  return;
}


//-----------------------------------------------------------------------------
void FileInfo::PrintSelf(ostream& os)
{

  os << "File Info Start --------------------------------------------\n";

  os << "Dimensions: "
     << this -> Dimensions[0] << " "
     << this -> Dimensions[1] << "\n";

  if (this -> FileName != NULL)
    os << "File Name: " << this -> FileName << "\n";
  else
    os << "File Name: null\n";

  os << "Extent: "
     << this -> Extent[0] << " "
     << this -> Extent[1] << " "
     << this -> Extent[2] << " "
     << this -> Extent[3] << "\n";

  os << "Local Position: "
     << this -> LocalPosition[0] << " "
     << this -> LocalPosition[1] << "\n";

  os << "File Info End ----------------------------------------------" << endl;

  return;
}


//-----------------------------------------------------------------------------
void FileInfo::SetDimensions(long w, long h)
{
  this -> Dimensions[0] = w;
  this -> Dimensions[1] = h;

  return;
}


//-----------------------------------------------------------------------------
void  FileInfo::SetExtent(long x, long y,
			  long w, long h)
{
  this -> Extent[0] = x;
  this -> Extent[1] = y;
  this -> Extent[2] = w;
  this -> Extent[3] = h;

  return;
}


//-----------------------------------------------------------------------------
void FileInfo::SetExtentX(long x)
{
  this -> Extent[0] = x;
}


//-----------------------------------------------------------------------------
void FileInfo::SetExtentY(long y)
{
  this -> Extent[1] = y;
}


//-----------------------------------------------------------------------------
void FileInfo::SetExtentW(long w)
{
  this -> Extent[2] = w;
}


//-----------------------------------------------------------------------------
void FileInfo::SetExtentH(long h)
{
  this -> Extent[3] = h;
}


//-----------------------------------------------------------------------------
void FileInfo::SetFileName(const char* name)
{
  if (this -> FileName != NULL)
    {
      delete [] this -> FileName;
    }

  if (name != NULL)
    {
      this -> FileName = new char[strlen(name) + 1];
      strcpy(this -> FileName, name);
    }
  else {
    this -> FileName = NULL;
  }

  return;
}


//-----------------------------------------------------------------------------
void FileInfo::SetLocalPosition(long col, long row)
{
  this -> LocalPosition[0] = col;
  this -> LocalPosition[1] = row;

  return;
}


//-----------------------------------------------------------------------------
FileInfo::~FileInfo()
{
  if (this -> FileName != NULL)
    {
      delete [] this -> FileName;
    }
}
