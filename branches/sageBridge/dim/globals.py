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



# overlay types (these are same as SAGEDrawObject types)
OVERLAY_POINTER = "pointer"   
OVERLAY_APP = "app"      # app checker

# btnIDs
BTN_LEFT   = 1
BTN_RIGHT  = 2
BTN_MIDDLE = 3

# arrow IDs
ARROW_UP    = 1
ARROW_DOWN  = 2
ARROW_LEFT  = 3
ARROW_RIGHT = 4


# pointer orientations
BOTTOM_LEFT  = 1
TOP_LEFT     = 2
TOP_RIGHT    = 3
BOTTOM_RIGHT = 4


# z order
BOTTOM_Z = 99999
TOP_Z    = -1


# draw order for SAGE
POST_DRAW = 0
INTER_DRAW = 1
PRE_DRAW = 2


#-----------------------------------------------------
#       EVENTS
#-----------------------------------------------------

# EVENT TYPES
DEVICE_EVENT    = 1  # position dependent
GENERIC_EVENT   = 2  # sent to everybody that is listening

# device event IDs
EVT_CLICK   = 0
EVT_MOVE    = 2
EVT_ANALOG1 = 3
EVT_PAN     = 3 # alias
EVT_DRAG    = 3 # alias
EVT_ANALOG2 = 4
EVT_ROTATE  = 4 # alias
EVT_ANALOG3 = 5
EVT_ZOOM    = 5 # alias
EVT_ARROW   = 6
EVT_KEY     = 7
EVT_CUSTOM  = 8
EVT_ENTERED_WINDOW = 9
EVT_LEFT_WINDOW = 10

# special device events
EVT_CLICK_SPECIAL   = 20
EVT_MOVE_SPECIAL    = 22
EVT_ANALOG1_SPECIAL = 23
EVT_PAN_SPECIAL     = 23 # alias
EVT_DRAG_SPECIAL    = 23 # alias
EVT_ANALOG2_SPECIAL = 24
EVT_ROTATE_SPECIAL  = 24 # alias
EVT_ANALOG3_SPECIAL = 25
EVT_ZOOM_SPECIAL    = 25 # alias
EVT_ARROW_SPECIAL   = 26
EVT_KEY_SPECIAL     = 27
EVT_CUSTOM_SPECIAL  = 28
EVT_ENTERED_WINDOW_SPECIAL = 29
EVT_LEFT_WINDOW_SPECIAL = 30

# generic event IDs
EVT_APP_INFO  = 50
EVT_PERF_INFO = 51
EVT_NEW_APP   = 52
EVT_APP_KILLED = 53
EVT_Z_CHANGE  = 54
EVT_OBJECT_INFO = 55
EVT_DISPLAY_INFO = 56



#-----------------------------------------------------
#       GLOBAL ACCESS VARIABLES
#-----------------------------------------------------

# EventManager - so that everybody can access it
global evtMgr
def setEvtMgr(em):
    global evtMgr
    evtMgr = em

def getEvtMgr():
    return evtMgr



# DeviceManager - so that everybody can access it
global devMgr
def setDevMgr(dm):
    global devMgr
    devMgr = dm

def getDevMgr():
    return devMgr



# OverlayManager - so that everybody can access it
global overlayMgr
def setOverlayMgr(om):
    global overlayMgr
    overlayMgr = om

def getOverlayMgr():
    return overlayMgr



# SageGate - so that everybody can access it
global sageGate
def setSageGate(q):
    global sageGate
    sageGate = q

def getSageGate():
    return sageGate


# SageData - so that everybody can access it
global sageData
def setSageData(d):
    global sageData
    sageData = d

def getSageData():
    return sageData



    
sageServer = "sage.sl.startap.net"
def setSAGEServer(server):
    global sageServer
    sageServer = server

def getSAGEServer():
    return sageServer



# for quitting the whole application
global run
run = True
def exitApp():
    global run
    run = False
    getOverlayMgr().removeAllOverlays()
def doRun():
    return run




class Bounds:
    """ used in event conversion """
    def __init__(self, left=0, right=0, top=0, bottom=0):
        self.left = left
        self.right = right
        self.top = top
        self.bottom = bottom
        self.setAspectRatio()


    def setAll(self, left, right, top, bottom):
        self.left = left
        self.right = right
        self.top = top
        self.bottom = bottom
        self.setAspectRatio()

    def getAll(self):
        return self.left, self.right, self.top, self.bottom

    def setAspectRatio(self):
        if self.getHeight() != 0:
            self.aspectRatio = self.getWidth() / float(self.getHeight())
        else:
            self.aspectRatio = 1.0

    def getAspectRatio(self):
        return self.aspectRatio

    def getWidth(self):
        return self.right - self.left

    def setWidth(self, w):
        self.right = self.left + w
        self.setAspectRatio()

    def getHeight(self):
        return self.top - self.bottom

    def setHeight(self, h):
        self.top = self.bottom + h
        self.setAspectRatio()

    def setSize(self, w, h):
        self.setWidth(w)
        self.setHeight(h)


    def getX(self):
        return self.left

    def getY(self):
        return self.bottom

    def setPos(self, x, y):
        self.setX(x)
        self.setY(y)
        
    def setX(self, x):
        w = self.getWidth()
        self.left = x
        self.right = x + w

    def setY(self, y):
        h = self.getHeight()
        self.bottom = y
        self.top = y + h


    def isIn(self, x, y):
        """ returns True if the (x,y) is in Bounds, False otherwise """
        if self.left <= x and self.right >= x and self.bottom <= y and self.top >= y:
            return True
        else:
            return False
