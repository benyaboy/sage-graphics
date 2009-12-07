//
// ============================================================
//                     === OpenGL Library === 
//
// $Date: 2004/10/28 22:43:53 $
// $Revision: 1.1 $
//
// Hacked into C++ from SGI's trackball.h---see copyright
// at end.
// 
// ============================================================
//

#include <iostream.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

#include "Trackball.h"
#include "VectorMath.h"
#include "MatrixMath.h"


//===========================================================================
//===========================================================================
// Static Initialization
//===========================================================================
//===========================================================================

//  This size should really be based on the distance from the center of
//  rotation to the point on the object underneath the mouse.  That
//  point would then track the mouse as closely as possible.  This is a
//  simple example, though, so that is left as an Exercise for the
//  Programmer.
float  Trackball::trackballSize = (float)0.9;

//  Number of iterations before we renormalize.
int  Trackball::renormCount = 97;


//===========================================================================
//===========================================================================
// Constructors/Destructor
//===========================================================================
//===========================================================================

Trackball::Trackball()
{
  lastX = 0.0;
  lastY = 0.0;
  vZero(lastQuat);
  vZero(curQuat);
  lastQuat[3] = 1.0;
  curQuat[3] = 1.0;
}


Trackball::~Trackball()
{
  //  Empty.
}



//===========================================================================
// Public Member Functions
//===========================================================================

void
Trackball::start(float x, float y)
{
  lastX = x;
  lastY = y;
  vZero(lastQuat);
  lastQuat[3] = 1.0;
}



/////////////////////////////////////////////////////////////////////////////
//
//  Ok, simulate a track-ball.  Project the points onto the virtual
//  trackball, then figure out the axis of rotation, which is the cross
//  product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
//  Note:  This is a deformed trackball-- is a trackball in the center,
//  but is deformed into a hyperbolic sheet of rotation away from the
//  center.  This particular function was chosen after trying out
//  several variations.
//
//  It is assumed that the arguments to this routine are in the range
//  (-1.0 ... 1.0)
//
void
Trackball::update(float x, float y)
{
  float p1[3], p2[3], d[3];
  float t;

  //  Might just be able to return here.
  if (lastX == x && lastY == y) {
    /* Zero rotation */
    vZero(lastQuat);
    lastQuat[3] = 1.0;
    return;
  }

  /*
   * First, figure out z-coordinates for projection of P1 and P2 to
   * deformed sphere
   */
  vSet(p1, lastX, lastY, projectToSphere(trackballSize, lastX, lastY));
  vSet(p2, x,     y,     projectToSphere(trackballSize,  x,    y));

  /*
   *  Now, we want the cross product of P1 and P2
   */
  vCross(p2, p1, a);

  /*
   *  Figure out how much to rotate around that axis.
   */
  vSub(p1,p2,d);
  t = (float)(vLength(d) / (2.0*trackballSize));

  /*
   * Avoid problems with out-of-control values...
   */
  if (t > 1.0) t = 1.0;
  if (t < -1.0) t = -1.0;
  phi = (float)(2.0 * asin(t));

  axisToQuat(a, phi, lastQuat);

  lastX = x;
  lastY = y;

  addQuats(lastQuat, curQuat, curQuat);
}

//get the axis and angle for the rotation
void Trackball::getAxisAngle(float& angle, float axis[3]) {
	angle = phi*180.0f/3.141517;
	for (int i=0;i<3;i++) 
		axis[i] = a[i];
}

/////////////////////////////////////////////////////////////////////////////
// joeys clear
//
void 
Trackball::clear()
{
  lastX = 0.0;
  lastY = 0.0;
  vZero(lastQuat);
  vZero(curQuat);
  lastQuat[3] = 1.0;
  curQuat[3] = 1.0;
}



/////////////////////////////////////////////////////////////////////////////
//
//  Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
//  if we are away from the center of the sphere.
//
float
Trackball::projectToSphere(float r, float x, float y)
{
  float d, t, z;

  d = (float)(sqrt(x*x + y*y));
  if (d < r * 0.70710678118654752440) {    /* Inside sphere */
    z = (float)(sqrt(r*r - d*d));
  } else {           /* On hyperbola */
    t = (float)(r / 1.41421356237309504880);
    z = t*t / d;
  }
  return z;
}



/////////////////////////////////////////////////////////////////////////////
//
//  Build a rotation matrix, given a quaternion rotation.
//
void
Trackball::buildRotMatrix(float m[4][4])
{
  m[0][0] = (float)(1.0 - 2.0 * (curQuat[1] * curQuat[1] + curQuat[2] * curQuat[2]));
  m[0][1] = (float)(2.0 * (curQuat[0] * curQuat[1] - curQuat[2] * curQuat[3]));
  m[0][2] = (float)(2.0 * (curQuat[2] * curQuat[0] + curQuat[1] * curQuat[3]));
  m[0][3] = (float)(0.0);

  m[1][0] = (float)(2.0 * (curQuat[0] * curQuat[1] + curQuat[2] * curQuat[3]));
  m[1][1]= (float)(1.0 - 2.0 * (curQuat[2] * curQuat[2] + curQuat[0] * curQuat[0]));
  m[1][2] = (float)(2.0 * (curQuat[1] * curQuat[2] - curQuat[0] * curQuat[3]));
  m[1][3] = (float)(0.0);

  m[2][0] = (float)(2.0 * (curQuat[2] * curQuat[0] - curQuat[1] * curQuat[3]));
  m[2][1] = (float)(2.0 * (curQuat[1] * curQuat[2] + curQuat[0] * curQuat[3]));
  m[2][2] = (float)(1.0 - 2.0 * (curQuat[1] * curQuat[1] + curQuat[0] * curQuat[0]));
  m[2][3] = (float)(0.0);

  m[3][0] = (float)(0.0);
  m[3][1] = (float)(0.0);
  m[3][2] = (float)(0.0);
  m[3][3] = (float)(1.0);
}

void
Trackball::reapply()
{
  addQuats(lastQuat, curQuat, curQuat);
}
