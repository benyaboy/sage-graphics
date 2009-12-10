/*---------------------------------------------------------------------------*/
/* Volume Rendering Application Testing Tools                                */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                  */
/*                                                                           */
/* This software is free software; you can redistribute it and/or modify it  */
/* under the terms of the GNU Lesser General Public License as published by  */
/* the Free Software Foundation; either Version 2.1 of the License, or       */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This software is distributed in the hope that it will be useful, but      */
/* WITHOUT ANY WARRANTY; without even the implied warranty of                */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser   */
/* General Public License for more details.                                  */
/*                                                                           */
/* You should have received a copy of the GNU Lesser Public License along    */
/* with this library; if not, write to the Free Software Foundation, Inc.,   */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                     */
/*---------------------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "FrustumCalculator.h"
#include "Matrix.h"
#include "Vertex.h"

/*---------------------------------------------------------------------------*/

#define max(A,B) ( (A) > (B) ? (A):(B))
#define min(A,B) ( (A) < (B) ? (A):(B))

/*---------------------------------------------------------------------------*/

int main(int argc, char** argv) {

  // Data set name
  char _dataName[1024];

  // Data set prefix
  char _dataPrefix[1024];

  // Total display width in pixels
  int _displayWidth = 0;

  // Total display height in pixels
  int _displayHeight = 0;

  // Total display's vertical field of view
  float _displayVFOV = 0.0;

  // Total display's near clipping plane
  float _displayNear = 0.0;

  // Total display's far clipping plane
  float _displayFar = 0.0;

  // Total dislay's left clipping plane
  float _displayLeft = 0.0;

  // Total display's right clipping plane
  float _displayRight = 0.0;

  // Total display's bottom clipping plane
  float _displayBottom = 0.0;

  // Total display's top clipping planes
  float _displayTop = 0.0;

  // CacheRAM size
  int _ramSize = -1;

  // Scale matrix
  float S[16];

  // CacheVRAM size
  int _vramSize = -1;

  // Wait time between operations
  int _wait = 0;


  // Bounding vertices of entire data set
  Vertex _dataVertex[8];


  // Check command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: ScriptGenerator inputFile outputFile\n");
    exit(1);
  }

  // Input file
  FILE* infptr = NULL;

  // Try to open input file
  if ((infptr = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Can not read input file %s\n", argv[1]);
  }

  // Read display width and height
  fscanf(infptr, "%d %d", &_displayWidth, &_displayHeight);

  // Read display's vertical field of view
  fscanf(infptr, "%f", &_displayVFOV);

  // Read display's near and far clipping planes
  fscanf(infptr, "%f %f", &_displayNear, &_displayFar);

  // Read overview vertices
  for (int i = 0 ; i < 8 ; i++) {
    float x, y, z;
    fscanf(infptr, "%f %f %f", &x, &y, &z);
    _dataVertex[i].SetXYZ(x, y, z);
  }

  // Read inital scale matrix
  fscanf(infptr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
         &S[0], &S[4], &S[8], &S[12],
         &S[1], &S[5], &S[9], &S[13],
         &S[2], &S[6], &S[10], &S[14],
         &S[3], &S[7], &S[11], &S[15]);

  // Read wait time
  fscanf(infptr, "%d", &_wait);

  // Read data set prefix
  fscanf(infptr, "%s", _dataPrefix);

  // Read data set name
  fscanf(infptr, "%s", _dataName);

  // Read cache sizes
  fscanf(infptr, "%d %d", &_vramSize, &_ramSize);

  // Close input file
  fclose(infptr);


  // Output file
  FILE* outfptr = NULL;

  // Try to open output file
  if ((outfptr = fopen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Can not open output file %s\n", argv[2]);
  }

  // Print script header
  fprintf(outfptr, "VRA_UI_SCRIPT\n");

  // Print data set prefix
  fprintf(outfptr, "CMD_PRE %s\n", _dataPrefix);

  // Print data set name
  fprintf(outfptr, "CMD_DAT %s %d %d\n", _dataName, _ramSize, _vramSize);

  // Wait for some time while data is loaded
  fprintf(outfptr, "WAIT 30\n");


  // Setup frustum calculator
  FrustumCalculator frustumCalculator;
  frustumCalculator.SetDisplayWidth(_displayWidth);
  frustumCalculator.SetDisplayHeight(_displayHeight);
  frustumCalculator.SetNearClippingPlane(_displayNear);
  frustumCalculator.SetVerticalFieldOfView(_displayVFOV);

  // Calculate full frustum for entire display
  frustumCalculator.CalculateFrustum();
  frustumCalculator.GetFullFrustum(&_displayLeft,
                                   &_displayRight,
                                   &_displayBottom,
                                   &_displayTop);


  // Calculate frustum width, height, and depth
  float frustumWidth = fabs(_displayRight - _displayLeft);
  float frustumHeight = fabs(_displayTop - _displayBottom);
  float frustumDepth = fabs(_displayFar - _displayNear);

  // Determine minimum frustum axis
  float minFrustumAxis = min(frustumWidth, min(frustumHeight, frustumDepth));


  // Calculate width, height, and depth of data set
  float dataWidth = fabs(_dataVertex[3].GetX() - _dataVertex[0].GetX());
  float dataHeight = fabs(_dataVertex[4].GetY() - _dataVertex[0].GetY());
  float dataDepth = fabs(_dataVertex[1].GetZ() - _dataVertex[0].GetZ());

  // Determine maximum data axis
  float maxDataAxis = max(dataWidth, max(dataHeight, dataDepth));


  // Calculate scale factor
  //float scaleFactor = maxDataAxis / minFrustumAxis;
  float scaleFactor = frustumWidth / dataWidth;


  // Write scale matrix
  //fprintf(outfptr, 
  //        "CMD_SCA %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
  //        S[0], S[4], S[8], S[12], S[1], S[5], S[9], S[13],
  //        S[2], S[6], S[10], S[14], S[3], S[7], S[11], S[15]);
  fprintf(outfptr, 
          "CMD_SCA %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
          scaleFactor, 0.0, 0.0, 0.0, 0.0, scaleFactor, 0.0, 0.0,
          0.0, 0.0, scaleFactor, 0.0, 0.0, 0.0, 0.0, 1.0);


  /*
  // Base rotation matrix: UNUSED
  float Rb[16];

  // Calculate base rotation: UNUSED

  // Width is biggest
  if (dataWidth >= dataHeight && dataWidth >= dataDepth) {

    // Rotate 0.0 degrees about the x axis producing the identity matrix
    calc_rot_x(Rb, 0.0);

  }

  // Height is biggest
  else if (dataHeight >= dataWidth && dataHeight >= dataDepth) {

    // Rotate 90.0 degrees about the z axis
    calc_rot_z(Rb, 90.0);

  }

  // Depth is biggest
  else if (dataDepth >= dataWidth && dataDepth >= dataHeight) {

    // Rotate 90.0 degrees about the y axis
    calc_rot_y(Rb, 90.0);

  }
  */


  // Rotation matrix
  float R[16];

  // Rotate about y axis
  for (float i = 0.0 ; i < 180.0 ; i += 1.0) {

    // Calculate rotation about y axis
    calc_rot_y(R, i);

    fprintf(outfptr, 
            "CMD_ROT %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
            R[0], R[4], R[8], R[12], R[1], R[5], R[9], R[13],
            R[2], R[6], R[10], R[14], R[3], R[7], R[11], R[15]);
    fprintf(outfptr, "WAIT 1\n");
    fprintf(outfptr, "CMD_REN\n");
    fprintf(outfptr, "WAIT %d\n", _wait);

  }


  // Rotate about x axis
  /*
  for (float i = 0.0 ; i < 360.0 ; i += 1.0) {

    // Calculate rotation about x axis
    calc_rot_x(R, i);

    fprintf(outfptr, 
            "CMD_ROT %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
            R[0], R[4], R[8], R[12], R[1], R[5], R[9], R[13],
            R[2], R[6], R[10], R[14], R[3], R[7], R[11], R[15]);
    fprintf(outfptr, "WAIT 1\n");
    fprintf(outfptr, "CMD_REN\n");
    fprintf(outfptr, "WAIT %d\n", _wait);

  }
  */


  // Rotate about z axis
  /*
  for (float i = 0.0 ; i < 360.0 ; i += 1.0) {

    // Calculate rotation about y axis
    calc_rot_z(R, i);

    fprintf(outfptr, 
            "CMD_ROT %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
            R[0], R[4], R[8], R[12], R[1], R[5], R[9], R[13],
            R[2], R[6], R[10], R[14], R[3], R[7], R[11], R[15]);
    fprintf(outfptr, "WAIT 1\n");
    fprintf(outfptr, "CMD_REN\n");
    fprintf(outfptr, "WAIT %d\n", _wait);

  }
  */
  

  // Close output file
  fclose(outfptr);


  // Return
  return 0;

}

/*---------------------------------------------------------------------------*/
