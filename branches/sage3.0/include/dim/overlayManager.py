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



from globals import *
from threading import RLock
import os, os.path



class OverlayManager:
    """ creates and deletes all overlays """

    def __init__(self):
        self.overlays = {}            # key=overlayId, value=overlay objects
        self.__appOverlays = {}       # key=appId, value=list of overlayIds
        self.evtMgr = getEvtMgr()
        self.sageGate = getSageGate()
        self.addOverlayLock = RLock()  # for adding one overlay at a time
        self.addOverlayQueue = []

        # load all the plugins now
        self.__overlayPlugins = {}  # key=overlayType, value=overlayPlugin
        self.__loadAllPlugins()
        
        # register for events
        self.evtMgr.register(EVT_NEW_APP, self.__onNewApp)
        self.evtMgr.register(EVT_APP_KILLED, self.__onAppKilled)
        self.evtMgr.register(EVT_OBJECT_INFO, self.__onObjectInfo)

        # add other overlays
        self.__addOverlays()


    def __addOverlays(self):
        """ you can add overlays here manually """
        pass
        
        
    def __onNewApp(self, event):
        """ add the overlays that need to be tied to the app """
        self.__appOverlays[event.app.getId()] = []   # create the list for keeping overlays
        self.addOverlay(OVERLAY_APP, None, event.app)


    def __onAppKilled(self, event):
        """ remove the overlays tied to the app """
        appId = event.app.getId() 
        if appId in self.__appOverlays:
            for overlayId in self.__appOverlays[appId]:
                self.overlays[overlayId].destroy()
                self.removeOverlay(overlayId)
            del self.__appOverlays[appId]

  
    def __onObjectInfo(self, event):
        """ the overlay was successfully added by SAGE and we got the overlayId """
        self.addOverlayLock.acquire()
        overlayType, callback, app, displayId = self.addOverlayQueue.pop(0)

        # now add a new one (if there are any)
        if len(self.addOverlayQueue) > 0:
            # determine the draw order
            drawOrder = INTER_DRAW   #inter draw
            if overlayType == OVERLAY_POINTER:
                drawOrder = POST_DRAW     
            self.sageGate.addOverlay(self.addOverlayQueue[0][0],100,100,100,100, True, drawOrder, displayId)
        self.addOverlayLock.release()

        # if overlayPlugin isn't loaded yet 
        if overlayType not in self.__overlayPlugins:  
            if not self.__loadOverlayPlugin( overlayType ):
                return   # couldn't load the plugin

        # at this point we have a plugin loaded so create an overlay object
        if app: # is the overlay tied to the application? 
            newOverlayObj = self.__overlayPlugins[ overlayType ].makeNew(event.overlayId, app)
            self.__appOverlays[app.getId()].append(event.overlayId)
        else:
            newOverlayObj = self.__overlayPlugins[ overlayType ].makeNew(event.overlayId)
        self.overlays[ event.overlayId ] = newOverlayObj

        # finally report back to the entity that requested the overlay
        if callback:
            callback(newOverlayObj)


    def removeOverlay(self, overlayId):
        """ removes the overlay from SAGE and locally """
        self.sageGate.removeOverlay(overlayId)
        if overlayId in self.overlays:
            del self.overlays[overlayId]


    def removeAllOverlays(self):
        for overlayId in self.overlays.keys():
            self.removeOverlay(overlayId)
            

    def addOverlay(self, overlayType, callback=None, app=None, displayId=0):
        """ specify an optional callback to call once the
            overlay is successfully added by SAGE
            Specify an app if you want the overlay to be tied to an application
            (if so it will be deleted when the app is closed)
        """
        self.addOverlayLock.acquire()

        # determine the draw order
        drawOrder = INTER_DRAW   #inter draw
        if overlayType == OVERLAY_POINTER:
            drawOrder = POST_DRAW     

        # if this is the only thing to add, just do it now
        # otherwise it will be added by the __onObjectInfo when the previous one gets added
        self.addOverlayQueue.append((overlayType, callback, app, displayId))
        if len(self.addOverlayQueue) == 1:  
            self.sageGate.addOverlay(overlayType,0,0,100,100, True, drawOrder, displayId)
            
        self.addOverlayLock.release()


        
    def __loadAllPlugins(self):
        """ goes into the 'overlays' directory and loads all the plugins from there """

        for entry in os.listdir("overlays"):
            if os.path.splitext(entry)[1] == ".py" and entry != "__init__.py":
                self.__loadOverlayPlugin( os.path.splitext(entry)[0] )
            

    def __loadOverlayPlugin(self, overlayType):
        """ try to load the overlayPlugin for the requested overlayType """
        try:
            overlayPlugin = __import__("overlays."+overlayType, globals(), locals(), [overlayType])
            self.__overlayPlugins[ overlayType ] = overlayPlugin
            print "Overlay plugin loaded: ", overlayType
            return True
        
        except ImportError:
            return False   # failed to load the module

    
