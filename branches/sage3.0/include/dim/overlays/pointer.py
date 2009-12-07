############################################################################
#
# DIM - A Direct Interaction Manager for SAGE
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
# Direct questions, comments etc about SAGE UI to www.evl.uic.edu/cavern/forum
#
# Author: Ratko Jagodic
#        
############################################################################




from overlay import Overlay
from globals import *



def makeNew(overlayId):
    """ this is how we instantiate the device object from
        the main program that loads this plugin """
    return Pointer(overlayId)



# pointer states
RESET  = 0  # normal
MOVE   = 1
DRAG   = 2
RESIZE = 3
DOWN   = 4
UP     = 5
IN_APP = 6  # just change the color
ANGLE  = 7  # where the down side of the cursor is
ZOOM   = 8
ROTATE = 9



class Pointer(Overlay):

    def __init__(self, overlayId):
        Overlay.__init__(self, OVERLAY_POINTER, overlayId)
        self.state = RESET
        self.lastOrientation = 0
        self.inApp = False
        self.__savedState = [self.state, self.lastOrientation, self.inApp]
        
        # some devices only produce one analog event so set it here if so
        # (e.g. a zoom-only puck)
        # this is used when the device enters an application in order to show
        # the correct pointer shape describing what the device can do
        self.onlyAnalogEvent = None   


    def pushState(self):
        self.__savedState = [self.state, self.lastOrientation, self.inApp]


    def popState(self):
        state, lastOrientation, inApp = self.__savedState

        # apply the state
        if self.state != state and state != RESIZE:
            self.sendOverlayMessage(state)
            self.state = state

        # apply the orientation
        if self.state != state and state == RESIZE:
            self.showResizePointer(self.lastOrientation)

        # apply the inApp modifier
        if inApp: self.showInApp()
        else: self.showOutApp()
        

    def setAnalogEvent(self, eventId):
        self.onlyAnalogEvent = eventId
        
        
    def resetPointer(self):
        if self.state != RESET:
            self.sendOverlayMessage(RESET)
            self.state = RESET
            self.lastOrientation = 0
            #print "RESET"
            #self.inApp = False
            

    def showDownPointer(self):
        if self.state != DOWN:
            self.sendOverlayMessage(DOWN)
            self.state = DOWN


    def showUpPointer(self):
        if self.state != UP:
            self.sendOverlayMessage(UP)
            self.state = UP
            

    def showResizePointer(self, orientation):
        if self.state != RESIZE or self.lastOrientation != orientation:
            self.sendOverlayMessage(RESIZE, orientation)
            self.state = RESIZE
            self.lastOrientation = orientation


    def showDragPointer(self):
        if self.state != DRAG:
            self.sendOverlayMessage(DRAG)
            self.state = DRAG
            #print "DRAG"


    def showZoomPointer(self):
        if self.state != ZOOM:
            self.sendOverlayMessage(ZOOM)
            self.state = ZOOM


    def showRotatePointer(self):
        if self.state != ROTATE:
            self.sendOverlayMessage(ROTATE)
            self.state = ROTATE


    def movePointer(self, x, y):
        self.sendOverlayMessage(MOVE, x, y)


    def pointerAngle(self, angle):
        self.sendOverlayMessage(ANGLE, int(angle))


    def showInApp(self):
        if not self.inApp:
            self.sendOverlayMessage(IN_APP, 1)
            self.inApp = True
            #print "IN APP"
            # set the correct pointer shape if the device
            # can produce only one analog event
            if self.onlyAnalogEvent == EVT_DRAG:
                self.showDragPointer()
            elif self.onlyAnalogEvent == EVT_ROTATE:
                self.showRotatePointer()
            elif self.onlyAnalogEvent == EVT_ZOOM:
                self.showZoomPointer()
            elif self.onlyAnalogEvent == None:
                self.resetPointer()


    def showOutApp(self):
        if self.inApp:
            #print "OUT APP"
            self.sendOverlayMessage(IN_APP, 0)
            self.inApp = False
            #self.resetPointer()
            

