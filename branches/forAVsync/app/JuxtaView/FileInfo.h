/******************************************************************************
 * Program: GlobalCoordinatesToFileMapper
 * Module:  FileInfo.h
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

// .NAME FileInfo
// .SECTION Description
// FileInfo is a class used to pass data concerning a file's usage. The extent
// does not refer to the extent of the file in relation to the global space
// (although it could (it could refer to any extent depending on the context))
// but to the extent of the file that is of interest.


#ifndef FILE_INFO_H
#define FILE_INFO_H


#include <ostream.h>
#include <stddef.h>
#include <string.h>


class FileInfo {

 public:

  // Description:
  // Constructor
  FileInfo();

  // Description:
  // Get the width and height of the file in its coordinates, usually pixels.
  void GetDimensions(long* w, long* h);

  // Description:
  // Get the extent of the file in its coordinates, usually pixels.
  void GetExtent(long* x, long* y,
		 long* w, long* h);

  // Description:
  // Get the file name.
  const char* GetFileName();

  // Description:
  // Get the local column and row that the file resides in.
  void GetLocalPosition(long* col, long* row);

  // Description:
  // Print itself
  void PrintSelf(ostream& os);

  // Set the width and height of the file in its coordinates, usually pixels.
  void SetDimensions(long w, long h);

  // Set the extent of the file in its coordinates, usually pixels.
  void SetExtent(long x, long y,
		 long w, long h);

  // Set the height of the extent in the file's coordinates, usually pixels.
  void SetExtentH(long h);

  // Set the width of the extent in the file's coordinates, usually pixels.
  void SetExtentW(long w);

  // Set the starting x position of the extent in the file's coordinates,
  // usually pixels. The upper left corner is (0, 0).
  void SetExtentX(long x);

  // Set the starting y position of the extent in the file's coordinates,
  // usually pixels. The upper left corner is (0, 0).
  void SetExtentY(long y);

  // Set the file name.
  void SetFileName(const char*);

  // Set the column and row of the file's local position.
  void SetLocalPosition(long col, long row);

  // Description:
  // Destructor
  ~FileInfo();


 private:

  // File's dimensions in its coordinate system, usually in pixels.
  long Dimensions[2];

  // The extent of the file that should be used, given as starting x, y, and
  // width and height of the extent in the file's coordinate system, usually
  // in pixels.
  long Extent[4];

  // File name.
  char* FileName;

  // The local column and row that the file resides in.
  long LocalPosition[2];

};


#endif
