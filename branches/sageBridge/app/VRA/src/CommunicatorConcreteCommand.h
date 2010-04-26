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

#ifndef COMMUNICATOR_CONCRETE_COMMAND_H
#define COMMUNICATOR_CONCRETE_COMMAND_H

/*--------------------------------------------------------------------------*/

#include <stdlib.h>

#include "CommunicatorAbstractCommand.h"

/*--------------------------------------------------------------------------*/

class CommunicatorConcreteCommand : public CommunicatorAbstractCommand {
  
public:

  // Constructor
  CommunicatorConcreteCommand();

  // Constructor with pointer to instance of class Slave
  CommunicatorConcreteCommand(void* slave);

  // Destructor
  virtual ~CommunicatorConcreteCommand();

  // Set axis off
  virtual void SetAxisOff();

  // Set axis on
  virtual void SetAxisOn();

  // Set axis position
  virtual void SetAxisPosition(float x, float y, float z);

  // Set brick box off
  virtual void SetBrickBoxOff();

  // Set brick box on
  virtual void SetBrickBoxOn();

  // Set bounding box off
  virtual void SetBoundingBoxOff();

  // Set bounding box on
  virtual void SetBoundingBoxOn();

  // Set clipping planes
  virtual void SetClippingPlanes(float near, float far);

  // Set exit flag
  virtual void SetCommandExit();

  // Set render flag
  virtual void SetCommandRender();

  // Set data set filename
  virtual void SetData(char* filename);

  // Set data set filename and cache sizes
  virtual void SetData(char* filename, int ramSize, int vramSize);

  // Set display size
  virtual void SetDisplaySize(int width, int height, bool fullScreen,
                              char* hostname, int port,
                              float left, float right,
                              float bottom, float top);

  // Set frustum
  virtual void SetFrustum(float left, float right,
                          float bottom, float top);

  // Set 8-bit transfer function
  virtual void SetMap(unsigned char* map);

  // Set 16-bit transfer function
  virtual void SetMap16(unsigned char* map);

  // Set data set prefix
  virtual void SetPrefix(char* prefix);

  // Set rotation matrix
  virtual void SetRotationMatrix(float matrix[16]);

  // Set scale matrix
  virtual void SetScaleMatrix(float matrix[16]);

  // Set slice frequency
  virtual void SetSliceFrequency(double frequency);

  // Set translation matrix
  virtual void SetTranslationMatrix(float matrix[16]);

  // Set viewport
  virtual void SetViewport(int x, int y, int w, int h);

private:

  // Pointer to instance of class slave
  void* _slave;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
