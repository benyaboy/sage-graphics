/************************************************************************
 *
 *  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */

 
/*
 * sockets.c - functions to deal with sockets.
 */

#include "vncviewer.h"

#if defined(WIN32)
#include <Winsock2.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#endif

#if defined(__sun)
#include <sys/filio.h> 
#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif
#endif

#include <errno.h>
#include <assert.h>



#if defined(WIN32)
void InitWinsock()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
 
    wVersionRequested = MAKEWORD( 2, 2 );
 
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
	{
		aError("Cannot load the valid winsock 2 dll\n");
        exit(1);
    }
}
#endif


/*
 * ReadFromRFBServer is called whenever we want to read some data from the RFB
 * server.  It is non-trivial for two reasons:
 *
 * 1. For efficiency it performs some intelligent buffering, avoiding invoking
 *    the read() system call too often.  For small chunks of data, it simply
 *    copies the data out of an internal buffer.  For large amounts of data it
 *    reads directly into the buffer provided by the caller.
 *
 * 2. Whenever read() would block, it invokes the Xt event dispatching
 *    mechanism to process X events.  In fact, this is the only place these
 *    events are processed, as there is no XtAppMainLoop in the program.
 */


Bool
VNCViewer::ReadFromRFBServer(char *out, unsigned int n)
{
    if (n <= buffered)
    {
        memcpy(out, bufoutptr, n);
        bufoutptr += n;
        buffered -= n;
        return True;
    }

    memcpy(out, bufoutptr, buffered);

    out += buffered;
    n -= buffered;

    bufoutptr = buf;
    buffered = 0;

    if (n <= BUF_SIZE) {

        while (buffered < n) {
#if defined(WIN32)
            int i = recv(rfbsock, buf + buffered, BUF_SIZE - buffered,0);
#else
            int i = read(rfbsock, buf + buffered, BUF_SIZE - buffered);
#endif
            if (i <= 0) {
                if (i < 0) {
                    if (errno == EAGAIN) {
                        i = 0;
                    } else {
                        aLog("error : read1 -- i %d errno %d rfbsock %d\n",
								i, errno, rfbsock);
                        return False;
                    }
                } else {
                    aLog("VNC server closed connection\n");
                    return False;
                }
            }
            buffered += i;
        }

        memcpy(out, bufoutptr, n);
        bufoutptr += n;
        buffered -= n;
        return True;

    } else
    {
        while (n > 0) {
#if defined(WIN32)
            int i = recv(rfbsock, out, n, 0);
#else
            int i = read(rfbsock, out, n);
#endif
            if (i <= 0) {
                if (i < 0) {
                    if (errno == EAGAIN) {
                        i = 0;
                    } else {
                        aLog("error : read2 -- i %d errno %d\n", i, errno);
                        return False;
                    }
                } else {
                    aLog("VNC server closed connection\n");
                    return False;
                }
            }
            out += i;
            n -= i;
        }

        return True;
    }
}


/*
 * Write an exact number of bytes, and don't return until you've sent them.
 */

Bool
WriteExact(int sock, char *buf, int n)
{
    fd_set fds;
    int i = 0;
    int j;

    while (i < n) {
#if defined(WIN32)
        j = send (sock, buf + i, (n - i), 0);
#else
        j = write(sock, buf + i, (n - i));
#endif
        if (j <= 0) {
            if (j < 0) {
                if (errno == EAGAIN) {
                    FD_ZERO(&fds);
                    FD_SET(sock,&fds);

                    if (select(sock+1, NULL, &fds, NULL, NULL) <= 0) {
                        aLog("error : select");
                        return False;
                    }
                    j = 0;
                } else {
                    aLog("error : write");
                    return False;
                }
            } else {
                aLog("write failed\n");
                return False;
            }
        }
        i += j;
    }
    return True;
}


/*
 * ConnectToTcpAddr connects to the given TCP port.
 */


int
ConnectToTcpAddr(unsigned int host, int port)
{
    int sock;
    struct sockaddr_in addr;
    int one = 1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = host;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        aLog("ConnectToTcpAddr: socket\n" );
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        aLog("ConnectToTcpAddr: connect");
        close(sock);
        return -1;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                   (char *)&one, sizeof(one)) < 0) {
        aLog("ConnectToTcpAddr: setsockopt");
        close(sock);
        return -1;
    }

    return sock;
}



/*
 * ListenAtTcpPort starts listening at the given TCP port.
 */

int
ListenAtTcpPort(int port)
{
    int sock;
    struct sockaddr_in addr;
    int one = 1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
		aLog("ListenAtTcpPort: socket");
        return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                   (const char *)&one, sizeof(one)) < 0) {
        aLog("ListenAtTcpPort: setsockopt");
        close(sock);
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        aLog("ListenAtTcpPort: bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 5) < 0) {
        aLog("ListenAtTcpPort: listen");
        close(sock);
        return -1;
    }

    return sock;
}


/*
 * AcceptTcpConnection accepts a TCP connection.
 */

int
AcceptTcpConnection(int listenSock)
{
    int sock;
    struct sockaddr_in addr;
#if defined(WIN32)
    int addrlen = sizeof(addr);
#else
    socklen_t addrlen = sizeof(addr);
#endif
    int one = 1;

    sock = accept(listenSock, (struct sockaddr *) &addr, &addrlen);
    if (sock < 0) {
		aLog("AcceptTcpConnection: accept");
        return -1;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                   (char *)&one, sizeof(one)) < 0) {
        aLog("AcceptTcpConnection: setsockopt");
        close(sock);
        return -1;
    }

    return sock;
}


/*
 * SetNonBlocking sets a socket into non-blocking mode.
 */

Bool
SetNonBlocking(int sock)
{
#if defined (WIN32)
	unsigned long one = 1;
	if (ioctlsocket (sock, FIONBIO, &one) )
	{
		aLog("SetNonBlocking: fcntl");
		return False;
	}
	return True;
#else
#ifdef O_ASYNC 
    if (fcntl(sock, F_SETFL, O_ASYNC) < 0) {
#else
    int yes = 1; 
    if (ioctl(sock, FIOASYNC, &yes) < 0) {
#endif
        aLog("SetNonBlocking: fcntl");
        return False;
    }
    return True;
#endif
}


/*
 * StringToIPAddr - convert a host string to an IP address.
 */

Bool
StringToIPAddr(const char *str, unsigned long *addr)
{
    struct hostent *hp;

    if (strcmp(str,"") == 0) {
        *addr = 0; /* local */
        return True;
    }

    *addr = inet_addr(str);

	aLog("inet_addr %s = %d\n", str, *addr);

    if (*addr != INADDR_NONE)
        return True;

    hp = gethostbyname(str);

	aLog("gethostbyname %s = %x\n", str, hp);

    if (hp) {
        *addr = *(unsigned int *)hp->h_addr;
        return True;
    }

    return False;
}


/*
 * Test if the other end of a socket is on the same machine.
 */

Bool
SameMachine(int sock)
{
    struct sockaddr_in peeraddr, myaddr;
#if defined (WIN32)
    int addrlen = sizeof(struct sockaddr_in);
#else
    socklen_t addrlen = sizeof(struct sockaddr_in);
#endif

    getpeername(sock, (struct sockaddr *)&peeraddr, &addrlen);
    getsockname(sock, (struct sockaddr *)&myaddr, &addrlen);

    return (peeraddr.sin_addr.s_addr == myaddr.sin_addr.s_addr);
}
