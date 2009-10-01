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
from eventHandler import EventHandler
from globals import *
from math import cos, sin, radians


def makeNew(overlayId, app):
    """ this is how we instantiate the device object from
        the main program that loads this plugin """
    return App(overlayId, app)



# pointer states
RESET  = 0
DRAG   = 1
RESIZE = 2
DOWN   = 3
UP     = 4

Z_CHANGE = 3
ON_CORNER = 4

# corner orientations
APP          = 0
APP_BOTTOM   = -2
APP_TOP      = -1
BOTTOM_LEFT  = 1
TOP_LEFT     = 2
TOP_RIGHT    = 3
BOTTOM_RIGHT = 4



class App(Overlay, EventHandler):
    
    def __init__(self, overlayId, app):
        Overlay.__init__(self, OVERLAY_APP, overlayId)
        EventHandler.__init__(self)
        self.app = app   # which app is this overlay tied to
        self.bounds.setAll(app.left, app.right, app.top, app.bottom)
        self.displayId = app.getDisplayId()
        self.aspectRatio = self.bounds.getWidth() / float(self.bounds.getHeight())
        self.minAppSize = 300   # min app size in either direction
        self.z = self.app.getZvalue()
        self.__resetOverlay()
        self.capturedCorner = APP
        self.lastClickPos = (0,0)
        self.lastClickDeviceId = None
        self.overCorner = APP
                
        # register for the events that are fired when an app changes
        self.registerForEvent(EVT_APP_INFO, self.__onAppChanged)
        self.registerForEvent(EVT_Z_CHANGE, self.__onZChanged)

        # register for the events that are fired by devices
        self.registerForEvent(EVT_MOVE, self.__onOver)
        self.registerForEvent(EVT_CLICK, self.__onClick)
        self.registerForEvent(EVT_DRAG, self.__onDrag)
        self.registerForEvent(EVT_ROTATE, self.__onRotate)
        self.registerForEvent(EVT_ZOOM, self.__onZoom)
        self.registerForEvent(EVT_ARROW, self.__onArrow)
        self.registerForEvent(EVT_LEFT_WINDOW, self.__onLeftWindow)

        # register for the events from the special devices
        self.registerForEvent(EVT_MOVE_SPECIAL, self.__onSpecialMove)
        self.registerForEvent(EVT_ENTERED_WINDOW_SPECIAL, self.__onEnteredWindowSpecial)
        self.registerForEvent(EVT_LEFT_WINDOW_SPECIAL, self.__onLeftWindowSpecial)


    # -------    DENERIC  EVENTS   -----------------    

    def __onAppChanged(self, event):
        if self.app == event.app:
            app = event.app
            self.bounds.setAll(app.left, app.right, app.top, app.bottom)
            self.aspectRatio = self.bounds.getWidth() / float(self.bounds.getHeight())
            self.displayId = app.getDisplayId()
            self.__resetOverlay()
                        

    def __onZChanged(self, event):
        self.z = self.app.getZvalue()
        self.sendOverlayMessage(Z_CHANGE, self.z)


    def __onLeftWindow(self, event):
        # change the app overlay to default
        if self.overCorner != APP:
            self.sendOverlayMessage(ON_CORNER, self.overCorner, 0)
            self.overCorner = APP

        # change the pointer to default
        pointer = event.device.pointer
        event.device.dragMode = False
        if pointer:
            pointer.showOutApp()
            pointer.resetPointer()



    # -------    SPECIAL DEVICE EVENTS   -----------------

    def __onSpecialMove(self, event):
        x,y = self.__toAppCoords(event.x, event.y)
        dx,dy = self.__toAppDist(event.dX, event.dY)
        self.sageGate.sendAppEvent(EVT_MOVE, self.app.getSailID(), event.device.getSpecialId(), x, y, dx, dy)


    def __onEnteredWindowSpecial(self, event):
        pointer = event.device.pointer
        if pointer: pointer.hide()


    def __onLeftWindowSpecial(self, event):
        pointer = event.device.pointer
        if pointer: pointer.show()



    # -------    DEVICE EVENTS   -----------------
    
    def __onOver(self, event):
        corner = self.app.hitTest(event.x, event.y, self.cornerSize)
        device = event.device
        pointer = device.pointer
        if pointer:
            if corner > APP:    # corners
                pointer.showResizePointer(corner)
                pointer.showOutApp()
                device.dragMode = True
                if self.overCorner != corner:
                    self.sendOverlayMessage(ON_CORNER, corner, 1)
                    self.sendOverlayMessage(ON_CORNER, self.overCorner, 0)
                    self.overCorner = corner
                    
            elif corner < APP:   # top and bottom area
                pointer.showDragPointer()
                pointer.showOutApp()
                device.dragMode = True
                if self.overCorner != corner:
                    self.sendOverlayMessage(ON_CORNER, corner, 1)
                    self.sendOverlayMessage(ON_CORNER, self.overCorner, 0)
                    self.overCorner = corner

            else:      # interior of the app
                #pointer.resetPointer()
                pointer.showInApp()
                device.dragMode = False
                if self.overCorner != corner:
                    self.sendOverlayMessage(ON_CORNER, self.overCorner, 0)
                    self.overCorner = corner


                
    def __onClick(self, event):
        corner = self.app.hitTest(event.x, event.y, self.cornerSize)
        pointer = event.device.pointer
        x, y = event.x, event.y

        # BUTTON DOWN
        if event.isDown:
            self.capturedCorner = corner
            self.lastClickPos = (x, y)
            event.device.toEvtHandler = self   # tell the device to forward all events here (until the button goes up)
            if pointer: pointer.pushState()    # preserve the state of the pointer
            
            if corner < APP:  # top or bottom
                self.captured = True
                self.lastClickDeviceId = event.device.deviceId
                if pointer: pointer.showDragPointer()
                self.sendOverlayMessage(DRAG, 0,0, corner)
                self.sageGate.bringToFront(self.app.getId())

            elif corner > APP:  # corner
                self.captured = True
                self.lastClickDeviceId = event.device.deviceId
                if pointer: pointer.showResizePointer(corner)
                self.sendOverlayMessage(RESIZE, 0,0,corner)

            else:         # to app
                x,y = self.__toAppCoords(x,y)
                self.sageGate.sendAppEvent(EVT_CLICK, self.app.getSailID(), -1, x, y, event.btnId, 1, event.forEvt)


        # BUTTON UP
        elif not event.isDown:
            self.captured = False
            dX = x - self.lastClickPos[0]
            dY = y - self.lastClickPos[1]
            if pointer: pointer.popState()   # return to the previous pointer state
                
            if self.capturedCorner < APP:    # top/bottom
                if event.device.deviceId == self.lastClickDeviceId:
                    self.lastClickDeviceId = None
                    self.sageGate.moveWindow(self.app.getId(), dX, dY)

            elif self.capturedCorner > APP:  # corners
                if event.device.deviceId == self.lastClickDeviceId:
                    self.lastClickDeviceId = None
                    self.__resizeBounds(dX, self.capturedCorner)
                    self.sageGate.resizeWindow(self.app.getId(), self.bounds.left,
                                               self.bounds.right, self.bounds.bottom,
                                               self.bounds.top)
            else:    # to app
                x,y = self.__toAppCoords(x,y)
                self.sageGate.sendAppEvent(EVT_CLICK, self.app.getSailID(), -1, x, y, event.btnId, 0, event.forEvt)


    def __onDrag(self, event):
        x,y = event.x, event.y
        dx, dy, dz = event.dX, event.dY, event.dZ
        pointer = event.device.pointer
        
        if self.capturedCorner < APP:  # top or bottom
            self.sendOverlayMessage(DRAG, dx, dy, self.capturedCorner)

        elif self.capturedCorner > APP:       # corner
            self.sendOverlayMessage(RESIZE, dx, dy, self.capturedCorner)

        else:               # to app
            x,y = self.__toAppCoords(x,y)
            dx,dy = self.__toAppDist(dx, dy)
            self.sageGate.sendAppEvent(EVT_PAN, self.app.getSailID(), -1, x, y, dx, dy, dz)
            if pointer: pointer.showDragPointer()


    def __onRotate(self, event):
        dx, dy, dz = event.dX, event.dY, event.dZ
        pointer = event.device.pointer
        
        if self.capturedCorner == APP:  # to app
            x,y = self.__toAppCoords(event.x, event.y)
            dx,dy = self.__toAppDist(dx, dy)
            self.sageGate.sendAppEvent(EVT_ROTATE, self.app.getSailID(), -1, x, y, dx, dy, dz)
            if pointer: pointer.showRotatePointer()


    def __onZoom(self, event):
        x,y = event.x, event.y
        dx, dy, dz = event.dX, event.dY, event.dZ
        pointer = event.device.pointer
        
        if self.capturedCorner == APP:  # to app
            x,y = self.__toAppCoords(x,y)
            dx,dy = self.__toAppDist(dx, dy)
            self.sageGate.sendAppEvent(EVT_ZOOM, self.app.getSailID(), -1, x, y, dx, dy, dz)
            if pointer: pointer.showZoomPointer()

            
    def __onArrow(self, event):
        self.sageGate.sendAppEvent(EVT_ARROW, self.app.getSailID(), -1, event.arrow)




    # -------    HELPER METHODS   -----------------
    

    def __resetOverlay(self):
        self.__setCornerSize()
        self.sendOverlayMessage(RESET, self.bounds.left,
                                self.bounds.right, self.bounds.top,
                                self.bounds.bottom, self.z,
                                self.cornerSize, self.displayId)
        self.overCorner = APP

    
    def __toAppCoords(self, x, y):
        """ converts to the normalized coords with respect to the app window
            accounts for orientation of the sage window
        """
        orientation = self.app.getOrientation()
        
        if orientation == 0:
            x -= self.bounds.left
            y -= self.bounds.bottom
            normX = float(x)/self.bounds.getWidth()
            normY = float(y)/self.bounds.getHeight()
        elif orientation == 180:
            x = self.bounds.right - x
            y = self.bounds.top - y
            normX = float(x)/self.bounds.getWidth()
            normY = float(y)/self.bounds.getHeight()
        elif orientation == 90:
            tmp = y
            y = self.bounds.right - x
            x = tmp - self.bounds.bottom
            normX = float(x)/self.bounds.getHeight()
            normY = float(y)/self.bounds.getWidth()
            
        elif orientation == 270:
            tmp = y
            y = x - self.bounds.left
            x = self.bounds.top - tmp
            normX = float(x)/self.bounds.getHeight()
            normY = float(y)/self.bounds.getWidth()
            
        return normX, normY

            
    def __toAppDist(self, dx, dy):
        """ converts to the normalized coords with respect to the app window
            accounts for orientation of the sage window
        """
        orientation = self.app.getOrientation()

        if orientation == 0:
            normX = float(dx)/self.bounds.getWidth()
            normY = float(dy)/self.bounds.getHeight()
        elif orientation == 180:
            dx = -dx
            dy = -dy
            normX = float(dx)/self.bounds.getWidth()
            normY = float(dy)/self.bounds.getHeight()
        elif orientation == 90:
            tmp = dy
            dy = -dx
            dx = tmp
            normX = float(dx)/self.bounds.getHeight()
            normY = float(dy)/self.bounds.getWidth()
        elif orientation == 270:
            tmp = dy
            dy = dx
            dx = -tmp
            normX = float(dx)/self.bounds.getHeight()
            normY = float(dy)/self.bounds.getWidth()
            
        #normX = float(dx)/self.bounds.getWidth()
        #normY = float(dy)/self.bounds.getHeight()

        return normX, normY

        #return float(dx)/self.bounds.getWidth(), float(dy)/self.bounds.getHeight()
            

    def __setCornerSize(self):
        maxSize = 400
        minSize = 150
        self.cornerSize = min(maxSize, max(minSize, int(min(self.bounds.getHeight(),
                                                            self.bounds.getWidth()) / 8)))

        
    def __resizeBounds(self, dx, corner):
        # change the bounds based on the amount moved since the click
        # also, don't resize smaller than the minAppSize
        (l,r,t,b) = self.bounds.getAll()
        
        if corner == BOTTOM_LEFT:
            l+=dx; b=b+dx/self.aspectRatio
            if r-l < self.minAppSize and (r-l) <= (t-b):
                l = r - self.minAppSize
                b = t - self.minAppSize / self.aspectRatio
            elif t-b < self.minAppSize and (t-b) < (r-l):
                b = t - self.minAppSize
                l = r - self.minAppSize * self.aspectRatio
        elif corner == TOP_LEFT:
            l+=dx; t=t-dx/self.aspectRatio 
            if r-l < self.minAppSize and (r-l) <= (t-b):
                l = r - self.minAppSize
                t = b + self.minAppSize / self.aspectRatio
            elif t-b < self.minAppSize and (t-b) < (r-l):
                t = b + self.minAppSize
                l = r - self.minAppSize * self.aspectRatio
        elif corner == TOP_RIGHT:
            r+=dx; t=t+dx/self.aspectRatio
            if r-l < self.minAppSize and (r-l) <= (t-b):
                r = l + self.minAppSize
                t = b + self.minAppSize / self.aspectRatio
            elif t-b < self.minAppSize and (t-b) < (r-l):
                t = b + self.minAppSize
                r = l + self.minAppSize * self.aspectRatio
        elif corner == BOTTOM_RIGHT:
            r+=dx; b=b-dx/self.aspectRatio
            if r-l < self.minAppSize and (r-l) <= (t-b):
                r = l + self.minAppSize
                b = t - self.minAppSize / self.aspectRatio
            elif t-b < self.minAppSize and (t-b) < (r-l):
                b = t - self.minAppSize
                r = l + self.minAppSize * self.aspectRatio

        self.bounds.setAll(l,r,t,b)

    
