/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

#include "mmap.h"
#include <stdio.h>

Mmap::Mmap()
{
    _filename = 0;
    _len = 0;
    _fd = -1;
    //_prot = PROT_READ|PROT_WRITE;
    _prot = PROT_READ;
    _shared = MAP_PRIVATE;
    _mmapAddress = (caddr_t) -1;
}

Mmap::~Mmap()
{

    // free filename
    if(_filename)
        delete _filename;

    // close file
    if(_fd >= 0)
        close(_fd);

    // unmap file
    if(_mmapAddress)
        munmap(_mmapAddress, _len);
}

int
Mmap::setFilename(const char *filename, int create)
{
    int len = strlen(filename)+1;
    _filename = new char [len];
    strcpy(_filename, filename);

    struct stat statbuf;


    if(!create)
    {
        // told not to create it -- it must exist already
        _fd = open(_filename, O_RDONLY) ;
    }
    else {
        _fd = open(_filename, O_RDONLY|O_CREAT, 00600);
    }

    if(_fd < 0)
        return -1;

    int rc = stat(_filename, &statbuf);
    if(rc < 0)
    {
        // Strange -- we could open but not stat????
        return(-1);
    }

    _len = (size_t) statbuf.st_size;
    fprintf(stderr,"File size %d\n",_len);
    return 1;
}

void Mmap::setSize(size_t size)
{
    if(_len != size) {
        // Must resize the file
        lseek(_fd, (off_t) size, SEEK_SET);
        // write one byte to ensure that we're at least this size
        write(_fd, "", 1);
        // Now truncate the file to be this size
        ftruncate(_fd, size);
    }
    _len = size;
}



caddr_t
Mmap::doMmap()
{
    _mmapAddress = (caddr_t) mmap(0, _len, _prot, _shared, _fd, 0);

    return _mmapAddress;
}

void
Mmap::bzero()
{
    if(_mmapAddress != (caddr_t) -1)
        memset(_mmapAddress, 0, _len);
}


