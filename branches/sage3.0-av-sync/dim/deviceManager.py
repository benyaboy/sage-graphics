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




import os, os.path


class DeviceManager:
    
    def __init__(self):
        self.devices = {}  # key=deviceId, value=deviceObj

        # load all the plugins now
        self.__devicePlugins = {}  # key=deviceType, value=devicePlugin
        self.__loadAllPlugins()

        # for special devices and assigning unique ids
        self.__specialDevices = []  # a list of special devices
        
        
        
    def onHWMessage(self, deviceId, deviceType, data):
        """ this gets called when a message arrives from any hw device """
        
        # check whether the device already exists, if so pass the message to it
        if deviceId in self.devices:
            self.devices[ deviceId ].onMessage(data, False)
            
        else:
            if deviceType not in self.__devicePlugins:  # devicePlugin isn't loaded yet 
                if not self.__loadDevicePlugin( deviceType ):
                    return   # couldn't load the plugin
            # at this point we have a plugin loaded so create a device object to do the conversion

            newDeviceObj = self.__devicePlugins[ deviceType ].makeNew(deviceId)
            self.devices[ deviceId ] = newDeviceObj

            # set a special id if the device is special
            if newDeviceObj.specialDevice:
                specId = len(self.__specialDevices)
                newDeviceObj.setSpecialId(specId)
                self.__specialDevices.append(deviceId)
                
            newDeviceObj.onMessage(data, firstMsg=True)


    def removeDevice(self, deviceId):
        # remove from the list of devices
        if deviceId in self.devices:
            self.devices[ deviceId ].destroy()
            del self.devices[ deviceId ]

        # remove from the list of special devices if necessary
        if deviceId in self.__specialDevices:
            self.__specialDevices.remove(deviceId)
            

    def __loadAllPlugins(self):
        """ goes into the 'devices' directory and loads all the plugins from there """

        for entry in os.listdir("devices"):
            if os.path.splitext(entry)[1] == ".py" and entry != "__init__.py":
                self.__loadDevicePlugin( os.path.splitext(entry)[0] )
            

    def __loadDevicePlugin(self, deviceType):
        """ try to load the devicePlugin for the requested deviceType """
        try:
            #(f, path, desc) = imp.find_module(deviceType, "devices")
            #devicePlugin = imp.load_module(deviceType, f, path, desc)
            devicePlugin = __import__("devices."+deviceType, globals(), locals(), [deviceType])
            self.__devicePlugins[ deviceType ] = devicePlugin
            print "Device plugin loaded: ", deviceType
            return True
                
        except ImportError:
            return False   # failed to load the module

    
