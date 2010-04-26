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

#include "Log.h"

/*--------------------------------------------------------------------------*/

// Define logCount
int Log::logCount = 0;

// Define logFile
FILE* Log::logFile = NULL;

// Define logMutex
pthread_mutex_t Log::logMutex = PTHREAD_MUTEX_INITIALIZER;

/*--------------------------------------------------------------------------*/

void Log::Assert(const char* message) {

#ifdef LOG

  // Rank
  int rank = 0;

  // Get rank (This is a hack)
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Clock time
  struct timeval clockTime;

  // Get time
  gettimeofday(&clockTime, NULL);

  // Convert time to microseconds
  long long t = ((long long) clockTime.tv_sec * (long long) 1000000) + 
    (long long) clockTime.tv_usec;

  // Lock mutex
  pthread_mutex_lock(&Log::logMutex);

  // Print message and time to standard output
  //fprintf(stderr, 
  //      "[%d %d Log - Time: %ds %ldus %s]\n",
  //      rank, Log::logCount++, clockTime.tv_sec, clockTime.tv_usec, message);
  if (Log::logFile != NULL) {
    fprintf(logFile, 
            "[%d %d Log - Time: %lldus %s]\n",
            rank, Log::logCount++, t, message);
  }

  // Unlock mutex
  pthread_mutex_unlock(&Log::logMutex);

#endif

}

/*--------------------------------------------------------------------------*/

void Log::Start() {

#ifdef LOG

  // Rank
  int rank = 0;

  // Get rank (This is a hack)
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Clock time
  struct timeval clockTime;

  // Get time
  gettimeofday(&clockTime, NULL);

  // Convert time to microseconds
  long long t = ((long long) clockTime.tv_sec * (long long) 1000000) + 
    (long long) clockTime.tv_usec;

  // Create filename
  char filename[256];
  memset(filename, 0, 256);
  sprintf(filename, 
          "/pvfs2/evl/schwarz/benchmarks/VRA_LOG_%d_%lld", 
          rank, t);

  // Attempt to open log file
  if ((Log::logFile = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "Log: Can't open log file.\n");
  }

#endif

}

/*--------------------------------------------------------------------------*/

void Log::Start(char* sufix) {

#ifdef LOG

  // Rank
  int rank = 0;

  // Get rank (This is a hack)
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Clock time
  struct timeval clockTime;

  // Get time
  gettimeofday(&clockTime, NULL);

  // Convert time to microseconds
  long long t = ((long long) clockTime.tv_sec * (long long) 1000000) +
    (long long) clockTime.tv_usec;

  // Create filename
  char filename[512];
  memset(filename, 0, 512);
  sprintf(filename, 
          "/pvfs2/evl/schwarz/benchmarks/VRA_LOG_%d_%lld_%s", 
          rank, t, sufix);

  // Attempt to open log file
  if ((Log::logFile = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "Log: Can't open log file.\n");
  }

#endif                        

}

/*--------------------------------------------------------------------------*/

void Log::Stop() {

#ifdef LOG

  // Close log file
  if (Log::logFile != NULL) {
    fclose(Log::logFile);
  }

#endif

}

/*--------------------------------------------------------------------------*/
