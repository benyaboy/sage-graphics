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

#include "UserInterfaceClientConcreteCommand.h"
#include <stdio.h>

/*--------------------------------------------------------------------------*/

UserInterfaceClientConcreteCommand::UserInterfaceClientConcreteCommand() {

  _addMessageCallback = NULL;

}

/*--------------------------------------------------------------------------*/

UserInterfaceClientConcreteCommand::
UserInterfaceClientConcreteCommand(void (*cb)(unsigned char* message)) {

  _addMessageCallback = cb;

}

/*--------------------------------------------------------------------------*/

UserInterfaceClientConcreteCommand::~UserInterfaceClientConcreteCommand() {
}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetAxisOff() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AOF");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetAxisOn() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_AON");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetAxisPosition(float x,
                                                         float y, 
                                                         float z) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_APO");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackFloat(x);
  packer.PackFloat(y);
  packer.PackFloat(z);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetBoundingBoxOff() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BOF");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetBoundingBoxOn() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BON");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetBrickBoxOff() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BBF");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetBrickBoxOn() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_BBN");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetClippingPlanes(int node, 
                                                           float near, 
                                                           float far) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_CLI");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackFloat(near);
  packer.PackFloat(far);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetCommandExit() {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_EXI");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetData(char* filename) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DAT");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(filename[i]);
  }
  packer.PackInt(-1);
  packer.PackInt(-1);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetData(char* filename,
                                                 int ramSize, int vramSize) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DAT");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(filename[i]);
  }
  packer.PackInt(ramSize);
  packer.PackInt(vramSize);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetDatasetListFile(int index, 
                                                            char* file) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DSF");

  // Destination
  int destination = index;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(file[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetDatasetListName(int index, 
                                                            char* name) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DSN");

  // Destination
  int destination = index;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(name[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetDatasetListPrefix(int index, 
                                                              char* prefix) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DSP");

  // Destination
  int destination = index;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(prefix[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetDatasetListSize(int size) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DSS");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackInt(size);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }  

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetDatasetType(int type) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DTP");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackInt(type);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }  

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetDisplaySize(int node, 
                                                        int width, 
                                                        int height, 
                                                        bool fullScreen, 
                                                        float left, 
                                                        float right, 
                                                        float bottom, 
                                                        float top) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_DIS");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Full screen flag
  int flag = 0;
  if (fullScreen == true) {
    flag = 1;
  }

  // Pack data
  packer.PackInt(width);
  packer.PackInt(height);
  packer.PackInt(flag);
  packer.PackFloat(left);
  packer.PackFloat(right);
  packer.PackFloat(bottom);
  packer.PackFloat(top);
      
  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetFrustum(int node, 
                                                    float left, 
                                                    float right,
                                                    float bottom, 
                                                    float top) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_FRU");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackFloat(left);
  packer.PackFloat(right);
  packer.PackFloat(bottom);
  packer.PackFloat(top);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetMap(unsigned char* map) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_MAP");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackUnsignedChar(map[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetOverviewVertices(float 
                                                             vertices[24]) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_OVV");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 24 ; i++) {
    packer.PackFloat(vertices[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetPrefix(char* prefix) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_PRE");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(prefix[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetProgress(int value, int total) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_PRO");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackInt(value);
  packer.PackInt(total);

  //fprintf(stderr, "UICCC level: %d of %d\n", value, total);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetRotationMatrix(float m[16]) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_ROT");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 16 ; i++) {
    packer.PackFloat(m[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetScaleMatrix(float m[16]) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_SCA");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 16 ; i++) {
    packer.PackFloat(m[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetSliceFrequency(double frequency) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_SLF");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackFloat((float) frequency);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetTotalDisplayDimensions(int w, 
                                                                   int h) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_TDD");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackInt(w);
  packer.PackInt(h);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetTotalDisplayFrustum(float 
                                                                frustum[6]) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_TDF");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackFloat(frustum[0]);
  packer.PackFloat(frustum[1]);
  packer.PackFloat(frustum[2]);
  packer.PackFloat(frustum[3]);
  packer.PackFloat(frustum[4]);
  packer.PackFloat(frustum[5]);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetTranslationMatrix(float m[16]) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_TRA");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  for (int i = 0 ; i < 16 ; i++) {
    packer.PackFloat(m[i]);
  }

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetVerticalFieldOfView(float vFOV) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_VFV");

  // Destination
  int destination = 0;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackFloat(vFOV);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/

void UserInterfaceClientConcreteCommand::SetViewport(int node, int x, int y, 
                                                     int w, int h) {

  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);

  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "CMD_VIE");

  // Destination
  int destination = node;

  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }

  // Pack destination
  packer.PackInt(destination);

  // Pack data
  packer.PackInt(x);
  packer.PackInt(y);
  packer.PackInt(w);
  packer.PackInt(h);

  // Add message
  if (_addMessageCallback != NULL) {
    _addMessageCallback(buffer);
  }

}

/*--------------------------------------------------------------------------*/
