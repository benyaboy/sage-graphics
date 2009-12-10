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



class Overlay:
    """ this describes the basic overlay - ie an object
        that's drawn on the screen
    """

    def __init__(self, overlayType, overlayId):
        #self.overlayId
        self.overlayType = overlayType
        self.overlayId = overlayId
        self.displayId = 0
        self.sageGate = getSageGate()
        self.visible = True
        #self.x = 0  # position in SAGE coords
        #self.y = 0


    def getId(self):
        return self.overlayId
    

    def getType(self):
        return self.overlayType


    def isVisible(self):
        return self.visible


    def sendOverlayMessage(self, *params):
        self.sageGate.sendOverlayMessage(self.overlayId, *params)
    

    def hide(self):
        if self.visible:
            self.visible = False
            self.sageGate.showOverlay(self.overlayId, False)


    def show(self):
        if not self.visible:
            self.visible = True
            self.sageGate.showOverlay(self.overlayId, True)


    def update(self, now):
        """ do something to update your drawing or whatever
            the *now* parameter is the current time in seconds... float
        """
        pass


##     def destroy(self):
##         """ Called when the object is deleted.
##             Make sure you call this method if you override it.
##         """
##         pass


    
