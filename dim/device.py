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




import events
from globals import *


# for smoothing of input data
SMOOTHING_BUFFER_SIZE = 10   # how many previous values do we take into account?
SMOOTHING_MODIFIER    = 0.2  # the amount of smoothing, and delay (0=none, 1=tons)



class Device:
    """ events is a list of eventIds that this device can generate """

    def __init__(self, deviceType, deviceId, needPointer, displayId=0):
        self.deviceType = deviceType
        self.deviceId = deviceId
        self.displayId = displayId
        self.__smoothingBuffer = []  # history of previous values (tuples of (x,y))
        
        # set the bounds
        di = getSageData().getDisplayInfo(self.displayId)
        self.bounds = Bounds(0, di.sageW, di.sageH, 0)
        self.displayBounds = self.bounds   # first bounds are always equal to the display size
        #self.globalMode = True  # False if in appMode... ie the events are forwarded to the app
        self.toEvtHandler = None  # all our the events should go to this handler (ie device captured by the evtHandler)
        self.lastMoveCallback = None  # this is used for generating EVT_LEFT_WINDOW and EVT_ENTERED_WINDOW
        self.dragMode = False     # an overlay can request this device to produce drag event for a bit

        # this is for app specific stuff such as magic carpet lenses
        # these devices send special events
        self.specialDevice = False   
        self.specialId = 0        
        
        self.evtMgr = getEvtMgr()  # from globals
        self.overlayMgr = getOverlayMgr()  # from globals

        # we create a pointer if needed
        self.pointer = None   
        if needPointer:
            self.overlayMgr.addOverlay(OVERLAY_POINTER, self.onPointerAdded, displayId=self.displayId)


    def destroy(self):
        if self.pointer:
            self.overlayMgr.removeOverlay(self.pointer.overlayId)


    def onPointerAdded(self, pointerOverlay):
        self.pointer = pointerOverlay


    ## def changeMode(self, toGlobal, newBounds):
##         """ Changes the current mode for this device.
##             toGlobal=False for appMode, True for globalMode
##             You need to pass the new bounds in as well. If changing
##             to appMode, pass in the appBounds in SAGE coords, otherwise
##             pass in the SAGE display bounds.
##         """
    
##         self.bounds = newBounds
##         self.globalMode = toGlobal

##         # now convert the position to those new bounds
##         self.x = float(self.x - newBounds.left) / newBounds.getWidth()
##         self.y = float(self.y - newBounds.bottom) / newBounds.getHeight()

        
        
    def onMessage(self, data, firstMsg=False):
        """ This method gets called when a message arrives from the HWCapture
            for this device. In here you should provide the conversion from
            HW events to the SAGE App Events. FirstMsg just signifies
            that this is the very first message right after the device creation.
            
            Must be implemented by the subclass.
        """
        
        raise NotImplementedError


    def setSpecialId(self, newId):
        self.specialId = newId


    def getSpecialId(self):
        """ When using special devices you need to specify which deviceId will be reported """
        return self.specialId
    

    ### these are the methods to call when an event occurs
    ### this will pass the event on to the system to be handled
    
    def postEvtMove(self, newX, newY, dX, dY):
        evt = events.MoveEvent(self, newX, newY, dX, dY, self.toEvtHandler)
        self.evtMgr.postEvent( evt )
            

    def postEvtAnalog1(self, x, y, dX, dY, dZ):
        evt = events.Analog1Event(self, x, y, dX, dY, dZ, self.toEvtHandler)
        self.evtMgr.postEvent( evt )


    def postEvtAnalog2(self, x, y, dX, dY, dZ):
        evt = events.Analog2Event(self, x, y, dX, dY, dZ, self.toEvtHandler)
        self.evtMgr.postEvent( evt )


    def postEvtAnalog3(self, x, y, dX, dY, dZ):
        evt = events.Analog3Event(self, x, y, dX, dY, dZ, self.toEvtHandler)
        self.evtMgr.postEvent( evt )

    
    def postEvtClick(self, x, y, btnId, isDown, forEvt):
        evt = events.ClickEvent(self, x, y, btnId, isDown, forEvt, self.toEvtHandler)
        self.evtMgr.postEvent( evt )
        if not isDown:
            self.toEvtHandler = None  # not captured anymore


    def postEvtArrow(self, arrow, x, y):
        evt = events.ArrowEvent(self, arrow, x, y, self.toEvtHandler)
        self.evtMgr.postEvent( evt )


    def postEvtKey(self, key):
        evt = events.KeyEvent(self, key, self.toEvtHandler)
        self.evtMgr.postEvent( evt )


    def postEvtCustom(self, data):
        evt = events.CustomEvent(self, data, self.toEvtHandler)
        self.evtMgr.postEvent( evt )
        



    ### should be called by the subclass if smoothing is desired for every frame (new input)
    ### takes in two new values for x and y and returns smoothed values for x and y
    def smooth(self, newX, newY, bufSize=SMOOTHING_BUFFER_SIZE, smoothAmount=SMOOTHING_MODIFIER):
        self.__smoothingBuffer.insert(0, (newX, newY))   # insert the newest

        if len(self.__smoothingBuffer) > bufSize:
            del self.__smoothingBuffer[ len(self.__smoothingBuffer)-1 ]  # delete the oldest

        # do the non-linear averaging
        totX, totY, totMod = 0.0, 0.0, 0.0
        mod = 1.0 
        for i in range(0, len(self.__smoothingBuffer)):
            totX += self.__smoothingBuffer[i][0]*mod
            totY += self.__smoothingBuffer[i][1]*mod
            totMod += mod
            mod = mod*smoothAmount

        smoothX = int(round(totX / totMod))
        smoothY = int(round(totY / totMod))

        # record the new value
        self.__smoothingBuffer[0] = (smoothX, smoothY)
                
        return smoothX, smoothY
        
