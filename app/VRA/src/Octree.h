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

#ifndef OCTREE_H
#define OCTREE_H

/*--------------------------------------------------------------------------*/

#include "OctreeNode.h"

/*--------------------------------------------------------------------------*/

class Octree {

 public:

  // Default constructor
  Octree();

  // Default destructor
  ~Octree();

  // Get data format
  int GetDataFormat();

  // Get data type
  int GetDataType();

  // Get number of levels
  int GetNumberOfLevels();

  // Get number of nodes
  int GetNumberOfNodes();

  // Get root
  OctreeNode* GetRoot();

  // Load octree recursively from file
  void LoadFromFile(const char* filename);

  // Load octree from file
  void LoadFromFile2(const char* filename);

  // Load octree from file recursively called from LoadFromFile2
  void LoadFromFileRecursive(FILE* fptr, OctreeNode* node);

  // Set data format
  void SetDataFormat(int format);

  // Set data type
  void SetDataType(int type);

  // Set number of levels
  void SetNumberOfLevels(int numberOfLevels);

  // Set number of nodes
  void SetNumberOfNodes(int numberOfNodes);

  // Set file prefix
  void SetPrefix(const char* prefix);

  // Set root
  void SetRoot(OctreeNode* root);

 private:

  // Data format
  int _dataFormat;

  // Data type
  int _dataType;

  // Number of levels;
  int _numberOfLevels;

  // Number of nodes
  int _numberOfNodes;

  // File prefix
  char _prefix[1024];

  // Root node
  OctreeNode* _root;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
