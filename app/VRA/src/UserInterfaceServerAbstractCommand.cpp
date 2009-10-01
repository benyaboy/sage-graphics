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

#include "UserInterfaceServerAbstractCommand.h"

/*--------------------------------------------------------------------------*/

UserInterfaceServerAbstractCommand::UserInterfaceServerAbstractCommand() {
}

/*--------------------------------------------------------------------------*/

UserInterfaceServerAbstractCommand::~UserInterfaceServerAbstractCommand() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetAxis(bool* state) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetAxisPosition(float* x,
                                                         float* y,
                                                         float* z) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetBoundingBox(bool* state) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetBrickBox(bool* state) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetClippingPlanes(int node, 
                                                           float* near, 
                                                           float* far) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetData(char* filename) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetData(char* filename,
                                                 int* ramSize, int* vramSize) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetDatasetListFile(int index,
                                                            char* file) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetDatasetListName(int index,
                                                            char* name) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetDatasetListPrefix(int index,
                                                              char* prefix) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetDatasetListSize(int* size) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetDatasetType(int* type) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetDisplaySize(int node, 
                                                        int* width, 
                                                        int* height,
                                                        bool* fullScreen,
                                                        float* left, 
                                                        float* right,
                                                        float* bottom, 
                                                        float* top) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetFrustum(int node, 
                                                    float* left, 
                                                    float* right,
                                                    float* bottom, 
                                                    float* top) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetMap(unsigned char* map) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetPrefix(char* prefix) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetOverviewVertices(float 
                                                             vertices[24]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetRotationMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetScaleMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetSliceFrequency(double* frequency) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetTotalDisplayDimensions(int* w, 
                                                                   int* h) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetTotalDisplayFrustum(float 
                                                                frustum[6]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetTranslationMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetVerticalFieldOfView(float* vFOV) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::GetViewport(int node, 
                                                     int* x, 
                                                     int* y, 
                                                     int* w, 
                                                     int* h) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::ResetView() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetAxisOff() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetAxisOn() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetAxisPosition(float x, float y, 
                                                         float z) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetBoundingBoxOff() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetBoundingBoxOn() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetBrickBoxOff() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetBrickBoxOn() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetClippingPlanes(int node, 
                                                           float near, 
                                                           float far) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetCommandExit() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetCommandRender() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetData(char* filename) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetData(char* filename,
                                                 int ramSize, int vramSize) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetFrustum(int node, 
                                                    float left, float right,
                                                    float bottom, float top) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetMap(unsigned char* map) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetMap16(unsigned char* map) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetPrefix(char* prefix) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetRotationMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetScaleMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetSliceFrequency(double frequency) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetTranslationMatrix(float m[16]) {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceServerAbstractCommand::SetViewport(int node, int x, int y, 
                                                     int w, int h) {
}

/*--------------------------------------------------------------------------*/
