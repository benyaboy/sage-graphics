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

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#ifdef WIN32
#include <windows.h>		//  must be included before any OpenGL
#endif

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include "Trackball.h"     //SGI quaternion trackball
#include "VectorMath.h"    //usefull vector/matrix operations
#include "global.h"         //global definitions
#include "glUE.h"          //gl utilities and extensions
#include "CCmdLine.h"
#include "vFPSText.h"
#include "vRenderer.h"     //standard renderer
#include "vPrimitive.h"
#include "vGeometry.h"
#include "vNet.h"
#include "vUI.h"
#include "vSliceView.h"

#if defined(ADDCGGL)
#include "vCG.h"
#endif
#if defined(ADDARBGL)
#include "vARB.h"
#endif

#if defined(V_DISTRIBUTED)
#include "mpi.h"
#include "vNet.h"
#endif

#if defined(V_NETTF)
#include "vTF.h"
#endif
vector<vPrimitive*>   renderList; //all renderables attatch to this one
vGlobal		global;	//the global v data structure (global.h)
Trackball	cutTrack;	//global trackball

/* Atul June 8 2004. Commented out the # since vNet is required for
 * stand alone version also */
#if defined(V_DISTRIBUTED)
vNet* net = 0;	//The volvis controller
#endif
/* Atul June 8 2004 */

#if defined(V_NETTF)
vTF * tfNet = 0;	//The tfcontroller
#endif

#if defined(V_SAGE)
	// headers for SAGE
#include "sail.h"
#include "misc.h"
int winWidth, winHeight;
GLubyte *rgbBuffer = 0;
sail sageInf; // sail object
#endif



enum {
	MENU_ROAM = 1,
	MENU_PROBE,
        MENU_CUT,
	MENU_ISOSURFACE,
	MENU_ISOPOINTS,
	MENU_BOUNDBOX,
	MENU_FPS,
	MENU_SAMPLERATE,
	MENU_SAVEVOLUME,
	MENU_SAVEGRADIENT,
	MENU_HELP,
	MENU_EXIT
};

void menuCallback(int idCommand)
{
	switch (idCommand)
	{
		case MENU_ROAM:
			setRoam();
			break;
		case MENU_PROBE:
			setProbe();
			break;
		case MENU_CUT:
			toggleCutPlane();
			break;
		case MENU_ISOSURFACE:
			genIsosurface(100,100);
			break;
		case MENU_ISOPOINTS:
			//genIsopoint(100,100);
			break;
		case MENU_BOUNDBOX:
			toggleBoundBox();
			break;
		case MENU_FPS:
			togglePrintFPS();
			break;
		case MENU_SAMPLERATE:
			break;
		case MENU_SAVEVOLUME:
			if (global.volume)
				global.volume->saveVolume("out");
			break;
		case MENU_SAVEGRADIENT:
			saveGradientVolume();
			break;
		case MENU_HELP:
			printUsage();
			break;
		case MENU_EXIT:
			doExit();
			break;
	}
}

int buildPopupMenu (void)
{
        int menu;
        menu = glutCreateMenu(menuCallback);
        glutAddMenuEntry ("Roam", MENU_ROAM);
        glutAddMenuEntry ("Probe", MENU_PROBE);
        glutAddMenuEntry ("Enable Cut Plane", MENU_CUT);
        glutAddMenuEntry ("Gen Isosurface", MENU_ISOSURFACE);
        glutAddMenuEntry ("Boundbox Toggle", MENU_BOUNDBOX);
        glutAddMenuEntry ("FPS Toggle", MENU_FPS);
        glutAddMenuEntry ("Sample Rate", MENU_SAMPLERATE);
        glutAddMenuEntry ("Save Volume", MENU_SAVEVOLUME);
        glutAddMenuEntry ("Save Gradient Volume", MENU_SAVEGRADIENT);
        glutAddMenuEntry ("Print Help", MENU_HELP);
        glutAddMenuEntry ("Exit", MENU_EXIT);
        return menu;
}

void calcFrustum() {
	float fov = 45.0f*3.1415/180.0f;
	float b = global.env.clip[0] * (float) tan(fov * 0.5);
	float r = b * global.win.aspect;
	for (int i=0;i<global.env.frustumList.size();i++) {
		global.env.frustumList[i].left *= r;  //left
		global.env.frustumList[i].right *= r;   //right
		global.env.frustumList[i].bottom *= b;  //bottom
		global.env.frustumList[i].top *= b;   //top
	}
}

//loads the data in dtex to the handle associated with the dependant texture
//sent to CG
void loadDepTex(unsigned int deptexName, unsigned char *dtex)
{
#if defined(ADDCGGL)
	if (fprofile == CG_PROFILE_FP30) {
		// 1D tex
		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, deptexName);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, depX, 0, GL_RGBA, GL_UNSIGNED_BYTE, dtex);
		GlErr("vRenderer", "loadDepTex - loading scaled dependant texture");
		glDisable(GL_TEXTURE_1D);
	}
	else if (fprofile == CG_PROFILE_FP20) {
		// 2D tex for FP20
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, deptexName);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depX, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dtex);
		GlErr("vRenderer", "loadDepTex - loading scaled dependant texture");
		glDisable(GL_TEXTURE_2D);
	}
#endif

#if defined(ADDARBGL)
	// 2D tex for FP20
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, deptexName);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, depX, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dtex);
	GlErr("vRenderer", "loadDepTex - loading scaled dependant texture");
	glDisable(GL_TEXTURE_2D);
#endif
}

//takes the data from the 2d texture sent from the UI and copies it to the dependant texture
//that will be passed to CG
//the dependant texture is 1D but the UI texture is 2d, so for now,
//just extract the first row of the UI 2d texture to the dependant texture
void copyScale(float sampleRate, unsigned char *cgTex)
{
	float alphaScale = 1.0/sampleRate;
	for(int j=0; j<depX; ++j){
		cgTex[j*4 + 0] = global.volren.deptex[j*4 + 0];
		cgTex[j*4 + 1] = global.volren.deptex[j*4 + 1];
		cgTex[j*4 + 2] = global.volren.deptex[j*4 + 2];
		cgTex[j*4 + 3] = (1.0 - ::pow((1.0-(global.volren.deptex[j*4 + 3]/255.0)), alphaScale))*255;

	}
}

void update() {
	//set the current sampling rate based on the interaction mode
	if (global.volren.lowRes)
		global.volren.sampleRate = global.volren.interactSamp;
	else
		global.volren.sampleRate = global.volren.goodSamp;

	//Re-Scale Alpha values depending on no of slices
	//if we need to load a new TF, load it here (from the remote UI)
	if(global.volren.scaleAlphas){
		if((global.volren.lastSamp != global.volren.sampleRate)||(global.volren.loadTLUT)){  //see if the sample rate changed
			if((global.volren.lastGoodSamp != global.volren.goodSamp) || global.volren.loadTLUT){ //good sample rate changed
				copyScale(global.volren.goodSamp * 1/global.volren.gamma, global.volren.gDeptex);
				global.volren.lastGoodSamp = global.volren.goodSamp;
			}
			if((global.volren.lastInteSamp != global.volren.interactSamp) || global.volren.loadTLUT){ //interact samp rate changed
				copyScale(global.volren.interactSamp * 1/global.volren.gamma, global.volren.iDeptex);
				global.volren.lastInteSamp = global.volren.interactSamp;
			}
			if(global.volren.sampleRate == global.volren.goodSamp){ //which one do we load (good)
				loadDepTex(global.volren.scaledDeptexName,global.volren.gDeptex);
				global.volren.lastSamp = global.volren.goodSamp;
			}
			else if(global.volren.sampleRate == global.volren.interactSamp){ //(interactive)
				loadDepTex(global.volren.scaledDeptexName,global.volren.iDeptex);
				global.volren.lastSamp = global.volren.interactSamp;
			}
			copyScale(0.3, global.volren.nDeptex);
			loadDepTex(global.volren.origDeptexName, global.volren.nDeptex);
			global.volren.loadTLUT = 0;
		}
	} else {  //just do gamma scale, don't update for the sample rate (for testing purposes)
		if(global.volren.loadTLUT){
			copyScale(1/global.volren.gamma, global.volren.gDeptex);
			loadDepTex(global.volren.scaledDeptexName, global.volren.gDeptex);
			global.volren.loadTLUT = 0;
		}
	}

}

void deallocateAll() {
	if (global.volume);
		delete global.volume;
	if (global.fullVolume)
		delete global.fullVolume;
	for (int i=0;i<renderList.size();i++)
		delete renderList[i];

	delete [] global.volren.deptex ;
	delete [] global.volren.nDeptex;
	delete [] global.volren.gDeptex;
	delete [] global.volren.iDeptex;

}
int main(int argc, char **argv) {
	initGlobal(); //init global variables

	//init network related - MPI will change argc and argv
	// so pass as ref
	QUANTAinit();
	srand( (unsigned)time(NULL)); //initialise randon number generator
/* Atul June 8 2004. Commented out the # since vNet is required for
 * stand alone version also */
#if defined(V_DISTRIBUTED)
	net = new vNet();
	net->init(argc,argv);
#endif
/* Atul June 8 2004 */

#if defined(V_NETTF) //want transfer functions
#if defined (V_DISTRIBUTED) //in distributed mode, only master has this instance
	if (net->isActiveTile()) {
		tfNet = new vTF();
		tfNet->init();
	}
#else //in standalone instantiate it
	 tfNet = new vTF();
	 tfNet->init();
#endif
#endif

	//command line parsing - stores argumenents in global.*
	parseCmdLine(argc,argv);

#ifdef V_DISTRIBUTED
        if (net->isActiveTile()) {
               setenv("DISPLAY", getenv("VOLATILE_DISPLAY"), 1);
        }
#endif

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(global.win.width, global.win.height);
	glutInitWindowPosition(global.win.xPos, global.win.yPos);
	global.mainWindow =	glutCreateWindow("Vol-a-Tile");

#if defined(V_SAGE)
#if defined (V_DISTRIBUTED)
	if (!net->isActiveTile())
	{
		sageInf.init("sail.conf", "volatile", net->getRank()-1,
                     global.win.width, global.win.height, 24, TVPIXFMT_888, BOTTOM_TO_TOP);
		fprintf(stderr, "SAIL width %d height %d\n", global.win.width, global.win.height);
	}
#else
    sageInf.init("sail.conf", "volatile", 0,
                 global.win.width, global.win.height, 24, TVPIXFMT_888, BOTTOM_TO_TOP);
#endif
#endif

#ifdef WIN32
	LoadAllExtensions();
#endif

	initGL(); //opengl init stuff
	//init the dependant textures
	//initialize the dependant texture for transfer functions
	//copy the good and interactive dependant textures
	copyScale(global.volren.goodSamp, global.volren.gDeptex);
	copyScale(global.volren.interactSamp, global.volren.iDeptex);
	copyScale(0.3, global.volren.nDeptex);
	global.volren.lastSamp = global.volren.goodSamp;
	global.volren.lastGoodSamp = global.volren.goodSamp;
	global.volren.lastInteSamp = global.volren.interactSamp;

	loadDepTex(global.volren.scaledDeptexName,global.volren.gDeptex);
	loadDepTex(global.volren.origDeptexName,global.volren.nDeptex);
	//load the given 1D texture to CG


#if defined(ADDCGGL)
        //init the CG library
#if defined (V_DISTRIBUTED)
    if (!net->isActiveTile())
#endif
        initCG();  // Master doesn't need CG
#endif

#if defined(ADDARBGL)
#if defined (V_DISTRIBUTED)
    if (!net->isActiveTile())
#endif
        initARB(); //init the ARB OpenGL
#endif

	//start creating our objects
	//creating renderer object
	if (global.volume) {
		vRenderer* renderer = new vRenderer();
		renderer->setVolume(global.volume);
		renderList.push_back(renderer);

		if (global.fullVolume) {
			vRenderer* fullRenderer = new vRenderer;
			fullRenderer->setVolume(global.fullVolume);
			fullRenderer->disableCut();
			fullRenderer->enableShowOverview();
			fullRenderer->setTranslation(2.0,0.0,0.0);
			renderList.push_back(fullRenderer);
		}
		//slice view
		vSliceView* view = new vSliceView;
		renderList.push_back(view);
		//the info
		vVolText* text = new vVolText;
		renderList.push_back(text);

	}


	//now create the FPS text
	vFPSText* timerText = new vFPSText;
	renderList.push_back(timerText);
	 //now create the pointer
	//vPointer* ptr = new vPointer;
	//renderList.push_back(ptr);

	//initialise all the objects
	for (int i=0;i<renderList.size();i++) {
		renderList[i]->init();
	}

	GlErr("vRenderer", "init");

#ifdef V_DISTRIBUTED
	if (net->isActiveTile()) {
		global.win.width  = 800;
		global.win.height = 600;
	}
	else {
        if (global.env.gameMode)
            glutFullScreen();
		glutSetCursor(GLUT_CURSOR_NONE);
	}
#else
	if (global.env.gameMode)
		glutFullScreen();
#endif

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	//only use idle when we are in standalone mode
#ifndef V_DISTRIBUTED
	glutIdleFunc(idle);
#endif
	buildPopupMenu ();
    glutAttachMenu (GLUT_RIGHT_BUTTON);
	//only master does the update and broadcasts it to client
	//so that everyone is syncronized
#ifdef V_DISTRIBUTED
	//if (net->isActiveTile())
	glutTimerFunc(50, updateAnim, 1);
#else
	glutTimerFunc(50, updateAnim, 1);
#endif
	glutMainLoop();
	return 0;
}

void    initGlobal() {
	global.volume = 0;
	global.fullVolume = 0;

	global.win.width  = 1024;      //size
	global.win.height = 768;
	global.win.xPos   = 0;      //position
	global.win.yPos   = 0;
	global.win.aspect = 1.0f;

	global.env.eye[0]     = 0;    //eye
	global.env.eye[1]     = 0;
	global.env.eye[2]     = 10.0;
	global.env.at[0]      = 0;    //at
	global.env.at[1]      = 0;
	global.env.at[2]      = 0;
	global.env.up[0]      = 0;    //up
	global.env.up[1]      = 1;
	global.env.up[2]      = 0;
	global.env.renderOn = true;

	//create a default frustum
	vFrustum newFrustum;
	newFrustum.left = newFrustum.bottom = -1.0;
	newFrustum.right= newFrustum.top = 1.0;
	global.env.frustumList.push_back(newFrustum);

	//create a default viewpoert
	vViewport newViewport;
	newViewport.x = newViewport.y = 0.0f;
	newViewport.width = newViewport.height = 1.0f;
	global.env.viewportList.push_back(newViewport);

	global.env.clip[0]    = 1;    //front
	global.env.clip[1]    = 50;   //back
	global.env.bgColor    = 0;    //white background

	global.mouse.alt     = 0;
	global.mouse.ctrl    = 0;
	global.mouse.shift   = 0;

	//lighting
	global.light.startpos[0] = -1.0f;
	global.light.startpos[1] = -1.0f;
	global.light.startpos[2] =-1.0f;
	global.light.startpos[3] = 0.0f;

	global.volren.interactSamp= .1;      //Interactive sample rate
	global.volren.goodSamp    = 1.5;     //High quality sample ..
	global.volren.sampleRate  = global.volren.goodSamp; //samples per voxel
	global.volren.lowRes   = 0; //highres rendering
	global.volren.deptex = new unsigned char[256*256*4];
	for(int j=0; j<256; ++j){
		for(int k=0; k<256; ++k){
			global.volren.deptex[j*256*4 + k*4 + 0] =  (unsigned char)(k/(float)255*255);
			global.volren.deptex[j*256*4 + k*4 + 1] =  (unsigned char)(255 - k/(float)255*255);
			global.volren.deptex[j*256*4 + k*4 + 2] =  (unsigned char)( k/(float)255*255);
			global.volren.deptex[j*256*4 + k*4 + 3] =  (unsigned char)(k/(float)255*255/(float)2);
		}
	}
	global.volren.nDeptex = new unsigned char[256*4];
	global.volren.gDeptex	= new unsigned char[256*4];
	global.volren.iDeptex = new unsigned char[256*4];
	global.volren.loadTLUT    = 1;       //first time - need to load LUT
	global.volren.loadVolume = 0; 		//no need to load the volume
	global.volren.scaleAlphas = 1;       //scale alphas to the sample rate
	global.volren.gamma       = 1;       //gamma identitiy
	global.volren.probe[0] = global.volren.probe[1] = global.volren.probe[2] = 0;

	identityMatrix(global.cut.transform);  //init the rotation
	global.cut.user[0] = 1.0;
	global.cut.user[1] = global.cut.user[2] = 0.0;
	global.cut.user[3] = 0.5f;
	global.cut.update = false;

	global.ui.printFPS = false;
	global.ui.printSlices = false;
	global.ui.cutEnabled = false;
	global.ui.bboxEnabled = true;
	global.ui.navMode = V_ROAM;
	global.ui.animate = false;

	global.pointerPos[0] = global.pointerPos[1] = 0.0f;
	global.pointerPos[2] = 1.0f;
	global.curPrimIdx = 0;
	cutTrack.clear();
}


void initGL( void )
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glLineWidth(3);
	glGenTextures(1, &global.volren.scaledDeptexName);   //the dep tex that we use for the tf- passed to CG
	glGenTextures(1, &global.volren.origDeptexName);   //the dep tex that we use for the tf- passed to CG
	GlErr("v::","initGL()");
}

void display()
{
#ifdef V_DISTRIBUTED
	net->process();
#endif
	update();
	if (global.env.renderOn) {
		int vpx, vpy, vpwidth, vpheight;
		glEnable (GL_SCISSOR_TEST); //dont draw outside viewpott
		//depending on no of viewports
		for (int i=0;i<global.env.viewportList.size();i++) {
			vpx = global.env.viewportList[i].x*global.win.width;
			vpy = global.env.viewportList[i].y*global.win.height;
			vpwidth = global.env.viewportList[i].width*global.win.width;
			vpheight = global.env.viewportList[i].height*global.win.height;
			glViewport(vpx,vpy,vpwidth,vpheight);
			//cout<<"Viewport "<<vpx<<" "<<vpy<<" "<<vpwidth<<" "<<vpheight<<endl;
			glScissor (vpx,vpy,vpwidth,vpheight);
			displayOneEye(1,i); // our local display function
		}
		glDisable (GL_SCISSOR_TEST); //dont draw outside viewpott
	}
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef V_DISTRIBUTED
	net->sync();
#endif


#if defined(V_SAGE)
#if defined (V_DISTRIBUTED)
	if (!net->isActiveTile())
#endif
        if (winWidth > 0)
        {
            glReadPixels(0, 0, winWidth, winHeight, GL_RGB, GL_UNSIGNED_BYTE, rgbBuffer);
            sageInf.swapBuffer((void *)rgbBuffer);
        }
#endif

	glutSwapBuffers();
	GlErr("v:","disp()");
}

void displayOneEye(int eye, int vpIndex)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//dont draw anything on the master
	glPushMatrix();
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(global.env.eye[0],  //eye
				global.env.eye[1],
				global.env.eye[2],
				global.env.at[0],   //at
				global.env.at[1],
				global.env.at[2],
				global.env.up[0],
				global.env.up[1],
				global.env.up[2]);          //up

		GLfloat light_pos[] = { 5.0, 5.0, 5.0,1.0 };
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(global.env.frustumList[vpIndex].left, //left
			global.env.frustumList[vpIndex].right, //right
                        global.env.frustumList[vpIndex].bottom , //bottom
			global.env.frustumList[vpIndex].top, //top
                        global.env.clip[0],    //front
                        global.env.clip[1]);   //back

		/*cout<<"Frustrum "<<global.env.frustumList[vpIndex].left<<" "
			<<global.env.frustumList[vpIndex].right<<" "
			<<global.env.frustumList[vpIndex].bottom<<" "
			<<global.env.frustumList[vpIndex].top<<" "
			<<endl;
			*/
		glMatrixMode(GL_MODELVIEW);
		for (int i=0;i<renderList.size();i++) {
			renderList[i]->draw();
		}
		renderList[global.curPrimIdx]->setSelected(true);

	} glPopMatrix();
}

void mouse(int button, int state, int x, int y)
{
	y = global.win.height - y;
	global.mouse.button = button;
	global.mouse.state  = state;

	if(glutGetModifiers() & GLUT_ACTIVE_CTRL) global.mouse.ctrl = 1;
	else global.mouse.ctrl = 0;
	if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) global.mouse.shift = 1;
	else global.mouse.shift = 0;
	if(glutGetModifiers() & GLUT_ACTIVE_ALT) global.mouse.alt = 1;
	else global.mouse.alt = 0;
	if(state == GLUT_UP){
		global.mouse.button = -1;
	}
	//get the global coords

#if defined(V_DISTRIBUTED)
	if (!net->cmdFlushed)
		return;
#endif
	//standard button actions...
	switch(button){
	case GLUT_LEFT_BUTTON:
		{
			if (global.mouse.ctrl) { //emulate right button
				switch(state)
				{
				case GLUT_UP:
					endInteract();
					break;
				case GLUT_DOWN:
					startZoom(x,y);
					break;
				}
			}
			else {
				switch(state)
				{
				case GLUT_UP:
					endInteract();
					break;
				case GLUT_DOWN:
					startRotation(x,y);
					break;
				}
			}
		}
		break;
	case GLUT_RIGHT_BUTTON:
		{
			switch(state)
			{
			case GLUT_UP:
				endInteract();
				break;
			case GLUT_DOWN:
				startZoom(x,y);
				break;
			}
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		{
			switch(state)
			{
			case GLUT_UP:
				endInteract();
				break;
			case GLUT_DOWN:
				startTranslate(x,y);
				break;
			}
		}
		break;
	default:
		break;
	}
}

void passiveMotion(int x, int y) {
global.mouse.pos[0] = x;
y = global.win.height-y;

	//get the global coords
//	doSelect(x,y);
}

void motion(int x, int y)
{
#if defined(V_DISTRIBUTED)
	if (!net->cmdFlushed)
		return;
#endif
	y = global.win.height-y;
	//get the global coords
	switch(global.mouse.button)	{
	case GLUT_LEFT_BUTTON:
		{
			if (global.mouse.ctrl) { //emulate right button
				switch(global.mouse.state)
				{
				case GLUT_DOWN:
					doZoom(x,y);
					break;
				case GLUT_UP:
					break;
				}
			}
			else {
				switch(global.mouse.state)
				{
				case GLUT_DOWN:
					doRotation(x,y);
					break;
				case GLUT_UP:
					break;
				}
			}
		}
		break;
	case GLUT_RIGHT_BUTTON:
		{
			switch(global.mouse.state)
			{
			case GLUT_DOWN:
				doZoom(x,y);
				break;
			case GLUT_UP:
				break;
			}
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		{
			switch(global.mouse.state)
			{
			case GLUT_DOWN:
				doTranslate(x,y);
				break;
			case GLUT_UP:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void key(unsigned char key, int x, int y)
{
#if defined(V_DISTRIBUTED)
	if (!net->cmdFlushed)
		return;
#endif

	y = global.win.height-y;
	switch(key) {
	case '\033': //escape
		doExit();
		break;
	case 'b':
	case 'B':
		toggleBoundBox();
		break;
	case 'c':
	case 'C':
		toggleCutPlane();
		break;
	case 'f':
	case 'F':
		togglePrintFPS();
		break;
	case 'g':
	case 'G':
		saveGradientVolume();
		break;
	case 'h':
	case 'H':
		printUsage();
		break;
	case 'i':
	case 'I':
		genIsosurface(x,y);
		break;
	case 'j':
	case 'J':
		//genIsopoint(x,y);
		break;
//	case 'n':
//	case 'N':
//		global.ui.printSlices = !global.ui.printSlices;
//		break;
	case 'o':
	case 'O':
		global.volume->saveVolume("out");
		break;
	case 'p': //to probe volumes
	case 'P':
		setProbe();
		break;
	case 'r':
	case 'R':
		setRoam();
		break;
	case '+':
		//increase the number of slices
		scaleSampleRate(1.1);
		break;
	case '-':
		//decrease the number of slices
		scaleSampleRate(0.9);
		break;
	case '.':
		//increase isosurface value
		incIsoValue(15);
		break;
	case ',':
		//decrease isosurface value
		incIsoValue(-15);
		break;
	case ' ':
		animateVolume();
		break;
	case 8: //backspace key
		global.curPrimIdx--;
		if (global.curPrimIdx < 0)
			global.curPrimIdx = renderList.size()-1;
		doSelect(global.curPrimIdx);
		break;

	case 9: //tab key
		global.curPrimIdx++;
		if (global.curPrimIdx >= renderList.size())
			global.curPrimIdx = 0;
		doSelect(global.curPrimIdx);
		break;

	}
}


void special(int key, int x, int y)
{
	int ROAM = global.volume->maxDim/2;

#if defined(V_DISTRIBUTED)
	if (!net->cmdFlushed)
		return;
#endif
	switch(key){
    case GLUT_KEY_UP:
		if (global.ui.navMode == V_ROAM)
			roamVolume(0,ROAM,0);
		else if (global.ui.navMode == V_PROBE) {
			probeVolume(0,1,0);
		}
		else
			doAxisRotation(-1.0f,'x');
		break;
    case GLUT_KEY_DOWN:
		if (global.ui.navMode == V_ROAM)
			roamVolume(0,-ROAM,0);
		else if (global.ui.navMode == V_PROBE)
			probeVolume(0,-1,0);
		else
			doAxisRotation(1.0f,'x');
		break;
    case GLUT_KEY_RIGHT:
		if (global.ui.navMode == V_ROAM)
			roamVolume(ROAM,0,0);
		else if (global.ui.navMode == V_PROBE)
			probeVolume(1,0,0);
		else
			doAxisRotation(1.0f,'y');
		break;
    case GLUT_KEY_LEFT:
		if (global.ui.navMode == V_ROAM)
			roamVolume(-ROAM,0,0);
		else if (global.ui.navMode == V_PROBE)
			probeVolume(-1,0,0);
		else
			doAxisRotation(-1.0f,'y');
		break;
    case GLUT_KEY_PAGE_UP: //increase eye sep
		if (global.ui.navMode == V_ROAM)
			roamVolume(0,0,-ROAM);
		else if (global.ui.navMode == V_PROBE)
			probeVolume(0,0,-1);
		else if (global.env.stereoType)
			global.stereo.halfEyeSep +=0.05;
		break;
    case GLUT_KEY_PAGE_DOWN: //decrease eye sep
		if (global.ui.navMode == V_ROAM)
			roamVolume(0,0,ROAM);
		else if (global.ui.navMode == V_PROBE)
			probeVolume(0,0,1);
		else if (global.env.stereoType)
			global.stereo.halfEyeSep -=0.05;
		break;
	}
}

void idle() {
	display();
	QUANTAusleep(100);
}

void reshape(int w, int h)
{
	GlErr("vRenderer","testing");
	glMatrixMode(GL_MODELVIEW);
	global.win.width  = w;
	global.win.height = h;
#ifndef V_DISTRIBUTED
	global.win.aspect = (float)global.win.width/(float)global.win.height;
#endif

#if defined(V_SAGE)
    winWidth = w;
    winHeight = h;
    if (rgbBuffer)
        delete [] rgbBuffer;
    rgbBuffer = new GLubyte[w*h*3];
#endif

	glutPostRedisplay();
}

void parseCmdLine(int argc, char **argv)
{
	CCmdLine cmdLine; // our cmd line parser object
	if (cmdLine.SplitLine(argc, argv) < 1) {
		// no switches were given on the command line, abort
		exit(-1);
	}
	// test for the 'help' case
	if (cmdLine.HasSwitch("-h")) {
		printUsage();
		exit(0);
	}
#if defined(V_NETDATA)
	bool ok = false;
	// remote data - get IP and volume symbolic name
	if (cmdLine.HasSwitch("-v")) {
		//get the symbolic name of the volume
		string volName = cmdLine.GetArgument("-v",0);
		//get the server IP
		string serverIPFile = cmdLine.GetSafeArgument("-v",1,"optiserver.dat");
		//open the file check the server IP for my rank
		char serverIP[256];

#ifdef V_DISTRIBUTED
		strcpy(serverIP,getServerIP(net->getRank(),serverIPFile.c_str()));
#else
		strcpy(serverIP, serverIPFile.c_str());
#endif

		global.volume = new vOptiVolume(volName.c_str(), serverIP);
		if (cmdLine.GetArgumentCount("-v") > 2) {//also have thumbnail volume
			string fullVolName= cmdLine.GetArgument("-v",2);
			global.fullVolume = new vOptiVolume(fullVolName.c_str(), serverIP);
		}
		ok = true;

	}
	if (cmdLine.HasSwitch("-i")) {
		string wild = cmdLine.GetArgument("-i",0);
		cerr <<"wildcard "<<wild<<endl;
		//the numFiles arg is the server IP
		string serverIPFile = cmdLine.GetSafeArgument("-v",1,"optiserver.dat");
		//open the file check the server IP for my rank
		char serverIP[256];

#ifdef V_DISTRIBUTED
		strcpy(serverIP,getServerIP(net->getRank(),serverIPFile.c_str()));
#else
		strcpy(serverIP, serverIPFile.c_str());
#endif

		float scale = 1.0f;
		if (cmdLine.GetArgumentCount("-i") > 2) {//also have thumbnail volume
			string scaleStr = cmdLine.GetArgument("-i",2);
			scale = atof(scaleStr.c_str());
		}

		vGeometry* newGeom = new vGeometry((char*)wild.c_str(),(char*)serverIP,scale);
		renderList.push_back(newGeom);
		ok = true;
	}
	if (!ok) {
		fprintf(stderr,"Specify -v or -i option\n");
		exit(-1);
	}


#else
	//local, specify filename and filedimensions
	try {
		string filename = cmdLine.GetArgument("-f", 0);
		string x, y, z;
		x = cmdLine.GetArgument( "-f", 1);
		y = cmdLine.GetArgument( "-f", 2);
		z = cmdLine.GetArgument( "-f", 3);
		global.volume = new vFileVolume(filename.c_str(),
				atoi(x.c_str()), atoi(y.c_str()), atoi(z.c_str()));
	}
	catch (...)
	{
		// one of the required arguments was missing, abort
		exit(-1);
	}

	//load gradient volume
	if (cmdLine.HasSwitch("-g")) {
		((vFileVolume*)global.volume)->gradientFile = strdup(cmdLine.GetArgument("-g",0).c_str());
	}
#endif
	if (global.volume) {
		//now get the volume offset, if any - 0 by default
		global.volume->offsetX= atoi(cmdLine.GetSafeArgument( "-o", 0, "0").c_str());
		global.volume->offsetY = atoi(cmdLine.GetSafeArgument( "-o", 1, "0").c_str());
		global.volume->offsetZ = atoi(cmdLine.GetSafeArgument( "-o", 2, "0").c_str());
	}

	//get screen width and height
	if (cmdLine.HasSwitch("-w")) {
		global.win.width  = atoi(cmdLine.GetSafeArgument("-w", 0, "512").c_str());
		global.win.height = atoi(cmdLine.GetSafeArgument("-w", 1, "512").c_str());
		global.win.aspect = (float)global.win.width/(float)global.win.height;
	}
	else
		global.env.gameMode = true;

#ifdef V_DISTRIBUTED
	if (cmdLine.HasSwitch("-a")) {
		global.win.aspect = atof(cmdLine.GetSafeArgument("-a", 0, "1.0").c_str());
	}
#endif
	calcFrustum();
	//get the voxel spacing
	if (cmdLine.HasSwitch("-sp")) {
		if (global.volume) {
			//now get the volume offset, if any - 0 by default
			global.volume->spacingX= atof(cmdLine.GetSafeArgument( "-sp", 0, "0").c_str());
			global.volume->spacingY = atof(cmdLine.GetSafeArgument( "-sp", 1, "0").c_str());
			global.volume->spacingZ = atof(cmdLine.GetSafeArgument( "-sp", 2, "0").c_str());
		}
		if (global.fullVolume) {
			//now get the volume offset, if any - 0 by default
			global.fullVolume->spacingX= atof(cmdLine.GetSafeArgument( "-sp", 0, "0").c_str());
			global.fullVolume->spacingY = atof(cmdLine.GetSafeArgument( "-sp", 1, "0").c_str());
			global.fullVolume->spacingZ = atof(cmdLine.GetSafeArgument( "-sp", 2, "0").c_str());
		}
	}

	//get the texture volume dimensions
	if (global.volume) {
		try
		{
			string x = cmdLine.GetArgument("-d", 0);
			string y = cmdLine.GetArgument( "-d", 1);
			string z = cmdLine.GetArgument( "-d", 2);
			global.volume->load(atoi(x.c_str()), atoi(y.c_str()), atoi(z.c_str()));
		}
		catch (...)
		{
			fprintf(stderr,"Missing args to -d command line option\n");
			exit(-1);
		}
	}
	if (global.fullVolume) //load full volume
		global.fullVolume->load();


#if defined(V_DISTRIBUTED)
	char tileConfigFile[256];
	try
	{   // if any of these GetArgument calls fail,
		// we'll end up in the catch() block
		// filename
		strcpy(tileConfigFile, cmdLine.GetArgument("-n", 0).c_str());
	}
	catch (...)
	{
		// one of the required arguments was missing, abort
		exit(-1);
	}
	//init mpi related stuff
	net->readTileConfig(tileConfigFile);
	if (net->isActiveTile()) { //master
		global.env.renderOn = false;
	}
#endif

	global.env.stereoType = (vStereoType)(atoi(cmdLine.GetSafeArgument( "-s", 0, "0").c_str()));
	global.stereo.halfEyeSep = 0.25;
}

