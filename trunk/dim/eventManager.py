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



from threading import RLock
from globals import *
import traceback as tb
import sys
from events import *



class EventManager:
    """ receives all the events and then forwards them to
        all the event handlers that want them
    """

    def __init__(self):
        self.evtLock = RLock()
        self.__evtHandlers = {}   # keyed by eventId, value=list of callbacks

        # add other handlers
        #self.__addHandlers()


    def addHandlers(self):
        """ you can add evtHandlers here manually """


    def __onDisplayInfo(self, evt):
        di = event.displayInfo
        for display in di.getAllDisplays():
            self.__importHandlerPlugin("wall").makeNew(display.getId())  # one for each display


    def __importHandlerPlugin(self, name):
        plugin = __import__("overlays."+name, globals(), locals(), [name])
        return plugin
        

    def register(self, eventId, callback):
        """ register the evtHandler to receive events of eventId """
        if eventId not in self.__evtHandlers:
            self.__evtHandlers[eventId] = []
        self.__evtHandlers[eventId].append( callback )


    def unregister(self, callbackHash):
        """ unregisters the evtHandler from the specified events """
        for eventId, callback in callbackHash.iteritems():
            try:
                self.__evtHandlers[eventId].remove(callback)
            except:
                continue
            

    def postEvent(self, event):
        """ thread safe """
        self.evtLock.acquire()

        # handle differently depending on whether
        # it's a device event or a generic event
        if event.eventId in self.__evtHandlers:
            if event.eventType == DEVICE_EVENT:
                self.__sendToSingleEvtHandler(event)
            else:
                if event.eventType == EVT_DISPLAY_INFO:
                    self.__onDisplayInfo(event)   # create objects that are display dependent
                    
                self.__sendToMultipleEvtHandler(event)
            
        self.evtLock.release()


    def getCallbackAtPos(self, event):
        """ returns the callback function for the specified event at pos x,y
            Or None if no suitable evtHandler was found at that position
            Considers z ordering
        """
        currentCandidate = None  # the candidate based on the z value
        lastZ = BOTTOM_Z
        x, y, eventId, displayId = event.x, event.y, event.eventId, event.device.displayId
        
        
        if eventId in self.__evtHandlers:
            for callback in self.__evtHandlers[eventId]:
                evtHandler = callback.im_self
                if evtHandler.displayId == displayId and \
                       evtHandler.z < lastZ and \
                       evtHandler.bounds.isIn(x,y):
                    currentCandidate = callback
                    lastZ = evtHandler.z

        # check if the handler is already captured
        if currentCandidate and currentCandidate.im_self.captured:
            return None
        
        return currentCandidate
                

    def __sendToSingleEvtHandler(self, event):
        """ forward the event to a handler that is at the position
            of the event and wants the current event
        """
        eventId = event.eventId
        device = event.device

        # if the event goes to a specific evtHandler, no need to search for one
        if event.toEvtHandler:
            callback = event.toEvtHandler.getCallback(eventId)
        else:
            callback = self.getCallbackAtPos(event)
                
            # this is here to generate EVT_ENTERED_WINDOW and EVT_LEFT_WINDOW
            if eventId == EVT_MOVE or eventId == EVT_MOVE_SPECIAL:
                if callback != device.lastMoveCallback:   # window changed
                    if device.lastMoveCallback:
                        evtId = EVT_LEFT_WINDOW
                        if device.specialDevice: evtId = EVT_LEFT_WINDOW_SPECIAL
                        self.__sendEvent(WindowLeftEvent(device),
                                         device.lastMoveCallback.im_self.getCallback(evtId))
                    if callback:
                        evtId = EVT_ENTERED_WINDOW
                        if device.specialDevice: evtId = EVT_ENTERED_WINDOW_SPECIAL
                        self.__sendEvent(WindowEnteredEvent(device),
                                         callback.im_self.getCallback(evtId))
                    device.lastMoveCallback = callback
                    
        # send the original event
        self.__sendEvent(event, callback)
        

    def __sendToMultipleEvtHandler(self, event):
        """ forward to all the evtHandlers registered for this eventId """
        for callback in self.__evtHandlers[event.eventId]:
            self.__sendEvent(event, callback)


    def __sendEvent(self, event, callback):
        """ executes callback with event as a parameter """
        if callback:
            try:
                callback(event)
            except AttributeError:   # if the object doesn't exist anymore, remove its callback
                tb.print_exc()
                self.__evtHandlers[eventId].remove(callback)
