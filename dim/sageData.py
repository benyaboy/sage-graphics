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



# python stuff
import string, sys

# my imports
from sageApp import SageApp, SageAppInitial
from sageDisplayInfo import SageDisplayInfo
from globals import *
import events


## Main class to store all the messages returned by SAGE
class SageData:


    #### constructor initializes member values       
    def __init__(self) :
        self.hashApps = {}           # all the apps available for running??
        self.hashAppStatusInfo = {}  # apps currently running
        self.displayInfo = SageDisplayInfo()

        self.sageGate = getSageGate()
        self.sageGate.registerCallbackFunction(40000, self.setSageStatus)
        self.sageGate.registerCallbackFunction(40001, self.setSageAppInfo)
        #self.sageGate.registerCallbackFunction(40002, self.setSagePerfInfo)
        self.sageGate.registerCallbackFunction(40003, self.sageAppShutDown)
        self.sageGate.registerCallbackFunction(40004, self.setDisplayInfo)
        self.sageGate.registerCallbackFunction(40005, self.setSageZValue)
        self.sageGate.registerCallbackFunction(40006, self.setSageAppExecConfig)
        self.sageGate.registerCallbackFunction(40007, self.setDisplayConnections)
        self.sageGate.registerCallbackFunction(40018, self.setSageObjectInfo)


    ### clears the current state (to be done when reconnecting)
    def clear(self):
        self.hashApps = {}           # all the apps available for running??
        self.hashAppStatusInfo = {}
        
        
    #### Set the sage status
    def setSageStatus(self, appHash) :  
        for appName, configs in appHash.iteritems():
            self.hashApps[appName] = SageAppInitial(appName, configs)
            

    #----------------------------------------------------------------------


    ### Set the possible execution configurations for each app
    def setSageAppExecConfig(self, data):
        tokens = string.split( data, '\n', 1 )
        appName = tokens[0]
        data = tokens[1] #the rest
        configList = string.split( data, "config ")

        del configList[0]  #remove the first item in the list

        for config in configList:
            if appName in self.hashApps.keys():
                (name, stringConfig) = string.split(config, ":", 1)
                self.hashApps[appName].AddConfig(name, stringConfig)


    #----------------------------------------------------------------------
 
    
    #### Set the SAGE display information
    #### 
    def setDisplayInfo(self, data):
        listTokens = string.split(data, '\n')

        for i in range(0, len(listTokens), 3):
            tileNumTokens = string.split(listTokens[i], ' ')
            desktopTokens = string.split(listTokens[i+1], ' ')
            tileConfTokens = string.split(listTokens[i+2], ' ')            
            
            # so that we can support the old sage as well
            displayId = 0
            if len(tileConfTokens) > 2:
                displayId = int(tileConfTokens[2])

            self.displayInfo.addDisplay(int(tileNumTokens[0]), int(tileNumTokens[1]), int(tileNumTokens[2]),
                                   int(desktopTokens[0]), int(desktopTokens[1]),
                                   int(tileConfTokens[0]), int(tileConfTokens[1]), displayId)

        # for one display and old SAGE there are no connection messages so just
        # pretend that one came in for drawing everything correctly
        if self.displayInfo.getNumDisplays() == 1:
            self.setDisplayConnections("")
        

    #----------------------------------------------------------------------
    
    # returns the SageDisplayInfo object
    def getDisplayInfo(self, displayId=0):
        return self.displayInfo.getDisplay(displayId)


    #----------------------------------------------------------------------


    def setDisplayConnections(self, data):
        if data:
            for connection in data.split('\n'):
                tokens = connection.split()
                displayId = int(tokens[2])
                self.displayInfo.getDisplay(displayId).placement = int(tokens[1])

        evt = events.DisplayInfoEvent(self.displayInfo)
        getEvtMgr().postEvent(evt)


    #----------------------------------------------------------------------
    

    #### Get the new list of z values from SAGE
    #### and update local hashes, then call the function to update the UI visually
    def setSageZValue(self, message):
        tokens = string.split(message)
        numZChanges = int(tokens[0])  #the first item that comes in is the number of z changes
        zHash = {}   # key=appId, value=new z value
        
        # loop through all the tokens and update the z values of the apps
        for i in range(numZChanges):
            self.setZvalue( int(tokens[i*2+1]), int(tokens[i*2+2]) )
            zHash[int(tokens[i*2+1])] = int(tokens[i*2+2])
            
        evt = events.ZChangeEvent(zHash)
        getEvtMgr().postEvent(evt)


    #----------------------------------------------------------------------

    
    #### Set the SAGE app status
    #### prints Invalid app ID if does not exists
    def setSageAppInfo(self, data):
        listTokens = string.split(data)

        # to support old sage as well
        displayId = 0
        orientation = 0
        if len(listTokens) > 8:
            orientation = int(listTokens[8])
            displayId = int(listTokens[9])
            

        appId = int( listTokens[ 1 ] )
        if appId in self.hashAppStatusInfo:
            # update the app in the hash 
            self.hashAppStatusInfo[ appId ].setAll( listTokens[0], int(listTokens[1]),
                   int(listTokens[2]), int(listTokens[3]), int(listTokens[4]), int(listTokens[5]),
                   int(listTokens[6]), int(listTokens[7]), orientation, displayId)

            # make the event
            evt = events.AppInfoEvent(self.hashAppStatusInfo[appId])
        else:
            # when new app is started it is assigned a z=0 but since previous app on top had z=0,
            # we set this one even higher temporarily (z=-1) so that it gets drawn on top
            # the new z order message comes right after the app is started so this -1 is temporary
            zValue = int(listTokens[7])
            for app in self.hashAppStatusInfo.itervalues():
                if app.getZvalue() == zValue:
                    zValue = -1
            self.hashAppStatusInfo[ appId ] = SageApp( listTokens[0], int(listTokens[1]),
                   int(listTokens[2]), int(listTokens[3]), int(listTokens[4]), int(listTokens[5]),
                   int(listTokens[6]), zValue, orientation, displayId)

            # make the event
            evt = events.NewAppEvent(self.hashAppStatusInfo[appId])

        # post the event about the state change
        getEvtMgr().postEvent(evt)


    #----------------------------------------------------------------------
    
    
    ##### ShutDown the sage application
    ##### prints invalid app ID if doesn't exist
    def sageAppShutDown(self, data):
        listTokens = string.split(data)
        appId = int(listTokens[0])

        if appId in self.hashAppStatusInfo :
            # do this first and then remove the app from the hash!!
            # make and post the event
            evt = events.AppKilledEvent(self.hashAppStatusInfo[appId])
            getEvtMgr().postEvent(evt)

            # delete the app
            del self.hashAppStatusInfo[appId]


    #----------------------------------------------------------------------


    def setSageObjectInfo(self, data):
        evt = events.ObjectInfoEvent(data)
        getEvtMgr().postEvent(evt)
        

    #----------------------------------------------------------------------
    
    
    #   Get the z value of the app
    #   @arg appInstID App instance ID of the app
    #   @return Returns Z value if appID exists else returns -1
    def getZvalue(self, appId):
        if appId in self.hashAppStatusInfo:
            return self.hashAppStatusInfo[appId].getZvalue()
        else:
            print("getZvalue: Invalid app instance ID")
            return

            
    #----------------------------------------------------------------------


    #   Set the Z value of the app
    #   @arg appInstId App instance ID of the app
    #   @arg value New Z value to be set
    def setZvalue(self, appId, value):
        if (appId in self.hashAppStatusInfo):
            self.hashAppStatusInfo[appId].setZvalue(value)
        else:
            print ('setZvalue: Invalid app instance ID')
        return


    #----------------------------------------------------------------------


    # returns the SageApp associated with the given appInstId
    def getApp(self, appInstId):
        return self.hashAppStatusInfo[appInstId]
    

    #----------------------------------------------------------------------

    
    ### (RJ 2005-01-15)
    ### Returns a list of all the available apps
    def getAvailableApps( self ):
        return self.hashApps

    ### (AKS 2004-10-23)
    ### Get all app IDs
    ### currently running on sage
    ### @return list of app ids
    def getAllAppIDs(self) :
        return self.hashAppStatusInfo.keys()


    #----------------------------------------------------------------------

    
    ### Returns a hash of all running apps
    def getRunningApps( self ):
        return self.hashAppStatusInfo


    #----------------------------------------------------------------------


    # checks all the apps and reports whether any one of them was
    # hit with a click and which region was hit (corners or shape in general)
    # if more than one shape was hit it returns the one on the top
    def checkHits(self, x, y):
        zHash = {}  #key=z value, value=SageApp

        try:
            for appId, sageApp in self.hashAppStatusInfo.items():
                zHash[sageApp.getZvalue()] = sageApp

            zKeys = zHash.keys()
            zKeys.sort()

            for z in zKeys:
                hitResult = zHash[z].hitTest(x,y)
                if hitResult >= 0:
                    return (zHash[z], hitResult)
        except:
            pass
        return (None, -1)


    #----------------------------------------------------------------------


    # returns (appId, zValue) or (-1,sys.maxint) if no apps
    def getTopApp(self):
        minZ = (-1, sys.maxint)
        try:
            for appId, app in self.hashAppStatusInfo.items():
                if app.getZvalue() < minZ[1]:
                    minZ = (appId, app.getZvalue())
        except:
            pass
        return minZ
        
