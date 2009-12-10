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

#ifndef OCTREE_NODE_H
#define OCTREE_NODE_H

/*--------------------------------------------------------------------------*/

#include <GL/gl.h>
#include <string.h>

#include "Log.h"
#include "Vertex.h"

/*--------------------------------------------------------------------------*/

// Data type definitions
#define OCTREE_NODE_UNSIGNED_8_INT 0
#define OCTREE_NODE_UNSIGNED_16_INT 1

// Data format definitions
#define OCTREE_NODE_BIG_ENDIAN 0
#define OCTREE_NODE_LITTLE_ENDIAN 1

/*--------------------------------------------------------------------------*/

extern "C" {
  extern void glTexImage3D(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei,
                           GLint, GLenum, GLenum, const GLvoid*);
}

/*--------------------------------------------------------------------------*/

class OctreeNode {

 public:

  // Default constructor
  OctreeNode();

  // Default destructor
  ~OctreeNode();

  // Get child node
  OctreeNode* GetChild(int index);

  // Get depth
  int GetD();

  // Get data
  unsigned char* GetData();

  // Get data format
  int GetDataFormat();

  // Get data type
  int GetDataType();

  // Get height
  int GetH();

  // Get id
  int GetID();

  // Get level id
  int GetLevel();

  // Get texture
  GLuint GetTexture();

  // Get vertex
  void GetVertex(int index, Vertex* v);

  // Get vertex
  void GetVertex(int index, float* x, float* y, float *z);

  // Get width
  int GetW();

  // Get width, height, depth
  void GetWHD(int whd[3]);

  // Get width, height, depth
  void GetWHD(int* w, int* h, int* d);

  // Is data loaded in RAM
  bool IsDataLoaded();

  // Is texture loaded in VRAM
  bool IsTextureLoaded();

  // Load recursively from file
  void LoadFromFile(const char* filename);

  // Load data to RAM
  void LoadData();

  // Load texture to VRAM
  void LoadTexture();
  
  // Print
  void Print();

  // Set child node
  void SetChild(int index, OctreeNode* child);

  // Set depth
  void SetD(int d);

  // Set data
  void SetData(unsigned char* data);

  // Set data format
  void SetDataFormat(int format);

  // Set data type
  void SetDataType(int type);

  // Set filename
  void SetFilename(const char* filename);

  // Set height
  void SetH(int h);

  // Set id
  void SetID(int id);

  // Set level id
  void SetLevel(int level);

  // Set prefix
  void SetPrefix(const char* prefix);

  // Set vertex
  void SetVertex(int index, Vertex v);

  // Set vertex
  void SetVertex(int index, float x, float y, float z);

  // Set width
  void SetW(int w);

  // Set width, height, depth
  void SetWHD(int whd[3]);

  // Set width, height, depth
  void SetWHD(int w, int h, int d);

  // Unload data from RAM
  void UnloadData();

  // Unload texture from VRAM
  void UnloadTexture();

 private:

  // Print a message to stdout
  void Assert(const char* message);

  // Load unsigned 8 int texture to VRAM
  void LoadTexture8();

  // Load unsigned 16 int texture to VRAM
  void LoadTexture16();

  // Children
  OctreeNode* _child[8];

  // Data
  unsigned char* _data;

  // Data loaded flag
  bool _dataLoadedFlag;

  // Data type
  int _dataType;

  // Data format
  int _dataFormat;

  // Filename
  char _filename[128];

  // ID
  int _id;

  // Level ID
  int _level;

  // Prefix
  char _prefix[1024];

  // Texture
  GLuint _texture;

  // Texture loaded flag
  bool _textureLoadedFlag;

  // Vertices
  Vertex _vertex[8];

  // Width, height, depth
  int _whd[3];

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
