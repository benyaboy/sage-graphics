/******************************************************************************
 * Program: GlobalCoordinatesToFileMapper
 * Module:  GlobalCoordinatesToFileMapper.cpp
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
#include "GlobalCoordinatesToFileMapper.h"


//-----------------------------------------------------------------------------
GlobalCoordinatesToFileMapper::GlobalCoordinatesToFileMapper()
{

  // Initialize global image description to NULL.
  this -> Description = NULL;

  // Initialize file coordinate dimensions to 0 by 0.
  this -> FileCoordinateDimensions[0] = 0;
  this -> FileCoordinateDimensions[1] = 0;

  // Initialize global coordinate dimensions to 0 by 0.
  this -> GlobalCoordinateDimensions[0] = 0;
  this -> GlobalCoordinateDimensions[1] = 0;

  // Initalize global file layour array to NULL.
  this -> GlobalFileLayoutArray = NULL;

  // Initialize global file layout dimensions to 0.
  this -> GlobalFileLayoutDimensions[0] = 0;
  this -> GlobalFileLayoutDimensions[1] = 0;

  // Initialize the initialized flag to false.
  this -> Initialized = false;

  // Initialize the query extent.
  this -> QueryExtent[0] = 0;
  this -> QueryExtent[1] = 0;
  this -> QueryExtent[2] = 0;
  this -> QueryExtent[3] = 0;

  // Initialize the query file info array to NULL.
  this -> QueryFileInfoArray = NULL;

  // Initialize the query file info array dimensions to 0 by 0.
  this -> QueryFileInfoDimensions[0] = 0;
  this -> QueryFileInfoDimensions[1] = 0;

}


//-----------------------------------------------------------------------------
const char* GlobalCoordinatesToFileMapper::GetDescription()
{

  // Return pointer to global image description.
  return this -> Description;

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetFileCoordinateDimensionH()
{

  // Return FileCoordinateDimensionH
  return this -> FileCoordinateDimensions[1];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetFileCoordinateDimensionW()
{

  // Return FileCoordinateDimensionW
  return this -> FileCoordinateDimensions[0];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetGlobalCoordinateDimensionH()
{

  // Return GlobalCoordinateDimensionH
  return this -> GlobalCoordinateDimensions[1];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetGlobalCoordinateDimensionW()
{

  // Return GlobalCoordinateDimensionW
  return this -> GlobalCoordinateDimensions[0];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetGlobalCoordinates(const char* name,
							 long* x,
							 long* y,
							 long* w,
							 long* h)
{

  // Check if instance is initialized
  if (this -> Initialized == false)
    {
      return GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_NOT_INITIALIZED;
    }

  // Loop through the global file layout array.
  for (long c = 0, p = 0 ; c < GlobalFileLayoutDimensions[0] ; c++)
    {
      for (long r = 0 ; r < GlobalFileLayoutDimensions[1] ; r++, p++)
	{
	  // Check for given file.
	  if (strcmp(this -> GlobalFileLayoutArray[p], name) == 0)
	    {
	      *x = this -> FileCoordinateDimensions[0] * c;
	      *y = this -> FileCoordinateDimensions[1] * r;
	      *w = this -> FileCoordinateDimensions[0];
	      *h = this -> FileCoordinateDimensions[1];
	      return GLOBAL_COORDINATES_TO_FILE_MAPPER_SUCCESS;
	    }
	}
    }

  // Return error is the file does not exist in the mapper.
  return GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_INVALID_FILE;

}


//-----------------------------------------------------------------------------
char** GlobalCoordinatesToFileMapper::GetGlobalFileLayoutArray(long* cols,
							       long* rows)
{

  // GlobalFileLayoutDimensions
  *cols = this -> GlobalFileLayoutDimensions[0];
  *rows = this -> GlobalFileLayoutDimensions[1];

  // Return pointer to the global file layout array
  return this -> GlobalFileLayoutArray;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::GetGlobalFileLayoutDimensions(long* cols,
								  long* rows)
{

  // GlobalFileLayoutDimensions
  *cols = this -> GlobalFileLayoutDimensions[0];
  *rows = this -> GlobalFileLayoutDimensions[1];

  return;

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetQueryExtentH()
{

  // Return QueryExtentH
  return this -> QueryExtent[3];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetQueryExtentW()
{

  // Return QueryExtentW
  return this -> QueryExtent[2];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetQueryExtentX()
{

  // Return QueryExtentH
  return this -> QueryExtent[0];

}


//-----------------------------------------------------------------------------
long GlobalCoordinatesToFileMapper::GetQueryExtentY()
{

  // Return QueryExtentH
  return this -> QueryExtent[1];

}


//-----------------------------------------------------------------------------
FileInfo* GlobalCoordinatesToFileMapper::GetQueryFileInfoArray(long* cols,
							       long* rows)
{

  // QueryFileInfoArray dimensions
  *cols = this -> QueryFileInfoDimensions[0];
  *rows = this -> QueryFileInfoDimensions[1];

  // Return QueryFileInfoArray
  return this -> QueryFileInfoArray;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::GetQueryFileInfoDimensions(long* cols,
							       long* rows)
{

  // QueryFileInfoArray dimensions
  *cols = this -> QueryFileInfoDimensions[0];
  *rows = this -> QueryFileInfoDimensions[1];

  return;

}


//-----------------------------------------------------------------------------
int GlobalCoordinatesToFileMapper::Initialize(const char* name)
{

  // Try to open file for reading.
  ifstream file(name);

  // Check if file is open.
  if (!file.is_open())
    {
      return GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_INVALID_FILE;
    }

  // Read GlobalCoordinateDimensions, GlobalFileLayoutDimensions, and
  // FileCoordinateDimensions, respectively.
  file >> this -> GlobalCoordinateDimensions[0]
       >> this -> GlobalCoordinateDimensions[1]
       >> this -> GlobalFileLayoutDimensions[0]
       >> this -> GlobalFileLayoutDimensions[1]
       >> this -> FileCoordinateDimensions[0]
       >> this -> FileCoordinateDimensions[1];

  // Allocate storage for the GlobalFileLayoutArray
  this -> GlobalFileLayoutArray =
    new char*[GlobalFileLayoutDimensions[0] * GlobalFileLayoutDimensions[1]];

  // Read each file in the global image in order. Entries in the file are
  // column by column.
  for (long i = 0 ;
       i < GlobalFileLayoutDimensions[0] * GlobalFileLayoutDimensions[1] ;
       i++)
    {
      this -> GlobalFileLayoutArray[i] = new char[100];
      file >> this -> GlobalFileLayoutArray[i];
    }

  // Close file
  file.close();

  // Set initialized to true
  this -> Initialized = true;

  // Return successfully
  return GLOBAL_COORDINATES_TO_FILE_MAPPER_SUCCESS;

}


//-----------------------------------------------------------------------------
bool GlobalCoordinatesToFileMapper::IsInitialized()
{

  // Return Initialized
  return this -> Initialized;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::PrintSelf(ostream& os)
{

  os << "GlobalCoordinatesToFileMapper - Begin PrintSelf ------------------\n";

  if (this -> Description != NULL)
    os << "  Description: " << this -> Description << "\n";
  else
    os << "  Description: null\n";

  os << "  File Coordinate Dimensions: "
     << this -> FileCoordinateDimensions[0] << " "
     << this -> FileCoordinateDimensions[1] << "\n";

  os << "  Global Coordinate Dimensions: "
     << this -> GlobalCoordinateDimensions[0] << " "
     << this -> GlobalCoordinateDimensions[1] << "\n";

  os << "  Global File Layout Array:\n";
  for (long i = 0 ;
       i < this -> GlobalFileLayoutDimensions[0] *
	 GlobalFileLayoutDimensions[1] ;
       i++)
    {
      if (this -> GlobalFileLayoutArray != NULL)
	{
	  if (this -> GlobalFileLayoutArray[i] != NULL)
	    {
	      os << "    " << this -> GlobalFileLayoutArray[i] << "\n";
	    }
	  else
	    {
	      os << "    null" << endl;
	    }
	}
    }

  os << "  Global File Layout Dimensions: "
     << this -> GlobalFileLayoutDimensions[0] << " "
     << this -> GlobalFileLayoutDimensions[1] << "\n";

  os << "  Query Extent: "
     << this -> QueryExtent[0] << " "
     << this -> QueryExtent[1] << " "
     << this -> QueryExtent[2] << " "
     << this -> QueryExtent[3] << "\n";

  os << "  Query File Info Dimensions: "
     << this -> QueryFileInfoDimensions[0] << " "
     << this -> QueryFileInfoDimensions[1] << "\n";

  os << "GlobalCoordinatesToFileMapper - End PrintSelf --------------------\n"
     << endl;

  return;

}


//-----------------------------------------------------------------------------
int GlobalCoordinatesToFileMapper::Query()
{

  // Check if instance is initialized
  if (this -> Initialized == false)
    {
      return GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_NOT_INITIALIZED;
    }

  // Check that query extent is within global image bounds
  if ((this -> QueryExtent[0] + this -> QueryExtent[2] >
       this -> GlobalCoordinateDimensions[0]) ||
      (this -> QueryExtent[1] + this -> QueryExtent[3] >
       this -> GlobalCoordinateDimensions[1]))
    {
      return GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_INVALID_EXTENT;
    }

  // Calculate the first column and row
  long fc = this -> QueryExtent[0] / this -> FileCoordinateDimensions[0];
  long fr = this -> QueryExtent[1] / this -> FileCoordinateDimensions[1];

  // Calculate the last column and row
  long lc = (this -> QueryExtent[0] + this -> QueryExtent[2]) /
    this -> FileCoordinateDimensions[0];
  long lr = (this -> QueryExtent[1] + this -> QueryExtent[3]) /
    this -> FileCoordinateDimensions[1];

  // Set the query file info dimensions
  this -> QueryFileInfoDimensions[0] = lc - fc + 1;
  this -> QueryFileInfoDimensions[1] = lr - fr + 1;

  // Calculate the total number of files in the result
  long tot = (lc - fc + 1) * (lr - fr + 1);

  // Delete previous query result.
  if (this -> QueryFileInfoArray != NULL)
    {
      delete [] this -> QueryFileInfoArray;
    }

  // Allocate storage for new query.
  this -> QueryFileInfoArray = new FileInfo[tot];

  // Count number of files in FileInfoArray
  long fcnt = 0;

  // Create FileInfoArray for query. Loop through each column, and row
  // of the global array specified by the bounds computed above, filling in
  // the array. This probably isn't the best way to do this, but it's what
  // I came up with in one night.
  for (long cgc = fc, clc = 0 ; cgc <= lc ; cgc++, clc++)
    {
      for (long cgr = fr, clr = 0 ; cgr <= lr ; cgr++, clr++)
	{
	  // Set file dimensions
	  this -> QueryFileInfoArray[fcnt].
	    SetDimensions(this -> FileCoordinateDimensions[0],
			  this -> FileCoordinateDimensions[1]);

	  // Determine file's position in global array
	  long pos = (cgc * this -> GlobalFileLayoutDimensions[1]) + cgr;

	  // Calculate and set the extent for the current individual file.

	  // first and last column
	  if (clc == 0 && lc - fc == 0)
	    {
	      if (this -> QueryExtent[0] >=
		  this -> FileCoordinateDimensions[0])
		{
		  this -> QueryFileInfoArray[fcnt].
		    SetExtentX(this -> QueryExtent[0] -
			       (cgc * this -> FileCoordinateDimensions[0]));
		}
	      else
		{
		  this -> QueryFileInfoArray[fcnt].SetExtentX(this ->
							      QueryExtent[0]);
		}
	      this -> QueryFileInfoArray[fcnt].SetExtentW(this ->
							  QueryExtent[2]);
	    }

	  // first and last row
	  if (clr == 0 && lr - fr == 0)
	    {
	      // if first row is from 0 then it's OK, but if it's not
	      // then subtract number of rows * fileDim
	      if (this -> QueryExtent[1] >=
		  this -> FileCoordinateDimensions[1])
		{
		  this -> QueryFileInfoArray[fcnt].
		    SetExtentY(this -> QueryExtent[1] -
			       (cgr * this -> FileCoordinateDimensions[1]));
		}
	      else
		{
		  this -> QueryFileInfoArray[fcnt].SetExtentY(this ->
							      QueryExtent[1]);
		}
	      this -> QueryFileInfoArray[fcnt].SetExtentH(this ->
							  QueryExtent[3]);

	    }

	  // first, but not last, column
	  if (clc == 0 && lc - fc != 0)
	    {
	      long tmp = this -> QueryExtent[0] -
		(cgc * this -> FileCoordinateDimensions[0]);
	      this -> QueryFileInfoArray[fcnt].SetExtentX(tmp);
	      this -> QueryFileInfoArray[fcnt].
		SetExtentW(this -> FileCoordinateDimensions[0] - tmp);
	    }

	  // first, but not last, row
	  if (clr == 0 && lr - fr != 0)
	    {
	      long tmp = this -> QueryExtent[1] -
		(cgr * this -> FileCoordinateDimensions[1]);
	      this -> QueryFileInfoArray[fcnt].SetExtentY(tmp);
	      this -> QueryFileInfoArray[fcnt].
		SetExtentH(this -> FileCoordinateDimensions[1] - tmp);
	    }

	  // last, but not only, column
	  if (clc == (lc - fc) && clc != 0)
	    {
	      long tmp = (this -> QueryExtent[0] + this -> QueryExtent[2]) -
		(cgc * this -> FileCoordinateDimensions[0]);
	      this -> QueryFileInfoArray[fcnt].SetExtentX(0);
	      this -> QueryFileInfoArray[fcnt].SetExtentW(tmp);
	    }

	  // last, but not only, row
	  if (clr == (lr - fr) && clr != 0)
	    {
	      long tmp = (this -> QueryExtent[1] + this -> QueryExtent[3]) -
		(cgr * this -> FileCoordinateDimensions[1]);
	      this -> QueryFileInfoArray[fcnt].SetExtentY(0);
	      this -> QueryFileInfoArray[fcnt].SetExtentH(tmp);
	    }

	  // Not the first and not the last column
	  if (clc != 0 && clc != (lc - fc))
	    {
	      this -> QueryFileInfoArray[fcnt].SetExtentX(0);
	      this -> QueryFileInfoArray[fcnt].
		SetExtentW(this -> FileCoordinateDimensions[0]);
	    }

	  // Not the first and not the last row
	  if (clr != 0 && clr != (lr - fr))
	    {
	      this -> QueryFileInfoArray[fcnt].SetExtentY(0);
	      this -> QueryFileInfoArray[fcnt].
		SetExtentH(this -> FileCoordinateDimensions[1]);
	    }

	  // Set file name
	  this -> QueryFileInfoArray[fcnt].
	    SetFileName(this -> GlobalFileLayoutArray[pos]);

	  // Set local file position to current local column,
	  // current local row.
	  this -> QueryFileInfoArray[fcnt].SetLocalPosition(clc, clr);

	  // Increment number of files in FileInfoArray
	  fcnt++;
	}
    }

  // Return successfully
  return GLOBAL_COORDINATES_TO_FILE_MAPPER_SUCCESS;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetDescription(const char* string)
{

  // Delete current description.
  if (this -> Description != NULL)
    {
      delete [] this -> Description;
      this -> Description = NULL;
    }

  // Copy string as new description.
  if (string != NULL)
    {
      this -> Description = new char[strlen(string) + 1];
      strcpy(this -> Description, string);
    }

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetFileCoordinateDimensions(long w,
								long h)
{

  // Set FileCoordinateDimensions
  this -> FileCoordinateDimensions[0] = w;
  this -> FileCoordinateDimensions[1] = h;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetInitializedFalse()
{

  // Set Initialized to false.
  this -> Initialized = false;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetInitializedTrue()
{

  // Set Initialized to true.
  this -> Initialized = true;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetGlobalCoordinateDimensions(long w,
								  long h)
{

  // Set GlobalCoordinateDimensions
  this -> GlobalCoordinateDimensions[0] = w;
  this -> GlobalCoordinateDimensions[1] = h;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetGlobalFileLayoutArray(char** array,
							     long cols,
							     long rows)
{

  // Set GlobalFileLayoutDimensions
  this -> GlobalFileLayoutDimensions[0] = cols;
  this -> GlobalFileLayoutDimensions[1] = rows;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetGlobalFileLayoutDimensions(long cols,
								  long rows)
{

  // Set GlobalFileLayoutDimensions
  this -> GlobalFileLayoutDimensions[0] = cols;
  this -> GlobalFileLayoutDimensions[1] = rows;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetQueryExtent(long x, long y,
						   long w, long h)
{

  // Set QueryExtent
  this -> QueryExtent[0] = x;
  this -> QueryExtent[1] = y;
  this -> QueryExtent[2] = w;
  this -> QueryExtent[3] = h;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetQueryFileInfoArray(FileInfo* array,
							  long cols,
							  long rows)
{

  // Check if an array exists, and if so delete it.
  if (this -> QueryFileInfoArray != NULL)
    {
      delete [] this -> QueryFileInfoArray;
    }

  // Assigned array. NOTE: This is bad, very, very bad. Should be a deep
  // copy operation.
  this -> QueryFileInfoArray = array;

  // Set QueryFileInfoDimensions
  this -> QueryFileInfoDimensions[0] = cols;
  this -> QueryFileInfoDimensions[1] = rows;

  return;

}


//-----------------------------------------------------------------------------
void GlobalCoordinatesToFileMapper::SetQueryFileInfoDimensions(long cols,
							       long rows)
{

  // Set QueryFileInfoDimensions
  this -> QueryFileInfoDimensions[0] = cols;
  this -> QueryFileInfoDimensions[1] = rows;

  return;

}


//-----------------------------------------------------------------------------
GlobalCoordinatesToFileMapper::~GlobalCoordinatesToFileMapper()
{

  // Deallocate memory used for description.
  if (this -> Description != NULL)
    {
      delete [] this -> Description;
    }

  // Deallocate memory used for the GlobalFileLayoutArray
  if (this -> GlobalFileLayoutArray != NULL)
    {
      for (long i = 0 ;
	   i < this -> GlobalFileLayoutDimensions[0] *
	     this -> GlobalFileLayoutDimensions[1] ;
	   i++)
	{
	  if (this -> GlobalFileLayoutArray[i] != NULL)
	    {
	      delete [] this -> GlobalFileLayoutArray[i];
	    }
	}
      delete [] this -> GlobalFileLayoutArray;
    }

  // Deallocate memory for QueryFileInfoArray
  if (this -> QueryFileInfoArray != NULL)
    {
      delete [] this -> QueryFileInfoArray;
    }

}
