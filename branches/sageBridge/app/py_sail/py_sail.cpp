/*****************************************************************************************
 * py_sail: A library that allows you to use SAIL from python (ie write SAGE apps with python)
 *
 * Copyright (C) 2007 Electronic Visualization Laboratory,
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
 * Direct questions, comments etc about py_sail to www.evl.uic.edu/cavern/forum
 *
 * Author: Ratko Jagodic
 *
 *****************************************************************************************/


#include <Python.h>
#include <iostream>

// sage headers
#include <sail.h>
#include <misc.h>



using namespace std;


// SAGE Stuff
sail sageInf; // sail object
sailConfig scfg;
sageRect renderImageMap;
unsigned char *buffer = NULL;


// prototypes
static PyObject* initSail(PyObject *self, PyObject *args);
static PyObject* setBuffer(PyObject *self, PyObject *args);
static PyObject* checkMessages(PyObject *self, PyObject *args);
PyMODINIT_FUNC initpy_sail(void);



//-----------------------------------------------------------------------



// all the methods callable from Python have to be
// listed here
static PyMethodDef PySailMethods[] = {
    {"initSail",  initSail, METH_VARARGS,
     "Initialize Sail Python interface."},

    {"setBuffer", setBuffer, METH_VARARGS,
     "Receives the buffer from python."},

    {"checkMessages", checkMessages, METH_VARARGS,
     "Checks for new sail messages from SAGE."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};



// this function is called when we do import py_sail from python
// it let's the python interpreter know about all the functions
// in this module available for calling
PyMODINIT_FUNC initpy_sail(void)
{
    (void) Py_InitModule("py_sail", PySailMethods);
}



//-----------------------------------------------------------------------



// initializes the sail object from python
static PyObject* initSail(PyObject *self, PyObject *args)
{
    char *appName;
    char confName[128];
    int width, height;

    // parse the arguments
    if (!PyArg_ParseTuple(args, "sii", &appName, &width, &height))
        return NULL;

    // initialize sail and sage stuff
    renderImageMap.left = 0.0;
    renderImageMap.right = 1.0;
    renderImageMap.bottom = 0.0;
    renderImageMap.top = 1.0;
    scfg.imageMap = renderImageMap;

    // the config name should be "appName.conf"
    strcpy(confName, appName);
    strcat(confName, ".conf");
    fprintf(stderr, "\n\nSAIL CONFIG FILE BEING USED: %s", confName);

    scfg.init(confName);
    scfg.setAppName(appName);  // case sensitive
    scfg.rank = 0;

    scfg.resX = width;
    scfg.resY = height;

    scfg.pixFmt = PIXFMT_888;
    scfg.rowOrd = TOP_TO_BOTTOM;
    scfg.master = true;

    sageInf.init(scfg);
    // return none
    Py_INCREF(Py_None);
    return Py_None;
}




// receives the buffer and swaps it via sail object
static PyObject* setBuffer(PyObject *self, PyObject *args)
{
    int bufSize;
    unsigned char *pybuf = NULL;

    // parse the arguments
    if (!PyArg_ParseTuple(args, "s#", &pybuf, &bufSize))
    {
      cout << "py_sail.so:  ParseTuple failed..." << endl;
      return NULL;
    }

    // if something went wrong so just return None
    if (pybuf == NULL)
      {
	cout << "\n\npy_sail.so:  Buffer received from Python is NULL... returning without swapBuffer\n\n";
	Py_INCREF(Py_None);
	return Py_None;
      }

    // finally, swap the buffer via sail
    buffer = (unsigned char *) sageInf.getBuffer();  // get the new buffer from sage first
    memcpy(buffer, pybuf, scfg.resX*scfg.resY*3);
    sageInf.swapBuffer();

    // this function returns None
    Py_INCREF(Py_None);
    return Py_None;
}



/* checks for messages sent from SAGE
 defined messages and their return values (to the python program):

 APP_QUIT    -1

*/
static PyObject* checkMessages(PyObject *self, PyObject *args)
{
  sageMessage msg;
  int returnVal = 0;
  if (sageInf.checkMsg(msg, false) > 0)
    {
      switch (msg.getCode()) {
	  case APP_QUIT : {
	    returnVal = -1;
	    break;
	  }
       }
    }

  if (returnVal == 0)
    {
      Py_INCREF(Py_None);
      return Py_None;
    }
  else
    return Py_BuildValue("i", returnVal);
}
