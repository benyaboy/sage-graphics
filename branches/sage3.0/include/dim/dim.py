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



# python imports
import optparse, time, os, sys

# my imports
from deviceManager import DeviceManager
from eventManager import EventManager
from overlayManager import OverlayManager
from listener import Listener
from sageGate import SageGate
from sageData import SageData
from globals import *


# ------------------------------------------------------------------------------
#
#                         GLOBALS   
#
# ------------------------------------------------------------------------------

VERBOSE = False
LISTENER_PORT = 20005   # for HW capture modules (ie HW devices sending events)


os.chdir(sys.path[0])


class DIM:
    def __init__(self, host, port):
        # sageGate is the network connection with SAGE
        self.sageGate = SageGate()
        setSageGate(self.sageGate)

        # the event manager takes care of properly dispatching events
        self.evtMgr = EventManager()
        setEvtMgr(self.evtMgr)
        self.evtMgr.addHandlers()

        # sageData keeps the current state of the SAGE windows/apps
        self.sageData = SageData()
        setSageData(self.sageData)
        
        # overlay manager creates, destroys and updates overlays
        self.overlayMgr = OverlayManager()
        setOverlayMgr(self.overlayMgr)

        # contains all the devices and takes care of loading plugins for them
        # also, distributes HW messages to each device 
        self.devMgr = DeviceManager()
        setDevMgr(self.devMgr)

        # connect to SAGE
        for i in range(5):  # try to connect to SAGE for 5 seconds
            if self.sageGate.connectToSage(host, port) != 0:
                self.sageGate.registerSage()
                break
            time.sleep(1)
        else:  # we didn't manage to connect to sage in 5 seconds... so quit
            exitApp()

        # start listening for the device events
        time.sleep(2)   # wait till all the messages come in
        self.listener = Listener(LISTENER_PORT, self.devMgr.onHWMessage)
        self.listener.serve_forever()



### sets up the parser for the command line options
def get_commandline_options():
    parser = optparse.OptionParser()

    h = "if set, prints output to console, otherwise to output_log.txt"
    parser.add_option("-v", "--verbose", dest="verbose", action="store_true", help=h, default=False)

    h = "interaction with which sage? (default=localhost)"
    parser.add_option("-s", "--sage_host", dest="host", help=h, default="localhost")

    h = "the UI port number for sage (default is 20001)"
    parser.add_option("-p", "--sage_port", dest="port", help=h, type="int", default=20001)

    return parser.parse_args()




# ------------------------------------------------------------------------------
#
#                         MAIN
#
# ------------------------------------------------------------------------------


def main():
    global VERBOSE
    
    # parse the command line params
    (options, args) = get_commandline_options()
    VERBOSE = options.verbose
    sagePort = options.port
    sageHost = options.host

    # start everything off
    DIM(sageHost, sagePort)
    

    
if __name__ == '__main__':
    main()
