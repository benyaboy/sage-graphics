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

#include "CacheVRAMNode.h"

/*--------------------------------------------------------------------------*/

CacheVRAMNode::CacheVRAMNode() {

  // Initialize cache node values to NULL
  _previous = NULL;
  _next = NULL;
  _node = NULL;

}

/*--------------------------------------------------------------------------*/

CacheVRAMNode::~CacheVRAMNode() {
}

/*--------------------------------------------------------------------------*/

CacheVRAMNode* CacheVRAMNode::GetNext() {

  // Return next cache node
  return _next;

}

/*--------------------------------------------------------------------------*/

OctreeNode* CacheVRAMNode::GetOctreeNode() {

  // Return octree node
  return _node;

}

/*--------------------------------------------------------------------------*/

CacheVRAMNode* CacheVRAMNode::GetPrevious() {

  // Return previous cache node
  return _previous;

}

/*--------------------------------------------------------------------------*/

void CacheVRAMNode::SetNext(CacheVRAMNode* node) {

  // Set next cache node
  _next = node;

}

/*--------------------------------------------------------------------------*/

void CacheVRAMNode::SetOctreeNode(OctreeNode* node) {

  // Set octree node
  _node = node;

}

/*--------------------------------------------------------------------------*/

void CacheVRAMNode::SetPrevious(CacheVRAMNode* node) {

  // Set previous cache node
  _previous = node;

}

/*--------------------------------------------------------------------------*/
