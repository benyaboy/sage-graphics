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

#include "CommunicatorConcreteCommand.h"
#include "Slave.h"

/*--------------------------------------------------------------------------*/

CommunicatorConcreteCommand::CommunicatorConcreteCommand() {

  _slave = NULL;

}

/*--------------------------------------------------------------------------*/

CommunicatorConcreteCommand::CommunicatorConcreteCommand(void* slave) {

  _slave = slave;

}

/*--------------------------------------------------------------------------*/

CommunicatorConcreteCommand::~CommunicatorConcreteCommand() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetAxisOff() {

  ((Slave*) _slave) -> UpdateAxisOff();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetAxisOn() {

  ((Slave*) _slave) -> UpdateAxisOn();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetAxisPosition(float x, float y, float z) {

  ((Slave*) _slave) -> UpdateAxisPosition(x, y, z);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetBrickBoxOff() {

  ((Slave*) _slave) -> UpdateBrickBoxOff();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetBrickBoxOn() {

  ((Slave*) _slave) -> UpdateBrickBoxOn();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetBoundingBoxOff() {

  ((Slave*) _slave) -> UpdateBoundingBoxOff();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetBoundingBoxOn() {

  ((Slave*) _slave) -> UpdateBoundingBoxOn();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetClippingPlanes(float near, float far) {

  ((Slave*) _slave) -> UpdateClippingPlanes(near, far);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetCommandExit() {

  ((Slave*) _slave) -> UpdateCommandExit();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetCommandRender() {

  ((Slave*) _slave) -> UpdateCommandRender();

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetData(char* filename) {

  ((Slave*) _slave) -> UpdateData(filename);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetData(char* filename,
                                          int ramSize, int vramSize) {

  ((Slave*) _slave) -> UpdateData(filename, ramSize, vramSize);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetDisplaySize(int width, 
                                                 int height, 
                                                 bool fullScreen,
                                                 char* hostname,
                                                 int port,
                                                 float left,
                                                 float right,
                                                 float bottom,
                                                 float top) {

  ((Slave*) _slave) -> 
    UpdateDisplaySize(width, height, fullScreen, hostname, port, 
                      left, right, bottom, top);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetFrustum(float left, float right,
                                             float bottom, float top) {

  ((Slave*) _slave) -> UpdateFrustum(left, right, bottom, top);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetMap(unsigned char* map) {

  ((Slave*) _slave) -> UpdateMap(map);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetMap16(unsigned char* map) {

  ((Slave*) _slave) -> UpdateMap16(map);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetPrefix(char* prefix) {

  ((Slave*) _slave) -> UpdatePrefix(prefix);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetRotationMatrix(float matrix[16]) {

  ((Slave*) _slave) -> UpdateRotationMatrix(matrix);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetScaleMatrix(float matrix[16]) {

  ((Slave*) _slave) -> UpdateScaleMatrix(matrix);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetSliceFrequency(double frequency) {

  ((Slave*) _slave) -> UpdateSliceFrequency(frequency);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetTranslationMatrix(float matrix[16]) {

  ((Slave*) _slave) -> UpdateTranslationMatrix(matrix);

}

/*--------------------------------------------------------------------------*/

void CommunicatorConcreteCommand::SetViewport(int x, int y, int w, int h) {

  ((Slave*) _slave) -> UpdateViewport(x, y, w, h);

}

/*--------------------------------------------------------------------------*/
