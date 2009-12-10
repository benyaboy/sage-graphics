#!/usr/bin/python
############################################################################
#
# py_sail: An example of how to write SAGE applications using python and py_sail
#
# Copyright (C) 2007 Electronic Visualization Laboratory,
# University of Illinois at Chicago
#
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following disclaimer
#    in the documentation and/or other materials provided with the distribution.
#  * Neither the name of the University of Illinois at Chicago nor
#    the names of its contributors may be used to endorse or promote
#    products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author: Ratko Jagodic
#
############################################################################


import os, sys, time, array


# for importing py_sail properly
sys.path.append(os.environ["SAGE_DIRECTORY"]+"/lib")
import py_sail


# size of SAGE buffer for this app
BUFFER_WIDTH = 200
BUFFER_HEIGHT = 200


# initialize sail with appName, buffer width and height
py_sail.initSail("pysail_test", BUFFER_WIDTH, BUFFER_HEIGHT)


# make a pixel array of size BUFFER_WIDTH * BUFFER_HEIGHT * 3
a = array.array('B')            # unsigned char type
numPixels = BUFFER_WIDTH * BUFFER_HEIGHT
for i in xrange(0, BUFFER_WIDTH * BUFFER_HEIGHT):
    p = 1-i/float(numPixels)
    a.append(int(p*255)); a.append(int(p*255)); a.append(int(p*255))  #R,G,B


# convert the array to a string and send it to py_sail
buf = a.tostring()
py_sail.setBuffer(buf)


# keep looping and checking for the quit message
# you could also swap buffers here if you need 
# to update the image
while True:
    time.sleep(0.5)
    msg = py_sail.checkMessages()
    if msg == -1:    # -1 is returned when SAGE wants to close the app
        print "quitting..."
        break
