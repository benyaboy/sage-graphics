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

#include "UserInterfaceServerConcreteCommand.h"
#include "Master.h"

/*--------------------------------------------------------------------------*/

UserInterfaceServerConcreteCommand::UserInterfaceServerConcreteCommand() {

  _master = NULL;

}

/*--------------------------------------------------------------------------*/

UserInterfaceServerConcreteCommand::UserInterfaceServerConcreteCommand(void* 
                                                                       master) {

  _master = master;

}

/*--------------------------------------------------------------------------*/

UserInterfaceServerConcreteCommand::~UserInterfaceServerConcreteCommand() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetAxis(bool* state) {

  ((Master*) _master) -> GetAxis(state);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetAxisPosition(float* x,
                                                         float* y,
                                                         float* z) {

  ((Master*) _master) -> GetAxisPosition(x, y, z);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetBoundingBox(bool* state) {

  ((Master*) _master) -> GetBoundingBox(state);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetBrickBox(bool* state) {

  ((Master*) _master) -> GetBrickBox(state);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetClippingPlanes(int node, 
                                                           float* near, 
                                                           float* far) {

  ((Master*) _master) -> GetClippingPlanes(node, near, far);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetData(char* filename) {

  ((Master*) _master) -> GetData(filename);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetData(char* filename,
                                                 int* ramSize, int* vramSize) {

  ((Master*) _master) -> GetData(filename, ramSize, vramSize);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetDatasetListFile(int index, 
                                                            char* file) {

  ((Master*) _master) -> GetDatasetListFile(index, file);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetDatasetListName(int index, 
                                                            char* name) {

  ((Master*) _master) -> GetDatasetListName(index, name);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetDatasetListPrefix(int index, 
                                                              char* prefix) {

  ((Master*) _master) -> GetDatasetListPrefix(index, prefix);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetDatasetListSize(int* size) {

  ((Master*) _master) -> GetDatasetListSize(size);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetDatasetType(int* type) {

  ((Master*) _master) -> GetDatasetType(type);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetDisplaySize(int node, 
                                                        int* width, 
                                                        int* height,
                                                        bool* fullScreen,
                                                        float* left, 
                                                        float* right,
                                                        float* bottom, 
                                                        float* top) {

  ((Master*) _master) -> GetDisplaySize(node, width, height, fullScreen,
                                        left, right, bottom, top);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetFrustum(int node, 
                                                    float* left, 
                                                    float* right,
                                                    float* bottom, 
                                                    float* top) {

  ((Master*) _master) -> GetFrustum(node, left, right, bottom, top);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetMap(unsigned char* map) {

  ((Master*) _master) -> GetMap(map);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetOverviewVertices(float 
                                                             vertices[24]) {

  ((Master*) _master) -> GetOverviewVertices(vertices);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetPrefix(char* prefix) {

  ((Master*) _master) -> GetPrefix(prefix);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetRotationMatrix(float m[16]) {

  ((Master*) _master) -> GetRotationMatrix(m);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetScaleMatrix(float m[16]) {

  ((Master*) _master) -> GetScaleMatrix(m);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetSliceFrequency(double* frequency) {

  ((Master*) _master) -> GetSliceFrequency(frequency);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetTotalDisplayDimensions(int* w,
                                                                   int* h) {

  ((Master*) _master) -> GetTotalDisplayDimensions(w, h);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetTotalDisplayFrustum(float
                                                                frustum[6]) {

  ((Master*) _master) -> GetTotalDisplayFrustum(frustum);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetTranslationMatrix(float m[16]) {

  ((Master*) _master) -> GetTranslationMatrix(m);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetVerticalFieldOfView(float* vFOV) {

  ((Master*) _master) -> GetVerticalFieldOfView(vFOV);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::GetViewport(int node, 
                                                     int* x, 
                                                     int* y, 
                                                     int* w, 
                                                     int* h) {

  ((Master*) _master) -> GetViewport(node, x, y, w, h);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::ResetView() {

  ((Master*) _master) -> ResetView();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetAxisOff() {

  ((Master*) _master) -> UpdateAxisOff();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetAxisOn() {

  ((Master*) _master) -> UpdateAxisOn();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetAxisPosition(float x, 
                                                         float y, 
                                                         float z) {

  ((Master*) _master) -> UpdateAxisPosition(x, y, z);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetBoundingBoxOff() {

  ((Master*) _master) -> UpdateBoundingBoxOff();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetBoundingBoxOn() {

  ((Master*) _master) -> UpdateBoundingBoxOn();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetBrickBoxOff() {

  ((Master*) _master) -> UpdateBrickBoxOff();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetBrickBoxOn() {

  ((Master*) _master) -> UpdateBrickBoxOn();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetClippingPlanes(int node, 
                                                           float near, 
                                                           float far) {

  ((Master*) _master) -> UpdateClippingPlanes(node, near, far);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetCommandExit() {

  ((Master*) _master) -> UpdateCommandExit();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetCommandRender() {

  ((Master*) _master) -> UpdateCommandRender();

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetData(char* filename) {

  ((Master*) _master) -> UpdateData(filename);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetData(char* filename,
                                                 int ramSize, int vramSize) {

  ((Master*) _master) -> UpdateData(filename, ramSize, vramSize);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetFrustum(int node, 
                                                    float left, float right,
                                                    float bottom, float top) {

  ((Master*) _master) -> UpdateFrustum(node, left, right, bottom, top);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetMap(unsigned char* map) {

  ((Master*) _master) -> UpdateMap(map);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetMap16(unsigned char* map) {

  ((Master*) _master) -> UpdateMap16(map);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetPrefix(char* prefix) {

  ((Master*) _master) -> UpdatePrefix(prefix);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetRotationMatrix(float m[16]) {

  ((Master*) _master) -> UpdateRotationMatrix(m);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetScaleMatrix(float m[16]) {

  ((Master*) _master) -> UpdateScaleMatrix(m);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetSliceFrequency(double frequency) {

  ((Master*) _master) -> UpdateSliceFrequency(frequency);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetTranslationMatrix(float m[16]) {

  ((Master*) _master) -> UpdateTranslationMatrix(m);

}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerConcreteCommand::SetViewport(int node, int x, int y, 
                                                     int w, int h) {

  ((Master*) _master) -> UpdateViewport(node, x, y, w, h);

}

/*--------------------------------------------------------------------------*/
