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

#ifndef OCTREE_NODE_QUEUE_H
#define OCTREE_NODE_QUEUE_H

/*--------------------------------------------------------------------------*/

#include "Log.h"
#include "OctreeNode.h"

/*--------------------------------------------------------------------------*/

class OctreeNodeQueue {

 public:

  // Default constructor
  OctreeNodeQueue();

  // Constructor to specify size of queue
  OctreeNodeQueue(int size);

  // Default destructor
  ~OctreeNodeQueue();

  // Clear queue
  void Clear();

  // Is queue empty
  bool IsEmpty();

  // Is queue full
  bool IsFull();

  // Insert at back of queue
  bool Insert(OctreeNode* node);

  // Print
  void Print();

  // Remove from front of queue
  OctreeNode* Remove();

 private:

  // Print a message to stdout
  void Assert(const char* message);

  // Array
  OctreeNode** _array;

  // Back
  int _back;

  // Current size
  int _currentSize;

  // Front
  int _front;

  // Maximum size
  int _maximumSize;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
