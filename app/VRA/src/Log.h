/*--------------------------------------------------------------------------*/
/* Volume Rendering Application                                             */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                 */
/*                                                                          */
/* This software is free software; you can redistribute it and/or modify it */
/* under the terms of the GNU Lesser General Public License as published by */
/* the Free Software Foundation; either Version 2.1 of the License, or      */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This software is distributed in the hope that it will be useful, but     */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser  */
/* General Public License for more details.                                 */
/*                                                                          */
/* You should have received a copy of the GNU Lesser Public License along   */
/* with this library; if not, write to the Free Software Foundation, Inc.,  */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                    */
/*--------------------------------------------------------------------------*/

#ifndef LOG_H
#define LOG_H

/*--------------------------------------------------------------------------*/

#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/*--------------------------------------------------------------------------*/

class Log {

public:

  // Print message and time to standard output
  void static Assert(const char* message);

  // Start log
  void static Start();

  // Start log with sufix
  void static Start(char* sufix);

  // Stop log
  void static Stop();

  // Message count
  static int logCount;

  // Log file pointer
  static FILE* logFile;

  // Mutex
  static pthread_mutex_t logMutex;

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
