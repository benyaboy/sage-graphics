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

#include "Octree.h"

/*--------------------------------------------------------------------------*/

Octree::Octree() {

  // Initialize data format
  _dataFormat = OCTREE_NODE_BIG_ENDIAN;

  // Initialize data type
  _dataType = OCTREE_NODE_UNSIGNED_8_INT;

  // Initialize data prefix
  memset(_prefix, 0, 1024);

  // Initialize root to NULL
  _root = NULL;

}

/*--------------------------------------------------------------------------*/

Octree::~Octree() {

  // Clean up octree recursively
  if (_root != NULL) {
    delete _root;
  }

}

/*--------------------------------------------------------------------------*/

int Octree::GetDataFormat() {

  // Return data format
  return _dataFormat;

}

/*--------------------------------------------------------------------------*/

int Octree::GetDataType() {

  // Return data type
  return _dataType;

}

/*--------------------------------------------------------------------------*/

int Octree::GetNumberOfLevels() {

  // Return number of levels in octree
  return _numberOfLevels;

}

/*--------------------------------------------------------------------------*/

int Octree::GetNumberOfNodes() {

  // Return number of nodes in octree
  return _numberOfNodes;

}

/*--------------------------------------------------------------------------*/

OctreeNode* Octree::GetRoot() {

  // Return root node
  return _root;

}

/*--------------------------------------------------------------------------*/

void Octree::LoadFromFile(const char* filename) {
  fprintf(stderr, "LFF\n");
  // Create full path and filename of file to load
  char path[2048];
  memset(path, 0, sizeof(char) * 2048);
  memcpy(path, _prefix, sizeof(char) * strlen(_prefix));
  memcpy(&path[strlen(path)], filename, sizeof(char) * strlen(filename));

  // Open file
  FILE* metafptr;
  if ((metafptr = fopen(path, "r")) == NULL) {
    fprintf(stderr, "Octree: Error reading metadata file %s.\n", path);
    return;
  }

  // Read name of root node file
  char datafilename[128];
  fscanf(metafptr, "%s", datafilename);

  // Read number of levels in octree
  int numberOfLevels = 0;
  fscanf(metafptr, "%d", &numberOfLevels);

  // Read number of nodes in octree
  int numberOfNodes = 0;
  fscanf(metafptr, "%d", &numberOfNodes);

  // Read data type of nodes in octree
  char dataTypeString[128];
  fscanf(metafptr, "%s", dataTypeString);

  // Read data format of nodes in octree
  char dataFormatString[128];
  fscanf(metafptr, "%s", dataFormatString);

  // Close file
  fclose(metafptr);

  // Allocate root node
  if ((_root = new OctreeNode) == NULL) {
    fprintf(stderr, "Octree: Memory allocation error.\n");
    return;
  }

  // Load root node and entire tree recursively
  _root -> SetPrefix(_prefix);
  _root -> LoadFromFile(datafilename);

  // Set number of levels and nodes in octree
  _numberOfLevels = numberOfLevels;
  _numberOfNodes = numberOfNodes;

  // Set data type of nodes in octree
  if (!strcmp(dataTypeString, "UNSIGNED_8_INT")) {
    _dataType = OCTREE_NODE_UNSIGNED_8_INT;
  }
  else if (!strcmp(dataTypeString, "UNSIGNED_16_INT")) {
    _dataType = OCTREE_NODE_UNSIGNED_16_INT;
  }
  else {
    fprintf(stderr, "Octree: Unknown data type.\n");
    _dataType = OCTREE_NODE_UNSIGNED_8_INT;
  }

  // Set data format of nodes in octree
  if (!strcmp(dataFormatString, "BIG_ENDIAN")) {
    _dataFormat = OCTREE_NODE_BIG_ENDIAN;
  }
  else if (!strcmp(dataFormatString, "LITTLE_ENDIAN")) {
    _dataFormat = OCTREE_NODE_LITTLE_ENDIAN;
  }
  else {
    fprintf(stderr, "Octree: Unknown data format.\n");
    _dataType = OCTREE_NODE_BIG_ENDIAN;
  }

}

/*--------------------------------------------------------------------------*/

void Octree::LoadFromFile2(const char* filename) {

  // Create full path and filename of file to load
  char path[2048];
  memset(path, 0, sizeof(char) * 2048);
  memcpy(path, _prefix, sizeof(char) * strlen(_prefix));
  memcpy(&path[strlen(path)], filename, sizeof(char) * strlen(filename));

  // Open file
  FILE* metafptr;
  if ((metafptr = fopen(path, "r")) == NULL) {
    fprintf(stderr, "Octree: Error reading metadata file %s.\n", path);
    return;
  }


  // Read number of levels in octree
  int numberOfLevels = 0;
  fscanf(metafptr, "%d", &numberOfLevels);

  // Read number of nodes in octree
  int numberOfNodes = 0;
  fscanf(metafptr, "%d", &numberOfNodes);

  // Read data type of nodes in octree
  char dataTypeString[128];
  fscanf(metafptr, "%s", dataTypeString);

  // Read data format of nodes in octree
  char dataFormatString[128];
  fscanf(metafptr, "%s", dataFormatString);

  // Set data type of nodes in octree
  if (!strcmp(dataTypeString, "UNSIGNED_8_INT")) {
    _dataType = OCTREE_NODE_UNSIGNED_8_INT;
  }
  else if (!strcmp(dataTypeString, "UNSIGNED_16_INT")) {
    _dataType = OCTREE_NODE_UNSIGNED_16_INT;
  }
  else {
    fprintf(stderr, "Octree: Unknown data type.\n");
    _dataType = OCTREE_NODE_UNSIGNED_8_INT;
  }

  // Set data format of nodes in octree
  if (!strcmp(dataFormatString, "BIG_ENDIAN")) {
    _dataFormat = OCTREE_NODE_BIG_ENDIAN;
  }
  else if (!strcmp(dataFormatString, "LITTLE_ENDIAN")) {
    _dataFormat = OCTREE_NODE_LITTLE_ENDIAN;
  }
  else {
    fprintf(stderr, "Octree: Unknown data format.\n");
    _dataType = OCTREE_NODE_BIG_ENDIAN;
  }

  // Set number of levels and nodes in octree
  _numberOfLevels = numberOfLevels;
  _numberOfNodes = numberOfNodes;


  // Allocate root node
  if ((_root = new OctreeNode) == NULL) {
    fprintf(stderr, "Octree: Memory allocation error.\n");
    return;
  }

  // Load root node prefix
  _root -> SetPrefix(_prefix);

  // load recursively
  LoadFromFileRecursive(metafptr, _root);


  // Close file
  fclose(metafptr);

}

/*--------------------------------------------------------------------------*/

void Octree::LoadFromFileRecursive(FILE* metafptr, OctreeNode* node) {

  // id
  int id;

  // level
  int level;

  // vertices
  float v0x, v0y, v0z;
  float v1x, v1y, v1z;
  float v2x, v2y, v2z;
  float v3x, v3y, v3z;
  float v4x, v4y, v4z;
  float v5x, v5y, v5z;
  float v6x, v6y, v6z;
  float v7x, v7y, v7z;

  // width, height, depth
  int w, h, d;

  // Data type
  char dataTypeString[128];

  // Data format
  char dataFormatString[128];

  // raw data file name
  char datafilename[128];

  // number of children
  int numberofchildren;


  // read id and level
  fscanf(metafptr, "%d %d", &id, &level);

  // read vertices
  fscanf(metafptr, "%f %f %f", &v0x, &v0y, &v0z);
  fscanf(metafptr, "%f %f %f", &v1x, &v1y, &v1z);
  fscanf(metafptr, "%f %f %f", &v2x, &v2y, &v2z);
  fscanf(metafptr, "%f %f %f", &v3x, &v3y, &v3z);
  fscanf(metafptr, "%f %f %f", &v4x, &v4y, &v4z);
  fscanf(metafptr, "%f %f %f", &v5x, &v5y, &v5z);
  fscanf(metafptr, "%f %f %f", &v6x, &v6y, &v6z);
  fscanf(metafptr, "%f %f %f", &v7x, &v7y, &v7z);

  // read width, height, depth
  fscanf(metafptr, "%d %d %d", &w, &h, &d);

  // read data type
  fscanf(metafptr, "%s", dataTypeString);

  // read data format
  fscanf(metafptr, "%s", dataFormatString);

  // read raw data filename
  fscanf(metafptr, "%s", datafilename);

  // read children flag
  fscanf(metafptr, "%d", &numberofchildren);


  // assign id and level
  node -> SetID(id);
  node -> SetLevel(level);

  // assign vertices
  node -> SetVertex(0, v0x, v0y, v0z);
  node -> SetVertex(1, v1x, v1y, v1z);
  node -> SetVertex(2, v2x, v2y, v2z);
  node -> SetVertex(3, v3x, v3y, v3z);
  node -> SetVertex(4, v4x, v4y, v4z);
  node -> SetVertex(5, v5x, v5y, v5z);
  node -> SetVertex(6, v6x, v6y, v6z);
  node -> SetVertex(7, v7x, v7y, v7z);

  // assign width, height, depth
  node -> SetWHD(w, h, d);

  // assign data type
  if (!strcmp(dataTypeString, "UNSIGNED_8_INT")) {
    node -> SetDataType(OCTREE_NODE_UNSIGNED_8_INT);
  }
  else if (!strcmp(dataTypeString, "UNSIGNED_16_INT")) {
    node -> SetDataType(OCTREE_NODE_UNSIGNED_16_INT);
  }
  else {
    fprintf(stderr, "OctreeNode: Unknown data type.\n");
    node -> SetDataType(OCTREE_NODE_UNSIGNED_8_INT);
  }

  // assign data format
  if (!strcmp(dataFormatString, "BIG_ENDIAN")) {
    node -> SetDataFormat(OCTREE_NODE_BIG_ENDIAN);
  }
  else if (!strcmp(dataFormatString, "LITTLE_ENDIAN")) {
    node -> SetDataFormat(OCTREE_NODE_LITTLE_ENDIAN);
  }
  else {
    fprintf(stderr, "OctreeNode: Unknown data format.\n");
    node -> SetDataFormat(OCTREE_NODE_BIG_ENDIAN);
  }

  // assign filename
  node -> SetFilename(datafilename);


  // load children
  for (int i = 0 ; i < numberofchildren ; i++) {

    // allocate child node
    OctreeNode* child = NULL;
    if ((child = new OctreeNode) == NULL) {
      fprintf(stderr, "OctreeNode: Memory allocation error.\n");
    }

    // set child
    node -> SetChild(i, child);
    child -> SetPrefix(_prefix);

    // load recursively
    LoadFromFileRecursive(metafptr, child);

  }

}

/*--------------------------------------------------------------------------*/

void Octree::SetDataFormat(int format) {

  // Set data format
  _dataFormat = format;

}

/*--------------------------------------------------------------------------*/

void Octree::SetDataType(int type) {

  // Set data type
  _dataType = type;

}

/*--------------------------------------------------------------------------*/

void Octree::SetNumberOfLevels(int numberOfLevels) {

  // Set number of levels in octree
  _numberOfLevels = numberOfLevels;

}

/*--------------------------------------------------------------------------*/

void Octree::SetNumberOfNodes(int numberOfNodes) {

  // Set number of nodes in octree
  _numberOfNodes = numberOfNodes;

}

/*--------------------------------------------------------------------------*/

void Octree::SetPrefix(const char* prefix) {

  // Set data prefix
  if (prefix != NULL) {
    strcpy(_prefix, prefix);
  }

}

/*--------------------------------------------------------------------------*/

void Octree::SetRoot(OctreeNode* root) {

  // Set root node
  _root = root;

}

/*--------------------------------------------------------------------------*/
