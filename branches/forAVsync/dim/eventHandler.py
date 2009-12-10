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



class EventHandler:
    """ The base class for any objects wanting to receive device events.
        Make sure the bounds member is up to date since it's used
        in deciding whether an event should be delivered to it or not.
        Also, make sure you update the z value (if applicable)

        Example of registering for events:
        ----------------------------------------
        self.registerForEvent(EVT_CLICK, self.onClick)
        def onClick(self, event):
            # do something with the click event...
    """

    def __init__(self):
        self.evtMgr = getEvtMgr()  # for registering for events
        self.captured = False
        self.bounds = Bounds()     # the bounds for detecting the events
        self.displayId = 0         # the bounds will depend on this...
        self.z = TOP_Z   # always on top
        self.__callbacks = {}  # key eventId, value=callback

 ##        # kind of a hack to make sure that enter and
##         # leave events get delivered properly
##         self.registerForEvent(EVT_MOVE, self.dummyOnMove)


##     def dummyOnMove(self, event):
##         # kind of a hack to make sure that enter and
##         # leave events get delivered properly
##         pass
        

    def registerForEvent(self, eventId, callback):
        self.__callbacks[eventId] = callback
        self.evtMgr.register(eventId, callback)


    def getCallback(self, eventId):
        if eventId in self.__callbacks:
            return self.__callbacks[eventId]


    def destroy(self):
        """ may be overridden but you need to call this base class' method """
        self.evtMgr.unregister(self.__callbacks)
        
        
