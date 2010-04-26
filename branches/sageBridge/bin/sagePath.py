############################################################################
#
# SAGE LAUNCHER - A GUI for launching SAGE and all related components
#
# Copyright (C) 2010 Electronic Visualization Laboratory,
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

import os.path, os, sys
import traceback as tb

opj = os.path.join


CONFIG_DIR = ".sageConfig"
USER_DIR = os.path.expanduser("~")
USER_CONFIG_DIR = opj( USER_DIR, CONFIG_DIR )
SAGE_DIR = os.path.realpath(os.environ["SAGE_DIRECTORY"])    # where sage is located
DEFAULT_CONFIG_DIR = opj( SAGE_DIR, CONFIG_DIR.lstrip(".") )


# first make the user config dir if it doesnt exist
if not os.path.isdir(USER_CONFIG_DIR):
    os.makedirs(USER_CONFIG_DIR)
    os.makedirs( opj(USER_CONFIG_DIR, "applications", "pid") )


# searches two paths: ~/.sageConfig and $SAGE_DIRECTORY/sageConfig
def getPath(*args):
    fileName = ""
    for a in args:
        fileName = opj(fileName, a)

    userPath = opj( USER_CONFIG_DIR, fileName )
    defaultPath = opj( DEFAULT_CONFIG_DIR, fileName )
    currentPath = opj( os.getcwd(), args[len(args)-1])  # last argument

    if os.path.isfile( currentPath ):
        return currentPath
    elif os.path.isfile( userPath ):
        return userPath
    else:
        return defaultPath


# to get a path to a file in the user's home config directory
def getUserPath(*args):
    p = USER_CONFIG_DIR
    for a in args:
        p = opj(p, a)

    # make the directory... if it doesnt exist
    # this will make the directory even if a file is passed in with some directories before it
    if not os.path.exists(p):
        try:
            d, ext = os.path.splitext(p)
            if ext == "":   # it's a directory
                d = os.path.splitext(p)[0]
            else:           # it's a file
                d = os.path.dirname(p)                
            if not os.path.exists(d): os.makedirs(d)
            
        except:
            print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
        
    return p
    


# to get a path to a file in the default config directory
def getDefaultPath(*args):
    p = DEFAULT_CONFIG_DIR
    for a in args:
        p = opj(p, a)
    return p
