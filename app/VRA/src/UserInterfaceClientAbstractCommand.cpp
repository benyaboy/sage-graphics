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

#include "UserInterfaceClientAbstractCommand.h"

/*--------------------------------------------------------------------------*/

UserInterfaceClientAbstractCommand::UserInterfaceClientAbstractCommand() {
}

/*--------------------------------------------------------------------------*/

UserInterfaceClientAbstractCommand::~UserInterfaceClientAbstractCommand() {
}
/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetAxisOff() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetAxisOn() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetAxisPosition(float x, 
                                                         float y, 
                                                         float z) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetBoundingBoxOff() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetBoundingBoxOn() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetBrickBoxOff() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetBrickBoxOn() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetClippingPlanes(int node, 
                                                           float near, 
                                                           float far) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetCommandExit() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetData(char* filename) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetData(char* filename,
                                                 int ramSize, int vramSize) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetDatasetListFile(int index, 
                                                            char* file) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetDatasetListName(int index, 
                                                            char* name) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetDatasetListPrefix(int index, 
                                                              char* prefix) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetDatasetListSize(int size) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetDatasetType(int type) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetDisplaySize(int node,
                                                        int width, 
                                                        int height, 
                                                        bool fullScreen,
                                                        float left,
                                                        float right,
                                                        float bottom,
                                                        float top) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetFrustum(int node, 
                                                    float left, float right,
                                                    float bottom, float top) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetMap(unsigned char* map) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetOverviewVertices(float 
                                                             vertices[24]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetPrefix(char* prefix) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetProgress(int value, int total) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetRotationMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetScaleMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetSliceFrequency(double frequency) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetTotalDisplayDimensions(int w, 
                                                                   int h) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetTotalDisplayFrustum(float 
                                                                frustum[6]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetTranslationMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetVerticalFieldOfView(float vFOV) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientAbstractCommand::SetViewport(int node, int x, int y, 
                                                     int w, int h) {
}

/*--------------------------------------------------------------------------*/
