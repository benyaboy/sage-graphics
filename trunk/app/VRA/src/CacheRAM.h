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

#ifndef CACHE_RAM_H
#define CACHE_RAM_H

/*--------------------------------------------------------------------------*/

#include "CacheRAMNode.h"
#include "Log.h"
#include "OctreeNode.h"

/*--------------------------------------------------------------------------*/

class CacheRAM {
  
public:
  
  // Default constructor
  CacheRAM(int size);
  
  // Default destructor
  ~CacheRAM();

  // Is cache full
  bool IsFull();

  // Is item loaded
  bool IsLoaded(OctreeNode* node);

  // Load item
  void Load(OctreeNode* node);

  // Make item current
  void MakeCurrent(OctreeNode* node);

  // Move item to front of cache
  void MoveToFront(OctreeNode* node);

  // Print cache
  void Print();

  // Remove last item in cache
  void RemoveLast();
  
private:

  // Print a message to stdout
  void Assert(const char* message);

  // Current size of cache
  int _currentSize;

  // Maximum size of cache
  int _maximumSize;

  // Front of cache
  CacheRAMNode* _front;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
