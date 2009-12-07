/*---------------------------------------------------------------------------*/
/* Volume Rendering Application                                              */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                  */
/*                                                                           */
/* This software is free software; you can redistribute it and/or modify it  */
/* under the terms of the GNU Lesser General Public License as published by  */
/* the Free Software Foundation; either Version 2.1 of the License, or       */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This software is distributed in the hope that it will be useful, but      */
/* WITHOUT ANY WARRANTY; without even the implied warranty of                */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser   */
/* General Public License for more details.                                  */
/*                                                                           */
/* You should have received a copy of the GNU Lesser Public License along    */
/* with this library; if not, write to the Free Software Foundation, Inc.,   */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                     */
/*---------------------------------------------------------------------------*/

#ifndef DATA_PACKER_H
#define DATA_PACKER_H

/*---------------------------------------------------------------------------*/

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

// Status
#define DATA_PACKER_OK 0
#define DATA_PACKER_ERROR 1

// Data types
#define DATA_PACKER_CHAR 2
#define DATA_PACKER_FLOAT 3
#define DATA_PACKER_INT 4
#define DATA_PACKER_UNSIGNED_CHAR 5

// Data type sizes
#define DATA_PACKER_CHAR_SIZE 1
#define DATA_PACKER_FLOAT_SIZE 4
#define DATA_PACKER_INT_SIZE 4
#define DATA_PACKER_UNSIGNED_CHAR_SIZE 1

/*---------------------------------------------------------------------------*/

class DataPacker {

public:

  // Default constructor
  DataPacker();

  // Default destructor
  ~DataPacker();

  // Get buffer and buffer size
  void* GetBuffer(int* size);

  // Get buffer position
  int GetPosition();

  // Pack char
  int PackChar(char value);

  // Pack float
  int PackFloat(float value);

  // Pack int
  int PackInt(int value);

  // Pack unsigned char
  int PackUnsignedChar(unsigned char value);

  // Reset
  void Reset();

  // Set buffer and buffer size
  void SetBuffer(void* buffer, int size);

  // Set buffer position
  int SetPosition(int position);

  // Pack char
  int UnpackChar(char* value);

  // Pack float
  int UnpackFloat(float* value);

  // Pack int
  int UnpackInt(int* value);

  // Pack unsigned char
  int UnpackUnsignedChar(unsigned char* value);

private:

  // Data buffer
  unsigned char* _buffer;

  // Buffer position
  int _position;

  // Buffer size
  int _size;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
