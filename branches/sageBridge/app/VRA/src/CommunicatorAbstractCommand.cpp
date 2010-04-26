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

#include "CommunicatorAbstractCommand.h"

/*--------------------------------------------------------------------------*/

CommunicatorAbstractCommand::CommunicatorAbstractCommand() {
}

/*--------------------------------------------------------------------------*/

CommunicatorAbstractCommand::~CommunicatorAbstractCommand() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetAxisOff() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetAxisOn() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetAxisPosition(float x, float y, float z) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetBrickBoxOff() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetBrickBoxOn() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetBoundingBoxOff() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetBoundingBoxOn() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetClippingPlanes(float near, float far) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetCommandExit() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetCommandRender() {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetData(char* filename) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetData(char* filename, 
                                          int ramSize, int vramSize) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetDisplaySize(int width, 
                                                 int height, 
                                                 bool fullScreen, 
                                                 char* hostname,
                                                 int port,
                                                 float left,
                                                 float right,
                                                 float bottom,
                                                 float top) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetFrustum(float left, float right,
                                             float bottom, float top) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetMap(unsigned char* map) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetMap16(unsigned char* map) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetPrefix(char* prefix) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetRotationMatrix(float matrix[16]) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetScaleMatrix(float matrix[16]) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetSliceFrequency(double frequency) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetTranslationMatrix(float matrix[16]) {
}

/*--------------------------------------------------------------------------*/

void CommunicatorAbstractCommand::SetViewport(int x, int y, int w, int h) {
}

/*--------------------------------------------------------------------------*/
