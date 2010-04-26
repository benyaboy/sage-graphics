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




from eventHandler import EventHandler
from globals import *



def makeNew(displayId):
    """ this is how we instantiate the object from
        the main program that loads this plugin """
    return Wall(displayId)



class Wall(EventHandler):

    def __init__(self, displayId):
        EventHandler.__init__(self)
        self.displayId = displayId
        #self.registerForEvent(EVT_MOVE, self.__onMove)
        self.registerForEvent(EVT_DISPLAY_INFO, self.__onDispInfo)
        self.registerForEvent(EVT_ENTERED_WINDOW, self.__onEnteredWindow)
        self.registerForEvent(EVT_LEFT_WINDOW, self.__onLeftWindow)
        self.z = BOTTOM_Z


##     def __onMove(self, event):
##         """ just resets the pointer for now """
##         device = event.device
##         if device.pointer:
##             device.pointer.resetPointer()
##             device.pointer.showOutApp()

    def __onLeftWindow(self, event):
        pass
        

    def __onEnteredWindow(self, event):
        pointer = event.device.pointer
        if pointer:
            device.pointer.resetPointer()
            device.pointer.showOutApp()
    

    def __onDispInfo(self, event):
        di = event.displayInfo
        display = di.getDisplay(self.displayId)
        self.bounds.setAll(0, display.sageW, display.sageH, 0)
