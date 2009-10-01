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

#ifndef CACHE_VRAM_NODE_H
#define CACEH_VRAM_NODE_H

/*--------------------------------------------------------------------------*/

#include "OctreeNode.h"

/*--------------------------------------------------------------------------*/

class CacheVRAMNode {

public:

  // Default constructor
  CacheVRAMNode();

  // Default destructor
  ~CacheVRAMNode();

  // Get next node
  CacheVRAMNode* GetNext();

  // Get node
  OctreeNode* GetOctreeNode();

  // Get previous node
  CacheVRAMNode* GetPrevious();

  // Set next node
  void SetNext(CacheVRAMNode* node);

  // Set node
  void SetOctreeNode(OctreeNode* node);

  // Set previous node
  void SetPrevious(CacheVRAMNode* node);

private:

  // Node
  OctreeNode* _node;

  // Previous node in list
  CacheVRAMNode* _previous;

  // Next node in list
  CacheVRAMNode* _next;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
