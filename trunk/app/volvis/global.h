/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

//Contains the global windowing parameters for the vol-a-tile app
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "glUE.h"
#include "vVolume.h"
#include "Trackball.h"
#include "vNet.h"
#include <vector>
class vPrimitive;
typedef enum {
	NONE=0,
	AGAVE,
	CROSSEYE
} vStereoType;

enum STEREOVIEW { LEFT, RIGHT};

struct vWindow {  //Window size and location attributes
	unsigned int	width;
	unsigned int	displayWidth;  //for stereo etc
	unsigned int	height;
	unsigned int	xPos;
	unsigned int	yPos;
	float aspect;
};

struct vFrustum {
	float left, right, top, bottom;
};

struct vViewport {
	float x, y, width, height;
};

struct vStereo {
	float			halfEyeSep;
	vStereoType		type;
};

typedef vector<vFrustum> vFrustumList;
typedef vector<vViewport> vViewportList;
struct vEnv {     //GL rendering Environment parameters
	float                 eye[3];    // eye location
	float                 at[3];     // lookat point
	float                 up[3];     // up vector
	// frustum[0] = left,   frustum[1] = right frustum[2] = bottom, frustum[3] = top
	vFrustumList		frustumList;
	vViewportList		viewportList;
	float                 clip[2];   // clip[0] = front, clip[1] = back
	float                 diff[4];   // diffuse color (material)
	float                 spec[4];   // speculare color (material)
	int                   bgColor;   // background color? 0==white 1==black
	vStereoType	stereoType; //type of stereo, NONE, AGAVE, CROSSEYED
	bool		renderOn;
	bool		gameMode;
};

struct vLight{   //lighting information
	float                 pos[3];    //light position
	float                 startpos[3];
};

struct vCutTransform {
	double rotn[16]; //just rotation
	double transform[16]; //cumulative transform(with the volume)
	double user[4]; //eqn of the cut plane
	bool update;
	float axis[3];
	float angle;
};

struct vTransform{   //rendering transform parameters
	float                 rotn[16];    //rotation from trackball
	float                 scale;        //isotropic scale value
	float                 trans[3];     //translation
};

struct vMouse{   //mouse state information
	int                   button;  //GLUT mouse button
	int                   state;   //GLUT mouse state
	int                   pos[2];  //x,y
	int                   last[2]; //last mouse position
	int                   shift;   //is shift down
	int                   ctrl;    //is control down
	int                   alt;     //is alt down
};

struct vKey {     //key board state
	unsigned char         key;     //GLUT key
	int                   shift;   //is shift down
	int                   ctrl;    //is control down
	int                   alt;     //is alt down
};


struct vRen {  //volume rendering paramters
	float                 sampleRate;   //current sample rate for rendering
	float                 interactSamp; //interactive sample rate
	float                 goodSamp;     //high quality sample rate
	int 			lowRes;		//low res rendering
	unsigned char        *deptex;       //2d dependent texture 256x256xRGBA [gb]
	unsigned char* gDeptex;	//this is recalculated from deptex above, alpha values are recalculated based on no of slices
	unsigned char* iDeptex;	//this is recalculated from deptex above
	unsigned char* nDeptex;	//this is recalculated from deptex above

	float lastSamp;            	//last sample rate rendered
	float lastGoodSamp;        	//last good sample rate
	float lastInteSamp;       	 //last interactive sample rate

	int                   loadTLUT;     //reload the LUT if flag == 1
	int 			loadVolume; //reload the volume if == 1
	int                   scaleAlphas;  //scale alphas with sample rate
	float                 gamma;        //constant alpha scale
	int 			probe[3]; //the 3 points corresponding to the probe ptr
	unsigned int	scaledDeptexName, origDeptexName;
};

typedef enum {
	V_ROAM,
	V_PROBE
} vNav;

struct vUI {
	bool printFPS;
	bool printSlices;
	bool cutEnabled;
	bool bboxEnabled;
	vNav navMode;
	bool animate;
};


typedef enum {  //Major axies of volume coords
	V_Unknown,
		V_XPos,
		V_XNeg,
		V_YPos,
		V_YNeg,
		V_ZPos,
		V_ZNeg
} VolRenMajorAxis;


typedef enum {  //v Shade modes (shadows are handled by the light struct)
		V_UNKNOWN,
		V_AMBIENT,     //ambient
		V_DIFFUSE,    //diffuse
		V_DSPEC,   //diffuse + specular
		V_FAUX,    //Faux shading
		V_ARB,     //arbitrary shading via pixel texture
		V_MIP
} vShade;

typedef enum {       //v platforms supported
	GPGineric,         //default
		GPNV20,            //-nv20 - 3D textures GeForce 3
		GPATI8K            //ATI Radeon 8000 (R200)
} vPlatform;


typedef enum {
	V_NONE,
		V_UNDER,
		V_OVER,
		V_ZERO
} vBlend;


struct vGlobal {
	int			debug;
	vWindow		win;    //window
	vEnv		env;    //environment
	vLight		light;  //light
	vVolume		*volume ;//pointer to the data to be rendered
	vVolume		*fullVolume;//pointer to the full volume
	vMouse		mouse;  //mouse
	vPlatform	plat;   //plaform
	vRen		volren; //volume rendering stuff
	int			mainWindow;  //mainWindow Identifier
	int			tfWindow;  //mainWindow Identifier
//	vShade		shade;       //shading enable/disable flag
//	vBlend		reblend;     //re-render scene with blend
	vStereo		stereo; //stereo params
	vUI			ui;
	vCutTransform		cut;
	float 		pointerPos[3]; //posn of the pointer in world coords
	int		curPrimIdx; //index of the primitive thats currently selected, -1 if nothing is selected
};

void drawText(GLfloat x, GLfloat y, GLfloat z, char *message);

void    mouse(int button, int state, int x, int y); //mouse callback
void	passiveMotion(int x, int y) ;
void    motion(int x, int y);                       //motion callback
void    key(unsigned char key, int x, int y);   //key callback
void    special(int key, int x, int y);         //special key callback
void    idle();                                 //idle callback
void    reshape(int w, int h);                      //reshape callback
void    initGlobal() ;
void	display();
void    displayOneEye(STEREOVIEW eye);
void    displayOneEye(int,int);
void    initGL();
void	parseCmdLine(int, char**);
void deallocateAll() ;
void enableTex3D(unsigned int ); //opengl init before the render
void disableTex3D(); //opengl stuff after the render

void updateAnim(int value);
void calcFrustum();

extern vGlobal     global;
extern Trackball	cutTrack;	//global trackball
extern vector<vPrimitive*>   renderList; //all renderables attatch to this one
extern vector<vPrimitive*>   selectList; //all renderables that can be selected

#if defined(V_DISTRIBUTED)
extern vNet* net;
#endif

#endif


