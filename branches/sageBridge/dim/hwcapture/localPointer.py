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



import sys, wx
#sys.path.append("../dim/hwcapture")
from managerConn import ManagerConnection
from threading import Timer
import time

#if len(sys.argv) > 2:
SHOW_WINDOW = True
#else:
#    SHOW_WINDOW = False

MOVE = 1
CLICK = 2
WHEEL = 3

LEFT=1
RIGHT=2
MIDDLE = 3  

global lastX, lastY
lastX = lastY = 0
global frame, manager, panel, buttons, w, h, label
buttons = {}  #key=BTN_ID, value=True|False


def sendMsg(*data):
    msg = ""
    for m in data:
        msg+=str(m)+" "
    manager.sendMessage("", "mouse", msg)


def onKey(event):
    if event.GetKeyCode() == wx.WXK_ESCAPE:
        if panel.HasCapture():
            panel.ReleaseMouse()
        frame.Close()
    elif event.GetKeyCode() == wx.WXK_SPACE:
        global label
        if panel.HasCapture():
            label.SetLabel("Press SPACE to capture mouse")
            panel.ReleaseMouse()
        else:
            label.SetLabel("Press SPACE to release mouse")
            panel.CaptureMouse()


def onMove(event):
    global lastX, lastY
    ms = wx.GetMouseState()
    mx = ms.GetX()
    my = ms.GetY()
    x = float(mx/float(w))
    y = 1-float(my/float(h))
    if lastX != x and lastY != y:
        sendMsg(MOVE, x,y)
        lastX = x
        lastY = y


def onClick(event):
    isDown=False
    if event.ButtonDown():  isDown=True

    if event.GetButton() == wx.MOUSE_BTN_LEFT:
        btnId = LEFT
    elif event.GetButton() == wx.MOUSE_BTN_RIGHT:
        btnId = RIGHT
    else:
        btnId = MIDDLE

    sendMsg(CLICK, btnId, int(isDown))


def onWheel(event):
    numSteps = event.GetWheelRotation()/event.GetWheelDelta()
    sendMsg(WHEEL, numSteps)
            

def onClose(event):
    manager.quit()
    frame.Destroy()
    wx.GetApp().ExitMainLoop()


def doCapture():
    wx.CallAfter(panel.CaptureMouse)
    

# runs in a thread, captures mouse position and sends to dim regularly
def captureMouse(mgr):
    app = wx.App()
    
    global manager,w,h
    manager = mgr

    (w,h) = wx.DisplaySize()

    if SHOW_WINDOW:
        global frame, panel, label
        frame = wx.Frame(None, size=(250,80), pos=(100,100))
        frame.Bind(wx.EVT_CLOSE, onClose)
        
        panel = wx.Panel(frame, size=(250,80), pos=(0,0))
        panel.Bind(wx.EVT_KEY_DOWN, onKey)
        panel.Bind(wx.EVT_LEFT_DOWN, onClick)
        panel.Bind(wx.EVT_LEFT_UP, onClick)
        panel.Bind(wx.EVT_RIGHT_DOWN, onClick)
        panel.Bind(wx.EVT_RIGHT_UP, onClick)
        panel.Bind(wx.EVT_MOTION, onMove)
        panel.Bind(wx.EVT_MOUSEWHEEL, onWheel)

        #label = wx.StaticText(panel, wx.ID_ANY, "Press SPACE to release mouse")
        label = wx.StaticText(panel, wx.ID_ANY, "Press SPACE to capture mouse")
        panel.Fit()
        panel.SetFocus()

        #t = Timer(2.0, doCapture)
        #t.start()
              
        frame.Show()
        app.MainLoop()
    else:
        while True:
            onIdle()
    
if len(sys.argv) < 2:
    print "USAGE: python localPointer.py SAGE_IP"
else:
    print "Connecting to: ", sys.argv[1]
    captureMouse(ManagerConnection(sys.argv[1], 20005))
