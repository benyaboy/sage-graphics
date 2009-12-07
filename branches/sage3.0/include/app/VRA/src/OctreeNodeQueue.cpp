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

#include "OctreeNodeQueue.h"

/*--------------------------------------------------------------------------*/

OctreeNodeQueue::OctreeNodeQueue() {

  // Initialize queue values
  _back = -1;
  _currentSize = 0;
  _front = 0;
  _maximumSize = 0;

  // Allocate memory for queue
  if ((_array = new OctreeNode*[128]) == NULL) {
    fprintf(stderr, "OctreeNodeQueue: Memory allocation error.\n");
  }
  else {
    _maximumSize = 128;
  }

}

/*--------------------------------------------------------------------------*/

OctreeNodeQueue::OctreeNodeQueue(int size) {

  // Initialize queue values
  _back = -1;
  _currentSize = 0;
  _front = 0;
  _maximumSize = 0;

  // Allocate memory for queue
  if ((_array = new OctreeNode*[size]) == NULL) {
    fprintf(stderr, "OctreeNodeQueue: Memory allocation error.\n");
  }
  else {
    _maximumSize = size;
  }

}

/*--------------------------------------------------------------------------*/

OctreeNodeQueue::~OctreeNodeQueue() {

  // Clean up queue
  if (_array != NULL) {
    delete [] _array;
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNodeQueue::Assert(const char* message) {

#ifdef LOG

  // Print message and time to log
  char buffer[1024];
  sprintf(buffer, "OctreeNodeQueue: %s", message);
  Log::Assert(buffer);

#endif

}

/*--------------------------------------------------------------------------*/

void OctreeNodeQueue::Clear() {

  // Reset queue values
  _back = -1;
  _currentSize = 0;
  _front = 0;

  // Clear queue
  for (int i = 0 ; i < _maximumSize ; i++) {
    _array[i] = NULL;
  }

}

/*--------------------------------------------------------------------------*/

bool OctreeNodeQueue::IsEmpty() {

  // Check if queue is empty
  if (_currentSize == 0) {
    return true;
  }
  else {
    return false;
  }

}

/*--------------------------------------------------------------------------*/

bool OctreeNodeQueue::IsFull() {

  // Check if queue is full
  if (_currentSize == _maximumSize) {
    return true;
  }
  else {
    return false;
  }

}

/*--------------------------------------------------------------------------*/

bool OctreeNodeQueue::Insert(OctreeNode* node) {

  // Check if queue is full
  if (IsFull() == true) {
    return false;
  }

  // If end reaches end of array, set it to beginning
  if (++_back == _maximumSize) {
    _back = 0;
  }

  // Set node in queue
  _array[_back] = node;

  // Increment size
  _currentSize++;

  // Return
  return true;

}

/*--------------------------------------------------------------------------*/

void OctreeNodeQueue::Print() {

  // Message
  Assert("Printing OctreeNodeQueue begin.");

  // Message
  char message[1024];
  sprintf(message, "  Current Size: %d", _currentSize);
  Assert(message);
  sprintf(message, "  Maximum Size: %d", _maximumSize);
  Assert(message);

  // Print each node
  for (int i = 0 ; i < _currentSize ; i++) {

    // Location
    int location = (_front + i) % _maximumSize;

    // Print node
    _array[i] -> Print();

  }

  // Message
  Assert("Printing OctreeNodeQueue end.");

}

/*--------------------------------------------------------------------------*/

OctreeNode* OctreeNodeQueue::Remove() {

  // Check if queue is empty
  if (IsEmpty() == true) {
    return NULL;
  }

  // Decrement size
  _currentSize--;

  // Get node from front
  OctreeNode* node = _array[_front];

  // Set front to NULL
  _array[_front] = NULL;

  // If front reaches end of array, set it to beginning
  if (++_front == _maximumSize) {
    _front = 0;
  }

  // Return node
  return node;

}

/*--------------------------------------------------------------------------*/
