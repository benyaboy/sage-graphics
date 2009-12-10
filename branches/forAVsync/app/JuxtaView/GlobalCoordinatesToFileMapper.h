/******************************************************************************
 * Program: GlobalCoordinatesToFileMapper
 * Module:  GlobalCoordinatesToFileMapper.h
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
// .NAME GlobalCoordinatesToFileMapper
// .SECTION Description
// GlobalCoordinatesToFileMapper is a class that maps from the global
// coordinate space, usually pixels, of an image formed from a montage of
// smaller, identically sized, rectangular images to the real image files
// that the larger images consists of.


#ifndef GLOBAL_COORDINATES_TO_FILE_MAPPER_H
#define GLOBAL_COORDINATES_TO_FILE_MAPPER_H


// Preprocessor definition error code for invalid extent
#define GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_INVALID_EXTENT 0

// Preporcessor definition error code for invalid file name
#define GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_INVALID_FILE 1

// Preprocessor definition error code for un-initialized mapper
#define GLOBAL_COORDINATES_TO_FILE_MAPPER_ERROR_NOT_INITIALIZED 2

// Preprocessor definition to indicate operation success
#define GLOBAL_COORDINATES_TO_FILE_MAPPER_SUCCESS 3


#include <fstream.h>
#include <stddef.h>
#include <string.h>


// class FileInfo holds information about individual files.
#include "FileInfo.h"


class GlobalCoordinatesToFileMapper {

 public:

  // Description:
  // Constructor
  GlobalCoordinatesToFileMapper();

  // Description:
  // Get description of the global image.
  const char* GetDescription();

  // Description:
  // Get height of a single file that makes up the global image in coordinates
  // local to the file, usually pixels. We assume that all files are the same
  // size.
  long GetFileCoordinateDimensionH();

  // Description:
  // Get width of a single file that makes up the global image in coordinates
  // local to the file, usually pixels. We assume that all files are the same
  // size.
  long GetFileCoordinateDimensionW();

  // Description:
  // Get height of the global image in global coordinates, usually pixels.
  long GetGlobalCoordinateDimensionH();

  // Description:
  // Get width of the global image in global coordinates, usually pixels.
  long GetGlobalCoordinateDimensionW();

  // Description:
  // Get extent for a given file that makes up the global image in global
  // coordinates, usually pixels.
  long GetGlobalCoordinates(const char* name,
			    long* x,
			    long* y,
			    long* w,
			    long* h);

  // Description:
  // Get the GlobalFileLayoutArray, and its corresponding dimensions.The
  // GlobalFileLayoutArray is an adjacency matrix that specifies the layout of
  // files in the global image. The matrix starts in the upper left of the
  // global image, and is stored in column, row order. The matrix is an array
  // of pointers to character strings. Each character string contains the name
  // of its corresponding file. This class allocates and deallocates storage
  // for this array.
  char** GetGlobalFileLayoutArray(long* cols,
				  long* rows);

  // Description:
  // Get the dimensions of the GlobalFileArray. The GlobalFileLayoutArray is
  // an adjacency matrix that specifies the layout of files in the global
  // image. The matrix starts in the upper left of the global image, and is
  // stored in column, row order. The matrix is an array of pointers to
  // character strings. Each character string contains the name of its
  // corresponding file. This class allocates and deallocates storage for this
  // array.
  void GetGlobalFileLayoutDimensions(long* cols,
				     long* rows);

  // Description:
  // Get the height of the current query's extent in global coordinates,
  // usually pixels.
  long GetQueryExtentH();

  // Description:
  // Get the width of the current query's extent in global coordinates,
  // usually pixels.
  long GetQueryExtentW();

  // Description:
  // Get the starting x position of the current query's extent in global
  // coordinates, usually pixels. We assume that position (0,0) is in the
  // upper left corner.
  long GetQueryExtentX();

  // Description:
  // Get the starting y position of the current query's extent in global
  // coordinates, usually pixels. We assume that position (0,0) is in the
  // upper left corner.
  long GetQueryExtentY();

  // Description:
  // Get the FileInfoArray, and its corresponding dimensions. The FileInfoArray
  // is a 1D array of type FileInfo stored in column, row order. The FileInfo
  // array is updated by the Query() method. When making a query this array is
  // its result.
  FileInfo* GetQueryFileInfoArray(long* cols,
				  long* rows);

  // Description:
  // Get the dimensions of the FileInfoArray. The FileInfoArray is a 1D array
  // of type FileInfo stored in column, row order. The FileInfo array is
  // updated by the Query() method.
  void GetQueryFileInfoDimensions(long* cols,
				  long* rows);

  // Description:
  // Initialize the class based on information in the file given.
  int Initialize(const char* name);

  // Description:
  // Returns whether this instance of the class has been initialized.
  bool IsInitialized();

  // Description:
  // Print self.
  void PrintSelf(ostream& os);

  // Description:
  // Query this instance of the class. Returns an error if the instance is
  // not initialized, or the data extent is not within the global coordinate
  // space. Call this method each time the query extent is changed. When the
  // query is complete, the FileInfoArray is updated.
  int Query();

  // Description:
  // Set description for the global image.
  void SetDescription(const char* string);

  // Description:
  // Set the dimensions of a single file that makes up the global image in
  // coordinates local to the file, usually pixels. We assume that all files
  // are the same size.
  void SetFileCoordinateDimensions(long w,
				   long h);

  // Description:
  // Set the state of this instance to not initialized. This method should only
  // be invoked by the Initialize(...) method.
  void SetInitializedFalse();

  // Description:
  // Set the state of this instance to initialized. This method should only be
  // invoked by the Initialize(...) method.
  void SetInitializedTrue();

  // Description:
  // Set the dimensions of the global image in global coordinates, usually
  // pixels.
  void SetGlobalCoordinateDimensions(long w,
				     long h);

  // Description:
  // Set the GlobalFileLayoutArray, along with its dimensions in columns and
  // rows. The GlobalFileLayoutArray is an adjacency matrix that specifies
  // the layout of files in the global image. The matrix starts in the upper
  // left of the global image, and is stored in column, row order. The matrix
  // is an array of pointers to character strings. Each character string
  // contains the name of its corresponding file. This class allocates and
  // deallocates storage for this array. This method should only be invoked by
  // the Initialize(...) method.
  void SetGlobalFileLayoutArray(char** array,
				long cols,
				long rows);

  // Description:
  // Set the dimensions of the GlobalFileArray. The GlobalFileLayoutArray is
  // an adjacency matrix that specifies the layout of files in the global
  // image. The matrix starts in the upper left of the global image, and is
  // stored in column, row order. The matrix is an array of pointers to
  // character strings. Each character string contains the name of its
  // corresponding file. This class allocates and deallocates storage for this
  // array. This method should only be invoked by the Initialize(...), or
  // SetGlobalFileLayoutArray(...) methods.
  void SetGlobalFileLayoutDimensions(long cols,
				     long rows);

  // Description:
  // Set the extent of the current query. Set this information before calling
  // the Query() method.
  void SetQueryExtent(long x, long y,
		      long w, long h);

  // Description:
  // Set the FileInfoArray, and its corresponding dimensions. The FileInfoArray
  // is a 1D array of type FileInfo stored in column, row order. This method
  // should only be invoked by the Query() method.
  void SetQueryFileInfoArray(FileInfo* array,
			     long cols,
			     long rows);

  // Description:
  // Set the dimensions of the FileInfoArray. The FileInfoArray is a 1D array
  // of type FileInfo stored in column, row order. This method should only be
  // invoked by the Query(), or SetQueryFileInfoArray(...) methods.
  void SetQueryFileInfoDimensions(long cols,
				  long rows);

  // Description:
  // Destructor
  ~GlobalCoordinatesToFileMapper();


 private:

  // Description:
  // Description of the global image.
  char* Description;

  // Description:
  // The width and height of a single file that makes up the global image in
  // coordinates local to the file, usually pixels. We assume that all files
  // are the same size.
  long FileCoordinateDimensions[2];

  // Description:
  // The width and height of the global image in global coordinates, usually
  // pixels.
  long GlobalCoordinateDimensions[2];

  // Description:
  // The GlobalFileLayoutArray is an adjacency matrix that specifies the
  // layout of files in the global image. The matrix starts in the upper left
  // of the global image, and is stored in column, row order. The matrix is an
  // array of pointers to character strings. Each character string contains
  // the name of its corresponding file. This class allocates and deallocates
  // storage for this array.
  char** GlobalFileLayoutArray;

  // Description:
  // The number of columns and rows of files that make up the global image.
  long GlobalFileLayoutDimensions[2];

  // Description:
  // Flag indicating whether the instance of the class is initialized.
  bool Initialized;

  // Description:
  // The extent of the current query as the x and y locations of the upper
  // left corner of the extent, and the width and height of the extent in
  // global coordinates, usually pixels.
  long QueryExtent[4];

  // Description:
  // The FileInfoArray is a 1D array of type FileInfo stored in column, row
  // order. The FileInfo array is updated by the Query() method. When making a
  // query this array is its result.
  FileInfo* QueryFileInfoArray;

  // Description:
  // The number of columns and rows in the FileInfoArray. Updated by the
  // Query() method.
  long QueryFileInfoDimensions[2];

};


#endif
