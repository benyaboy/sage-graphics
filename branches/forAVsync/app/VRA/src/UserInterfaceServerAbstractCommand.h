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

#ifndef USER_INTERFACE_SERVER_ABSTRACT_COMMAND_H
#define USER_INTERFACE_SERVER_ABSTRACT_COMMAND_H

/*--------------------------------------------------------------------------*/

class UserInterfaceServerAbstractCommand {

public:

  // Destructor
  virtual ~UserInterfaceServerAbstractCommand();

  // Get axis state
  virtual void GetAxis(bool* state);

  // Get axis position
  virtual void GetAxisPosition(float* x, float* y, float* z);

  // Get bounding box state
  virtual void GetBoundingBox(bool* state);

  // Get brick box state
  virtual void GetBrickBox(bool* state);

  // Get clipping planes
  virtual void GetClippingPlanes(int node, float* near, float* far);

  // Get data
  virtual void GetData(char* filename);

  // Get data and cache sizes
  virtual void GetData(char* filename, int* ramSize, int* vramSize);

  // Get dataset list file
  virtual void GetDatasetListFile(int index, char* file);

  // Get dataset list name
  virtual void GetDatasetListName(int index, char* name);

  // Get dataset list prefix
  virtual void GetDatasetListPrefix(int index, char* prefix);

  // Get dataset list size
  virtual void GetDatasetListSize(int* size);

  // Get dataset type
  virtual void GetDatasetType(int* type);

  // Get display size
  virtual void GetDisplaySize(int node, int* width, int* height,
                              bool* fullScreen,
                              float* left, float* right,
                              float* bottom, float* top);

  // Get frustum
  virtual void GetFrustum(int node, float* left, float* right,
                          float* bottom, float* top);

  // Get map
  virtual void GetMap(unsigned char* map);

  // Get overview vertices
  virtual void GetOverviewVertices(float vertices[24]);

  // Get prefix
  virtual void GetPrefix(char* prefix);

  // Get rotation matrix
  virtual void GetRotationMatrix(float m[16]);
  
  // Get scale matrix
  virtual void GetScaleMatrix(float m[16]);

  // Get slice frequency
  virtual void GetSliceFrequency(double* frequency);

  // Get total display dimensions
  virtual void GetTotalDisplayDimensions(int* w, int* h);

  // Get total display frustum
  virtual void GetTotalDisplayFrustum(float frustum[6]);

  // Get translation matrix
  virtual void GetTranslationMatrix(float m[16]);

  // Get vertical field of view
  virtual void GetVerticalFieldOfView(float* vFOV);

  // Get viewport
  virtual void GetViewport(int node, int* x, int* y, int* w, int* h);

  // Reset view
  virtual void ResetView();

  // Set axis off
  virtual void SetAxisOff();

  // Set axis on
  virtual void SetAxisOn();

  // Set axis position
  virtual void SetAxisPosition(float x, float y, float z);

  // Set bounding box off
  virtual void SetBoundingBoxOff();

  // Set bounding box on
  virtual void SetBoundingBoxOn();

  // Set brick box off
  virtual void SetBrickBoxOff();

  // Set brick box on
  virtual void SetBrickBoxOn();

  // Set clipping planes
  virtual void SetClippingPlanes(int node, float near, float far);

  // Set exit flag
  virtual void SetCommandExit();

  // Set render flag
  virtual void SetCommandRender();

  // Set data
  virtual void SetData(char* filename);

  // Set data and cache sizes
  virtual void SetData(char* filename, int ramSize, int vramSize);

  // Set frustum
  virtual void SetFrustum(int node, float left, float right,
                          float bottom, float top);

  // Set transfer function
  virtual void SetMap(unsigned char* map);

  // Set 16-bit transfer function
  virtual void SetMap16(unsigned char* map);
  
  // Set prefix
  virtual void SetPrefix(char* prefix);

  // Set rotation matrix
  virtual void SetRotationMatrix(float m[16]);

  // Set scale matrix
  virtual void SetScaleMatrix(float m[16]);

  // Set slice frequency
  virtual void SetSliceFrequency(double frequency);

  // Set translation matrix
  virtual void SetTranslationMatrix(float m[16]);

  // Set viewport
  virtual void SetViewport(int node, int x, int y, int w, int h);

protected:

  // Constructor
  UserInterfaceServerAbstractCommand();

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
