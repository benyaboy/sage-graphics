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

#include "DataPacker.h"

/*---------------------------------------------------------------------------*/

DataPacker::DataPacker() {

  // Initialize to NULL
  _buffer = NULL;

  // Initialize to -1
  _position = -1;

  // Initialize to 0
  _size = 0;

}

/*---------------------------------------------------------------------------*/

DataPacker::~DataPacker() {
}

/*---------------------------------------------------------------------------*/

void* DataPacker::GetBuffer(int* size) {

  // Set size
  *size = _size;

  // Return buffer
  return (void*) _buffer;

}

/*---------------------------------------------------------------------------*/

int DataPacker::GetPosition() {

  // Return position
  return _position;

}

/*---------------------------------------------------------------------------*/

int DataPacker::PackChar(char value) {

  // Check buffer size
  if (_position + DATA_PACKER_CHAR_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Pack value
  _buffer[_position] = value;

  // Increment position
  _position += DATA_PACKER_CHAR_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::PackFloat(float value) {

  // Check buffer size
  if (_position + DATA_PACKER_FLOAT_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Pack value
  uint32_t tmp = htonl(*(uint32_t*) ((void*) &value));
  memcpy(&_buffer[_position], &tmp, DATA_PACKER_FLOAT_SIZE);

  // Increment position
  _position += DATA_PACKER_FLOAT_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::PackInt(int value) {

  // Check buffer size
  if (_position + DATA_PACKER_INT_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Pack value
  uint32_t tmp = htonl(*(uint32_t*) ((void*) &value));
  memcpy(&_buffer[_position], &tmp, DATA_PACKER_INT_SIZE);

  // Increment position
  _position += DATA_PACKER_INT_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::PackUnsignedChar(unsigned char value) {

  // Check buffer size
  if (_position + DATA_PACKER_UNSIGNED_CHAR_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Pack value
  _buffer[_position] = value;

  // Increment position
  _position += DATA_PACKER_UNSIGNED_CHAR_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

void DataPacker::Reset() {

  // Reset to NULL
  _buffer = NULL;

  // Reset to -1
  _position = -1;

  // Reset to 0
  _size = 0;

}

/*---------------------------------------------------------------------------*/

void DataPacker::SetBuffer(void* buffer, int size) {

  // Set buffer
  _buffer = (unsigned char*) buffer;

  // Set position
  _position = 0;

  // Set size
  _size = size;

}

/*---------------------------------------------------------------------------*/

int DataPacker::SetPosition(int position) {

  // Check buffer size and position
  if (position >= _size || position < 0) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Set position
  _position = position;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::UnpackChar(char* value) {

  // Check buffer size
  if (_position + DATA_PACKER_CHAR_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Unpack value
  *value = _buffer[_position];

  // Increment position
  _position += DATA_PACKER_UNSIGNED_CHAR_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::UnpackFloat(float* value) {

  // Check buffer size
  if (_position + DATA_PACKER_FLOAT_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Unpack value
  uint32_t tmp = ntohl(*(uint32_t*) ((void*) &_buffer[_position]));
  memcpy(value, &tmp, DATA_PACKER_FLOAT_SIZE);

  // Increment position
  _position += DATA_PACKER_FLOAT_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::UnpackInt(int* value) {

  // Check buffer size
  if (_position + DATA_PACKER_INT_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Unpack value
  uint32_t tmp = ntohl(*(uint32_t*) ((void*) &_buffer[_position]));
  memcpy(value, &tmp, DATA_PACKER_INT_SIZE);

  // Increment position
  _position += DATA_PACKER_INT_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/

int DataPacker::UnpackUnsignedChar(unsigned char* value) {

  // Check buffer size
  if (_position + DATA_PACKER_CHAR_SIZE - 1 >= _size) {

    // Return ERROR
    return DATA_PACKER_ERROR;

  }

  // Unpack value
  *value = _buffer[_position];

  // Increment position
  _position += DATA_PACKER_UNSIGNED_CHAR_SIZE;

  // Return OK
  return DATA_PACKER_OK;

}

/*---------------------------------------------------------------------------*/
