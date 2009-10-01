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

#include "OctreeNode.h"

/*--------------------------------------------------------------------------*/

OctreeNode::OctreeNode() {

  // Message
  //Assert("Constructor begin.");

  // Initialize children
  _child[0] = NULL;
  _child[1] = NULL;
  _child[2] = NULL;
  _child[3] = NULL;
  _child[4] = NULL;
  _child[5] = NULL;
  _child[6] = NULL;
  _child[7] = NULL;

  // Initialize data
  _data = NULL;
  _dataLoadedFlag = false;
  _dataFormat = OCTREE_NODE_BIG_ENDIAN;
  _dataType = OCTREE_NODE_UNSIGNED_8_INT;

  // Initialize node id
  _id = 0;

  // Initialize node level
  _level = 0;

  // Initialize data prefix
  memset(_prefix, 0, 1024);

  // Initialize texture
  _texture = 0;
  _textureLoadedFlag = false;

  // Initialize data dimensions
  _whd[0] = 0;
  _whd[1] = 0;
  _whd[2] = 0;

  // Message
  //Assert("Constructor end.");

}

/*--------------------------------------------------------------------------*/

OctreeNode::~OctreeNode() {

  // Message
  Assert("Destructor begin.");

  // Clean up children (and octree) recursively
  for (int i = 0 ; i < 8 ; i++) {
    if (_child[i] != NULL) {
      delete _child[i];
    }
  }

  // Delete texture
  UnloadTexture();

  // Clean up data
  if (_data != NULL) {
    delete [] _data;
  }

  // Message
  Assert("Destructor end.");

}

/*--------------------------------------------------------------------------*/

void OctreeNode::Assert(const char* message) {

#ifdef LOG

  // Print message and time to stdout
  char buffer[1024];
  sprintf(buffer, "OctreeNode: %s", message);
  Log::Assert(buffer);

#endif

}

/*--------------------------------------------------------------------------*/

OctreeNode* OctreeNode::GetChild(int index) {

  // Test index and return child
  if (index >= 0 && index <= 7) {
    return _child[index];
  }
  else {
    return NULL;
  }

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetD() {

  // Return depth
  return _whd[2];

}

/*--------------------------------------------------------------------------*/

unsigned char* OctreeNode::GetData() {

  // Return data
  return _data;

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetDataFormat() {

  // Return data format
  return _dataFormat;

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetDataType() {

  // Return data type
  return _dataType;

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetH() {

  // Return height
  return _whd[1];

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetID() {

  // Return node id
  return _id;

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetLevel() {

  // Return node level
  return _level;

}

/*--------------------------------------------------------------------------*/

GLuint OctreeNode::GetTexture() {

  // Return texture id
  return _texture;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::GetVertex(int index, Vertex* vertex) {

  // Test index and return vertex
  if (index >= 0 && index <= 7) {
    *vertex = _vertex[index];
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNode::GetVertex(int index, float* x, float* y, float *z) {

  // Test index and return vertex
  if (index >= 0 && index <= 7) {
    *x = _vertex[index].GetX();
    *y = _vertex[index].GetY();
    *z = _vertex[index].GetZ();
  }

}

/*--------------------------------------------------------------------------*/

int OctreeNode::GetW() {

  // Return width
  return _whd[0];

}

/*--------------------------------------------------------------------------*/

void OctreeNode::GetWHD(int whd[3]) {

  // Return dimensions
  whd[0] = _whd[0];
  whd[1] = _whd[1];
  whd[2] = _whd[2];

}

/*--------------------------------------------------------------------------*/

void OctreeNode::GetWHD(int* w, int* h, int* d) {

  // Return dimensions
  *w = _whd[0];
  *h = _whd[1];
  *d = _whd[2];

}

/*--------------------------------------------------------------------------*/

bool OctreeNode::IsDataLoaded() {

  // Return data loaded flag
  return _dataLoadedFlag;

}

/*--------------------------------------------------------------------------*/

bool OctreeNode::IsTextureLoaded() {

  // Return texture loaded flag
  return _textureLoadedFlag;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::LoadData() {

  // Message
  char message[1024];
  sprintf(message, "LoadData node %d begin.", _id);
  Assert(message);

  // Create full path and filename of file to load
  char path[2048];
  memset(path, 0, sizeof(char) * 2048);
  memcpy(path, _prefix, sizeof(char) * strlen(_prefix));
  memcpy(&path[strlen(path)], _filename, sizeof(char) * strlen(_filename));

  // open raw data file
  FILE* datafptr;
  if ((datafptr = fopen(path, "r")) == NULL) {
    fprintf(stderr, 
            "OctreeNode: Error reading data file %s.\n", 
            path);
    return;
  }
  
  // raw data
  unsigned char* data = NULL;

  // Load unsigned 8bit int
  if (_dataType == OCTREE_NODE_UNSIGNED_8_INT) {

    // allocate memory for raw data
    if ((data = new unsigned char[(int)(_whd[0]* _whd[1] * _whd[2])]) == 
        NULL) {
      fprintf(stderr, "OctreeNode: Memory allocation error\n.");
      return;
    }

    // read raw data
    if (fread(data, sizeof(unsigned char), 
              _whd[0] * _whd[1] * _whd[2], datafptr) != 
        _whd[0] * _whd[1] * _whd[2]) {
      fprintf(stderr, 
              "OctreeNode: Error reading data from file %s.\n", _filename);

    }

  }

  // Load unsigned 16bit int
  else if (_dataType == OCTREE_NODE_UNSIGNED_16_INT) {

    // allocate memory for raw data
    if ((data = new unsigned char[(int)(_whd[0] * _whd[1] * _whd[2] * 2)]) == 
        NULL) {
      fprintf(stderr, "OctreeNode: Memory allocation error\n.");
      return;
    }
    
    // read raw data
    if (fread(data, sizeof(unsigned char), 
              _whd[0] * _whd[1] * _whd[2] * 2, datafptr) != 
        _whd[0] * _whd[1] * _whd[2] * 2) {
      fprintf(stderr, 
              "OctreeNode: Error reading data from file %s.\n", _filename);
    }

    // swap bytes
    if (_dataFormat == OCTREE_NODE_LITTLE_ENDIAN) {
      for (int i = 0 ; i < _whd[0] * _whd[1] * _whd[2] * 2 ; i += 2) {
        unsigned char temp = 0;
        temp = data[i];
        data[i] = data[i+1];
        data[i+1] = temp;
      }
    }

  }

  // close raw data file
  fclose(datafptr);

  // assign data
  _data = data;

  // set data loaded flag
  _dataLoadedFlag = true;

  // Message
  sprintf(message, "LoadData node %d end.", _id);
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void OctreeNode::LoadFromFile(const char* filename) {

  // Message
  Assert("LoadMetaData begin.");

  // raw data file name
  char datafilename[128];

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

  // level
  int level;

  // id
  int id;

  // number of children
  int numberofchildren;

  // child file name
  char childfilename[128];

  // Create full path and filename of file to load
  char path[2048];
  memset(path, 0, sizeof(char) * 2048);
  memcpy(path, _prefix, sizeof(char) * strlen(_prefix));
  memcpy(&path[strlen(path)], filename, sizeof(char) * strlen(filename));

  // open metadata file
  FILE* metafptr;
  if ((metafptr = fopen(path, "r")) == NULL) {
    fprintf(stderr, 
            "OctreeNode: Error reading metadata file %s.\n", 
            path);
    return;
  }

  // read raw data filename
  fscanf(metafptr, "%s", datafilename);

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

  // read level, parent id, id
  fscanf(metafptr, "%d %d", &level, &id);

  // read children flag
  fscanf(metafptr, "%d", &numberofchildren);

  // assign data filename
  strcpy(_filename, datafilename);

  // assign vertices
  _vertex[0].SetXYZ(v0x, v0y, v0z);
  _vertex[1].SetXYZ(v1x, v1y, v1z);
  _vertex[2].SetXYZ(v2x, v2y, v2z);
  _vertex[3].SetXYZ(v3x, v3y, v3z);
  _vertex[4].SetXYZ(v4x, v4y, v4z);
  _vertex[5].SetXYZ(v5x, v5y, v5z);
  _vertex[6].SetXYZ(v6x, v6y, v6z);
  _vertex[7].SetXYZ(v7x, v7y, v7z);

  // assign width, height, depth
  _whd[0] = w;
  _whd[1] = h;
  _whd[2] = d;

  // assign data type
  if (!strcmp(dataTypeString, "UNSIGNED_8_INT")) {
    _dataType = OCTREE_NODE_UNSIGNED_8_INT;
  }
  else if (!strcmp(dataTypeString, "UNSIGNED_16_INT")) {
    _dataType = OCTREE_NODE_UNSIGNED_16_INT;
  }
  else {
    fprintf(stderr, "OctreeNode: Unknown data type.\n");
    _dataType = OCTREE_NODE_UNSIGNED_8_INT;
  }

  // assign data format
  if (!strcmp(dataFormatString, "BIG_ENDIAN")) {
    _dataFormat = OCTREE_NODE_BIG_ENDIAN;
  }
  else if (!strcmp(dataFormatString, "LITTLE_ENDIAN")) {
    _dataFormat = OCTREE_NODE_LITTLE_ENDIAN;
  }
  else {
    fprintf(stderr, "OctreeNode: Unknown data format.\n");
    _dataFormat = OCTREE_NODE_BIG_ENDIAN;
  }

  // assign level
  _level = level;

  // assign id
  _id = id;

  // read children
  for (int i = 0 ; i < numberofchildren ; i++) {
    fscanf(metafptr, "%s", childfilename);
    if ((_child[i] = new OctreeNode) == NULL) {
      fprintf(stderr, "OctreeNode: Memory allocation error.\n");
    }

    // Create full path and filename of file to load
    char childpath[2048];
    memset(childpath, 0, sizeof(char) * 2048);
    memcpy(childpath, _prefix, sizeof(char) * strlen(_prefix));
    memcpy(&path[strlen(path)], childfilename, 
           sizeof(char) * strlen(childfilename));

    // Load child
    _child[i] -> SetPrefix(_prefix);
    _child[i] -> LoadFromFile(childfilename);
  }

  // close meta data file
  fclose(metafptr);

  // Message
  Assert("LoadMetaData end.");

}

/*--------------------------------------------------------------------------*/

void OctreeNode::LoadTexture() {

  // Load unsigned 8 int
  if (_dataType == OCTREE_NODE_UNSIGNED_8_INT) {
    LoadTexture8();
  }

  // Load unsigned 16 int
  else if (_dataType == OCTREE_NODE_UNSIGNED_16_INT) {
    LoadTexture16();
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNode::LoadTexture8() {

  // Message
  char message[1024];
  sprintf(message, "LoadTexture node %d begin.", _id);
  Assert(message);

  // Don't load if it's already loaded
  if (_textureLoadedFlag == true) {
    return;
  }

  // Enable 3D textures
  glEnable(GL_TEXTURE_3D);

  // Generate texture
  glGenTextures(1, &_texture);

  // Bind texture
  glBindTexture(GL_TEXTURE_3D, _texture);

  // Set clamping and interpolation parameters
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_WRAP_S, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_WRAP_T, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_WRAP_R, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_MAG_FILTER, 
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_MIN_FILTER, 
                  GL_LINEAR);
  
  // Load proxy texture
  glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_ALPHA,  
               GetW(), 
               GetH(), 
               GetD(),
               0, GL_ALPHA, GL_UNSIGNED_BYTE, 
               NULL);

  // Get proxy texture width, height and depth
  GLint width, height, depth;
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, 
                           GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, 
                           GL_TEXTURE_HEIGHT, &height);
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, 
                           GL_TEXTURE_DEPTH, &depth);

  // Test if there's enough resources for the desired texture
  if (width == 0 || height == 0 || depth == 0) {
    fprintf(stderr, "VolumeRenderer: Error loading texture.\n");
    glDeleteTextures(1, &_texture);
    fprintf(stderr, "Not loading data.\n");
    return;
  }

  // Load texture
  glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA, 
               GetW(),
               GetH(),
               GetD(),
               0, GL_ALPHA, GL_UNSIGNED_BYTE,
               GetData());

  // Set texture loaded flag
  _textureLoadedFlag = true;

  // Disable 3D textures
  glDisable(GL_TEXTURE_3D);

  // Message
  sprintf(message, "LoadTexture node %d end.", _id);
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void OctreeNode::LoadTexture16() {

  // Message
  char message[1024];
  sprintf(message, "LoadTexture node %d begin.", _id);
  Assert(message);

  // Don't load if it's already loaded
  if (_textureLoadedFlag == true) {
    return;
  }

  // Enable 3D textures
  glEnable(GL_TEXTURE_3D);

  // Generate texture
  glGenTextures(1, &_texture);

  // Bind texture
  glBindTexture(GL_TEXTURE_3D, _texture);

  // Set clamping and interpolation parameters
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_WRAP_S, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_WRAP_T, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_WRAP_R, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_MAG_FILTER, 
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, 
                  GL_TEXTURE_MIN_FILTER, 
                  GL_LINEAR);
  
  // Load proxy texture
  glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA,  
               GetW(), 
               GetH(), 
               GetD(),
               0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
               NULL);

  // Get proxy texture width, height and depth
  GLint width, height, depth;
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, 
                           GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, 
                           GL_TEXTURE_HEIGHT, &height);
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, 
                           GL_TEXTURE_DEPTH, &depth);

  // Test if there's enough resources for the desired texture
  if (width == 0 || height == 0 || depth == 0) {
    fprintf(stderr, "VolumeRenderer: Error loading texture.\n");
    glDeleteTextures(1, &_texture);
    fprintf(stderr, "Not loading data.\n");
    return;
  }

  // Load texture
  glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, 
               GetW(),
               GetH(),
               GetD(),
               0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
               GetData());

  // Set texture loaded flag
  _textureLoadedFlag = true;

  // Disable 3D textures
  glDisable(GL_TEXTURE_3D);

  // Message
  sprintf(message, "LoadTexture node %d end.", _id);
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void OctreeNode::Print() {

  // Message
  char message[1024];
  Assert("Printing OctreeNode begin.");

  // Print node id
  sprintf(message, 
          "  Node ID: %d ", 
          GetID());
  Assert(message);

  // Print node level
  sprintf(message, 
          "    Node Level: %d ", 
          GetLevel());
  Assert(message);

  // Print node w, h, d
  sprintf(message, 
          "    W: %d", 
          GetW());
  Assert(message);
  sprintf(message, 
          "    H: %d", 
          GetH());
  Assert(message);
  sprintf(message, 
          "    D: %d", 
          GetD());
  Assert(message);

  // Print data loaded flag
  if (IsDataLoaded() == true) {
    sprintf(message, 
            "    Data Loaded: true");
  }
  else {
    sprintf(message, 
            "    Data Loaded: false");
  }
  Assert(message);

  // Print texture loaded flag
  if (IsTextureLoaded() == true) {
    sprintf(message, 
            "    Texture Loaded: true");
  }
  else {
    sprintf(message, 
            "    Texture Loaded: false");
  }
  Assert(message);
  
  // Message
  Assert("Printing OctreeNode end.");

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetChild(int index, OctreeNode* child) {

  // Test index and set child node
  if (index >= 0 && index <= 7) {
    _child[index] = child;
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetD(int d) {

  // Set depth
  _whd[2] = d;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetData(unsigned char* data) {

  // Set data
  _data = data;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetDataFormat(int format) {

  // Set data format
  _dataFormat = format;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetDataType(int type) {

  // Set data type
  _dataType = type;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetFilename(const char* filename) {

  // Update local variables
  if (filename != NULL) {
    strcpy(_filename, filename);
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetH(int h) {

  // Set height
  _whd[1] = h;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetID(int id) {

  // Set node id
  _id = id;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetLevel(int level) {

  // Set node level
  _level = level;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetPrefix(const char* prefix) {

  // Update local variables
  if (prefix != NULL) {
    strcpy(_prefix, prefix);
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetVertex(int index, Vertex vertex) {
    
  // Test index and set vertex
  if (index >= 0 && index <= 7) {
    _vertex[index] = vertex;
  }

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetVertex(int index, float x, float y, float z) {

  // Test index and set vertex
  if (index >= 0 && index <= 7) {
    _vertex[index].SetXYZ(x, y, z);
  }
    
}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetW(int w) {

  // Set width
  _whd[0] = w;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetWHD(int whd[3]) {

  // Set dimensions
  _whd[0] = whd[0];
  _whd[1] = whd[1];
  _whd[2] = whd[2];

}

/*--------------------------------------------------------------------------*/

void OctreeNode::SetWHD(int w, int h, int d) {

  // Set dimension
  _whd[0] = w;
  _whd[1] = h;
  _whd[2] = d;

}

/*--------------------------------------------------------------------------*/

void OctreeNode::UnloadData() {

  // Message
  char message[1024];
  sprintf(message, "UnloadData node %d begin.", _id);
  Assert(message);

  // Clean up data
  if (_dataLoadedFlag == true) {
    if (_data != NULL) {
      delete [] _data;
      _data = NULL;
    }
    _dataLoadedFlag = false;
  }

  // Message
  sprintf(message, "UnloadData node %d end.", _id);
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void OctreeNode::UnloadTexture() {

  // Message
  char message[1024];
  sprintf(message, "UnloadTexture node %d begin.", _id);
  Assert(message);

  // Delete texture
  if (_textureLoadedFlag == true) {
    glDeleteTextures(1, &_texture);
    _textureLoadedFlag = false;
  }

  // Message
  sprintf(message, "UnloadTexture node %d end.", _id);
  Assert(message);

}

/*--------------------------------------------------------------------------*/
