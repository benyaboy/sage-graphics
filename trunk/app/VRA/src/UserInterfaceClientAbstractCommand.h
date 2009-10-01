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

#ifndef USER_INTERFACE_ABSTRACT_COMMAND_H
#define USER_INTERFACE_ABSTRACT_COMMAND_H

/*--------------------------------------------------------------------------*/

class UserInterfaceClientAbstractCommand {

public:

  // Destructor
  virtual ~UserInterfaceClientAbstractCommand();

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

  // Set data set filename
  virtual void SetData(char* filename);

  // Set data set filename and cache sizes
  virtual void SetData(char* filename, int ramSize, int vramSize);

  // Set data set list file
  virtual void SetDatasetListFile(int index, char* file);

  // Set data set list name
  virtual void SetDatasetListName(int index, char* name);

  // Set data set list prefix
  virtual void SetDatasetListPrefix(int index, char* prefix);

  // Set data set list size
  virtual void SetDatasetListSize(int size);

  // Set data set type
  virtual void SetDatasetType(int type);

  // Set display size
  virtual void SetDisplaySize(int node, int width, int height, 
                              bool fullScreen, 
                              float left, float right, 
                              float bottom, float top);

  // Set frustum
  virtual void SetFrustum(int node, float left, float right,
                          float bottom, float top);

  // Set transfer function
  virtual void SetMap(unsigned char* map);

  // Set overview vertices
  virtual void SetOverviewVertices(float vertices[24]);

  // Set data prefix
  virtual void SetPrefix(char* prefix);

  // Set progress
  virtual void SetProgress(int value, int total);

  // Set rotation matrix
  virtual void SetRotationMatrix(float m[16]);

  // Set scale matrix
  virtual void SetScaleMatrix(float m[16]);

  // Set slice frequency
  virtual void SetSliceFrequency(double frequency);

  // Set total display dimensions
  virtual void SetTotalDisplayDimensions(int w, int h);

  // Set total display frustum
  virtual void SetTotalDisplayFrustum(float frustum[6]);

  // Set translation matrix
  virtual void SetTranslationMatrix(float m[16]);

  // Set vertical field of view
  virtual void SetVerticalFieldOfView(float vFOV);

  // Set viewport
  virtual void SetViewport(int node, int x, int y, int w, int h);

protected:

  // Constructor
  UserInterfaceClientAbstractCommand();

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
