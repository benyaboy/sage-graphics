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

#if defined(V_DISTRIBUTED)
#include "vNet.h"
#include "global.h"
#include "vGeometry.h"
#include <string.h>

char* getServerIP(int rank, const char* serverFile) {
	string line;
	ifstream in(serverFile); // Read
	if (!in) {
		cerr <<"File "<<serverFile<<" cant be read\n";
		return NULL;
	}
	while(getline(in,line)) {
		istringstream line_stream(line);
		int tileRank;
		string serverIP;
		line_stream >> tileRank;
		line_stream >> serverIP;
		if (tileRank == rank)
			return (char*)serverIP.c_str();
	}
	return NULL;
}


QUANTAts_mutex_c tfMutex;
vNet::vNet()
{
	master = 0;
	cmdFlushed = 1;
	readyToDie =0;		//dont want to die right away
}

vNet::~vNet() {
}

void vNet::init(int &argc, char** &argv)
{
	//init MPI
	int rc = MPI_Init(&argc, &argv);
	MPI_Comm comm;
	rc = MPI_Comm_size(MPI_COMM_WORLD, &noProcs);
	rc = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (setenv("DISPLAY", ":0", 1) == -1)
	{
		cerr<<"\nCannot display on Tile "<<rank<<endl;
		exit(1);
	}
}

//Clean up and exit
void vNet::deactivateTiles() {
	fillExitCommand();
}

void vNet::process() {
	if (master != rank)	//slaves
    {
		char* data = new char[COMMAND_SIZE];
		bzero(data, COMMAND_SIZE);
		cmdFlushed = 1;
		//listen from master node for commands
		MPI_Bcast(data, COMMAND_SIZE, MPI_BYTE, master, MPI_COMM_WORLD );
		//fprintf(stderr, "Proc %d/%d> Receiving command\n", rank, noProcs);
		processCmd(data);	//process the command
		glutPostRedisplay();
    }
	else
    { //master
		//Broadcast a command that has been set appropriately
		//fprintf(stderr, "Proc %d/%d> Sending command\n", rank, noProcs);
		tfMutex.lock();
		MPI_Bcast(buffer, COMMAND_SIZE, MPI_BYTE, master, MPI_COMM_WORLD );
		tfMutex.unlock();


		if (readyToDie == 1)
		{
			int rc = MPI_Finalize(); //wait and clean up
			exit(0);         //All dead, We can now exit
		}
		fillDummyCommand(); //just to keep everyone displaying
		//fprintf(stderr, "Proc %d/%d> After Sending command, before display\n", rank, noProcs);
		glutPostRedisplay();
    }
}

void vNet::processCmd(char* data)
{
	//Unpack data - contains command name and arguments
	dataPack.initUnpack(data, COMMAND_SIZE);
	char key[24];
	int len;
	dataPack.unpackInt(&len);   //str length of command
	dataPack.unpack(key, len);
	if (!strcmp(key,"DUMMY"))
	{
		//do nothing - ignore (this is just to synchronize display calls of all tiles with master)
	}
	else if (!strcmp(key,"VOL_ROTN")) {
		//set the global transform rotn to be this one
		for (int i=0;i<16;i++)
			dataPack.unpackFloat(&renderList[global.curPrimIdx]->xform.rotn[i]);
	}
	else if (!strcmp(key,"VOL_CUTROTN")) {
		global.cut.update = true;
		//set the cut rotn to be this one
		dataPack.unpackFloat(&global.cut.angle);
		for (int i=0;i<3;i++)
			dataPack.unpackFloat(&global.cut.axis[i]);
	}
	else if (!strcmp(key,"VOL_LOWRES")) { //use tag to distinguish messages
		//set the global transform lowresrto be this one
		dataPack.unpackInt(&global.volren.lowRes);
	}
	else if (!strcmp(key,"VOL_TRANSLATION")) { //use tag to distinguish messages
		//set the global transform lowresrto be this one
		for (int i=0;i<3;i++)
			dataPack.unpackFloat(&renderList[global.curPrimIdx]->xform.trans[i]);
	}
	else if (!strcmp(key,"VOL_ZOOM")) { //use tag to distinguish messages
		//set the global transform lowresrto be this one
		float zoomFactor;
		dataPack.unpackFloat(&zoomFactor);
		renderList[global.curPrimIdx]->xform.scale += zoomFactor;
		//global.env.eye[2] += zoomFactor;
	}
	else if (!strcmp(key,"VOL_CUTZOOM")) { //use tag to distinguish messages
		//set the global transform lowresrto be this one
		float zoomFactor;
		dataPack.unpackFloat(&zoomFactor);
		global.cut.user[3] += zoomFactor;
	}
	else if (!strcmp(key,"VOL_SAMPLERATE")) { //use tag to distinguish messages
		//set the global transform lowresrto be this one
		float scaleSample;
		dataPack.unpackFloat(&scaleSample);
		global.volren.goodSamp *= scaleSample;
		global.volren.interactSamp *= scaleSample;
		global.volren.sampleRate = global.volren.goodSamp;
	}
	else if (!strcmp(key,"VOL_PRINTFPS")) {
		global.ui.printFPS = !global.ui.printFPS;
		if (global.ui.printFPS)
			glutIdleFunc(idle);
		else
			glutIdleFunc(0);
	}
	else if (!strcmp(key,"VOL_CUTPLANE")) {
		global.ui.cutEnabled = !global.ui.cutEnabled;
	}
	else if (!strcmp(key,"VOL_BOUNDBOX")) {
		global.ui.bboxEnabled = !global.ui.bboxEnabled;
	}
	else if (!strcmp(key,"VOL_NAVROAM")) {
		global.ui.navMode = V_ROAM;
	}
	else if (!strcmp(key,"VOL_NAVPROBE")) {
		global.ui.navMode = V_PROBE;
	}
	else if (!strcmp(key,"VOL_ISOSURFACE")) {
		int isoValue;
		float tx, ty, scale;
		dataPack.unpackInt(&isoValue);
		dataPack.unpackFloat(&tx);
		dataPack.unpackFloat(&ty);
		dataPack.unpackFloat(&scale);
		global.volume->setIsoValue(isoValue);
		NetVertexBuffer* theGeom = global.volume->isoSurface();
		vGeometry* newIso = new vGeometry(theGeom);
		newIso->xform.trans[0] = tx;
		newIso->xform.trans[1] = ty;
		newIso->xform.scale = scale;
		fprintf(stderr,"VOL_ISOSURFACE: New geom translation %f %f scale %f\n",newIso->xform.trans[0],newIso->xform.trans[1],
			newIso->xform.scale);
		renderList.push_back(newIso);
	}
	else if (!strcmp(key,"VOL_LUT")) {
		for (int i=0;i<256*4;i++) {
			dataPack.unpackChar((char*)&global.volren.deptex[i]);
		}
		dataPack.unpackInt(&global.volren.lowRes);
		global.volren.loadTLUT = 1;
	}
	else if (!strcmp(key,"VOL_SAVEGRADIENTVOL")) {
		global.volume->saveGradient();
	}
	else if (!strcmp(key,"VOL_ROAM")) {
		int x, y, z;
		dataPack.unpackInt(&x);
		dataPack.unpackInt(&y);
		dataPack.unpackInt(&z);
		global.volume->roam(x,y,z);
		global.volren.loadVolume = 1;
	}
	else if (!strcmp(key,"VOL_PROBE")) {
		int x, y, z;
		dataPack.unpackInt(&x);
		dataPack.unpackInt(&y);
		dataPack.unpackInt(&z);
		global.volume->probe(x,y,z);
	}
	/*
	else if (!strcmp(key,"VOL_POINTERPOS")) {
		dataPack.unpackFloat(&global.pointerPos[0]);
		dataPack.unpackFloat(&global.pointerPos[1]);
		dataPack.unpackFloat(&global.pointerPos[2]);
		global.curPrimIdx= 0;
		//select each primitive to see which is selected
		for (int i=0;i< renderList.size();i++) {
			if (renderList[i]->select()) {
				global.curPrimIdx= i;
				break;
			}
		}
	}
	*/
	else if (!strcmp(key,"VOL_SELECT")) {
		dataPack.unpackInt(&global.curPrimIdx);
		for (int i=0;i<renderList.size();i++) {
			if (i==global.curPrimIdx)
				renderList[i]->setSelected(true);
			else
				renderList[i]->setSelected(false);
		}
	}
	else if (!strcmp(key,"VOL_TOGGLEANIMATE")) {
		for (int i=0;i<renderList.size();i++)
			renderList[i]->toggleAnimate();
	}
	else if (!strcmp(key,"VOL_UPDATEANIM")) {
		for (int i=0;i<renderList.size();i++) {
			if (renderList[i]->isAnimating()) {
				renderList[i]->next();
			}
		}
	}
	else if (!strcmp(key,"EXIT"))
	{
		int rc = MPI_Finalize(); //clean up mpi stuff
		if (data) delete data;   //be responsible
		deallocateAll();
		exit(0);
	}
	if (data) delete data;
}

// Called when tile needs to be refreshed
bool vNet::readTileConfig(const char* configFile) {
	cerr << rank<<" readTileConfig : filename is "<<configFile<<endl;
	string line;
	ifstream in(configFile); // Read
	if (!in) {
		cerr <<"File "<<configFile<<" cant be read\n";
		return false;
	}
	global.env.frustumList.clear();
	global.env.viewportList.clear();
	while(getline(in,line)) {
		istringstream line_stream(line);
		int tileRank;
		float	minX, maxX, minY, maxY;
		float	vpx, vpy, vpwidth, vpheight;
		line_stream >> tileRank;
		//read the tile extent
		line_stream >> minX;
		line_stream >> maxX;
		line_stream >> minY;
		line_stream >> maxY;

		//read the viewport for each frustum
		if (line_stream.eof()) { //fill in default values
			vpx = vpy = 0.0;
			vpwidth = vpheight = 1.0f;
		}
		else {
			line_stream >> vpx;
			line_stream >> vpy;
			line_stream >> vpwidth;
			line_stream >> vpheight;
		}

		if (tileRank == rank)  //if the rank matches, store the values
		{
			//fprintf(stderr, "\nTile %d Image offsets: %d %d ", rank, xOffset, yOffset);
			vFrustum curFrustum;
			vViewport curvp;
			curFrustum.left = (minX - 50.0) / 50.0;
			curFrustum.right = (maxX - 50.0) / 50.0;
			curFrustum.bottom = (minY - 50.0) / 50.0;
			curFrustum.top = (maxY - 50.0) / 50.0;
			float fov = 45.0f*M_PI/180.0f;
			float b = global.env.clip[0] * (float) tan(fov * 0.5);
			fprintf(stderr,"Node %d aspect %f\n",rank,global.win.aspect);
			float r = b * global.win.aspect;
			curFrustum.left *= r;  //left
			curFrustum.right *= r;   //right
			curFrustum.bottom *= b;  //bottom
			curFrustum.top *= b;   //top

			//now add the current frustum to the frustum list
			global.env.frustumList.push_back(curFrustum);

			curvp.x = vpx;
			curvp.y = vpy;
			curvp.width = vpwidth;
			curvp.height = vpheight;
			global.env.viewportList.push_back(curvp);
			fprintf(stderr,"*******viewport %f %f %f %f\n",curvp.x, curvp.y, curvp.width, curvp.height);
		}
	}  //end while
	return true;
}
void vNet::packCommand(const char* cmdString, int flushed)
{
	memset(buffer, 0, COMMAND_SIZE);
	dataPack.initPack(buffer, COMMAND_SIZE);
	char cmd[25];
	strcpy(cmd, cmdString);
	int len = strlen(cmd) +1;
	dataPack.packInt(len);
	dataPack.pack(cmd, len );
	cmdFlushed = flushed;
	//the command is sent during the display call
}

//send vol rotation
void vNet::setRotn(float rotn[16]) {
	packCommand("VOL_ROTN");
	for (int i=0;i<16;i++)
		dataPack.packFloat(rotn[i]);
}

//send the transfer function
void vNet::setLUT(int size, unsigned char* data) {
	tfMutex.lock();
	packCommand("VOL_LUT");
	for (int i=0;i<size;i++)
		dataPack.packChar(data[i]);
	tfMutex.unlock();
}

//send the cutplane rotation in axis angle
void vNet::setCutRotation(float angle, float axis[3]) {
	packCommand("VOL_CUTROTN");
	dataPack.packFloat(angle);
	for (int i=0;i<3;i++)
		dataPack.packFloat(axis[i]);
}

//set the zoom of the cutplane
void vNet::setCutZoom(float dzoom) {
	packCommand("VOL_CUTZOOM");
	dataPack.packFloat(dzoom);
}

//send lowres
void vNet::setLowRes(int lowRes) {
	packCommand("VOL_LOWRES");
	dataPack.packInt(lowRes);
	//fprintf(stderr,"In setLowRes: lowres %d\n",lowRes);
}

//save GRadient Info
void vNet::saveGradientVolume() {
	 packCommand("VOL_SAVEGRADIENTVOL");
}

//do select
void vNet::select(float pointerPos[3]) {
	packCommand("VOL_POINTERPOS");
	for (int i=0;i<3;i++)
		dataPack.packFloat(pointerPos[i]);
}

//do select
void vNet::select(int index) {
	packCommand("VOL_SELECT");
	dataPack.packInt(index);
}

//set translation
void vNet::setTranslation(float translation[3]) {
	packCommand("VOL_TRANSLATION");
	for (int i=0;i<3;i++)
		dataPack.packFloat(translation[i]);
}

//set zoom
void vNet::setZoom(float factor) {
	packCommand("VOL_ZOOM");
	dataPack.packFloat(factor);
}

//scale sample rate
void vNet::scaleSampleRate(float factor) {
	packCommand("VOL_SAMPLERATE");
	dataPack.packFloat(factor);
}

//animate volume
void vNet::animateVolume() {
	packCommand("VOL_TOGGLEANIMATE");
}

//animate volume
void vNet::updateAnim() {
	packCommand("VOL_UPDATEANIM");
}


//toggle printing frame rate
void vNet::togglePrintFPS() {
	packCommand("VOL_PRINTFPS");
}

//toggle printing frame rate
void vNet::toggleCutPlane() {
	packCommand("VOL_CUTPLANE");
}

//toggle display of bound box
void vNet::toggleBoundBox() {
	packCommand("VOL_BOUNDBOX");
}

//set roam
void vNet::setRoam() {
	packCommand("VOL_NAVROAM");
}

//set probe
void vNet::setProbe() {
	packCommand("VOL_NAVPROBE");
}

//send the roam factor
void vNet::roamVolume(int roamX, int roamY, int roamZ) {
	packCommand("VOL_ROAM");
	dataPack.packInt(roamX);
	dataPack.packInt(roamY);
	dataPack.packInt(roamZ);
}

//send the roam factor
void vNet::probeVolume(int probeX, int probeY, int probeZ) {
	packCommand("VOL_PROBE");
	dataPack.packInt(probeX);
	dataPack.packInt(probeY);
	dataPack.packInt(probeZ);

}

//send command to get the isosurface and display it
void vNet::getIsosurface(int isoValue, float tx, float ty, float scale) {
	packCommand("VOL_ISOSURFACE");
	dataPack.packInt(isoValue);
	dataPack.packFloat(tx);
	dataPack.packFloat(ty);
	dataPack.packFloat(scale);

}

//send command to get the isopoint and display it
void vNet::getIsopoint(int isoValue, float tx, float ty, float scale) {
	packCommand("VOL_ISOPOINT");
	dataPack.packInt(isoValue);
	dataPack.packFloat(tx);
	dataPack.packFloat(ty);
	dataPack.packFloat(scale);

}

//send empty broadcasts to clients
void vNet::fillDummyCommand()
{
	packCommand("DUMMY", 1);
}

//fill up a command to do MPI_Finalize (clean up MPI)
void vNet::fillExitCommand()
{
	packCommand("EXIT");
	readyToDie =1;		//set flag -ready to exit now
}


#endif
