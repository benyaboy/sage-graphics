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

#include "Operation.h"

/*--------------------------------------------------------------------------*/

Operation::Operation() {

  // Initialize class name
  memset(_className, 0, 1024);

  // Initialize operation information
  memset(_info, 0, 4096);

  // Initialize log number
  _logNumber = -1;

  // Initialize process number
  _processNumber = -1;

  // Initialize time
  _time = -1;

}

/*--------------------------------------------------------------------------*/

Operation::Operation(char* string) {

  // Initialize class name
  memset(_className, 0, 1024);

  // Initialize operation information
  memset(_info, 0, 4096);

  // Initialize log number
  _logNumber = -1;

  // Initialize process number
  _processNumber = -1;

  // Initialize time
  _time = -1;


  // Position in input string
  int stringPosition = 0;

  // Start of operation found flag
  bool foundStart = false;

  // End of operation found flag
  bool foundEnd = false;

  // Log number as string
  char logNumberString[128];
  memset(logNumberString, 0, 128);

  // Process number as string
  char processNumberString[128];
  memset(processNumberString, 0, 128);

  // Nice text message
  char text[12];
  memset(text, 0, 12);

  // Time as string
  char timeString[128];
  memset(timeString, 0, 128);

  // Class name
  char className[1024];
  memset(className, 0, 1024);

  // Class name
  char info[4096];
  memset(info, 0, 4096);

  //fprintf(stdout, "%s\n", string);

  // Search for beginning of operation message indicated by '['
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c == '[') {
      foundStart = true;
      stringPosition++;
      break;
    }
  }

  // Check that start of operation is found
  if (foundStart == false) {
    fprintf(stderr, "Invalid operation format 0.\n");
    return;
  }

  // Skip spaces
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      break;
    }
  }

  // Get process number
  for (int i = 0 ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      processNumberString[i] = c;
      i++;
    }
    else {
      break;
    }
  }

  // Skip spaces
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      break;
    }
  }

  // Get log number
  for (int i = 0 ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      logNumberString[i] = c;
      i++;
    }
    else {
      break;
    }
  }

  // Skip spaces
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      break;
    }
  }


  // Skip nice text message
  for (int i = 0 ; 
       string[stringPosition] != '\0' && i < 11 ; 
       i++, stringPosition++) {
    text[i] = string[stringPosition];
  }

  // Check that nice message is correct
  if (strcmp("Log - Time:", text)) {
    fprintf(stderr, "Invalid operation format 1.\n");
    return;
  }
  
  // Skip spaces
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      break;
    }
  }

  // Get time
  for (int i = 0 ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ' && c != 'u' && c != 's') {
      timeString[i] = c;
      i++;
    }
    else {
      break;
    }
  }

  // Check for 'u' in 'us'
  if (string[stringPosition++] != 'u') {
    fprintf(stderr, "Invalid operation format 2.\n");
    return;
  }

  // Check for 's' in 'us'
  if (string[stringPosition++] != 's') {
    fprintf(stderr, "Invalid operation format 3.\n");
    return;
  }

  // Skip spaces
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      break;
    }
  }

  // Get class name
  for (int i = 0 ; 
       string[stringPosition] != '\0' && i < 1024 ; 
       i++, stringPosition++) {
    char c = string[stringPosition];
    if (c != ':' && c != ' ') {
      className[i] = c;
    }
    else {
      stringPosition++;
      break;
    }
  }

  // Skip spaces
  for ( ; string[stringPosition] != '\0' ; stringPosition++) {
    char c = string[stringPosition];
    if (c != ' ') {
      break;
    }
  }

  // Get operation info
  for (int i = 0 ; 
       string[stringPosition] != '\0' && i < 4096 ; 
       i++, stringPosition++) {
    char c = string[stringPosition];
    if (c != ']') {
      info[i] = c;
    }
    else {
      foundEnd = true;
      break;
    }
  }

  // Check that end of operation is found
  if (foundEnd == false) {
    fprintf(stderr, "Invalid operation format 4.\n");
    return;
  }

  // Convert log number
  _logNumber = atoi(logNumberString);

  // Convert process number
  _processNumber = atoi(processNumberString);

  // Convert time
  _time = atoll(timeString);

  // Copy class name
  strcpy(_className, className);

  // Copy operation information
  strcpy(_info, info);

}

/*--------------------------------------------------------------------------*/

Operation::~Operation() {

  //fprintf(stdout, "Destructor\n");

}

/*--------------------------------------------------------------------------*/

char* Operation::GetClassName() {

  // Return class name
  return _className;

}

/*--------------------------------------------------------------------------*/

char* Operation::GetInfo() {

  // Return operation information
  return _info;

}

/*--------------------------------------------------------------------------*/

int Operation::GetLogNumber() {

  // Return log number
  return _logNumber;

}

/*--------------------------------------------------------------------------*/

int Operation::GetProcessNumber() {

  // Return process number
  return _processNumber;

}

/*--------------------------------------------------------------------------*/

long long Operation::GetTime() {

  // Return operation time
  return _time;

}

/*--------------------------------------------------------------------------*/
