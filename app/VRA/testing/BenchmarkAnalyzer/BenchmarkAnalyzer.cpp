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

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <vector.h>

#include "Operation.h"

/*--------------------------------------------------------------------------*/

#define ERROR 1
#define OK 0

/*--------------------------------------------------------------------------*/

// Brick size
int _brickSize[3];

// Size of CacheRAM
int _cacheRAMSize = 0;

// Size of CacheVRAM
int _cacheVRAMSize = 0;

// Input file pointer
FILE* _inputFilePtr = NULL;

// Number of LODs
int _numberOfLOD = 0;

// Number of processes
int _numberOfProcesses = 0;

// Vector to hold number of cacheRAM hits for each LOD for each process
vector<long long>* _cacheRAMHits = NULL;

// Vector to hold number of cacheRAM misses for each LOD for each process
vector<long long>* _cacheRAMMisses = NULL;

// Vector to hold number of cacheVRAM hits for each LOD for each process
vector<long long>* _cacheVRAMHits = NULL;

// Vector to hold number of cacheVRAM misses for each LOD for each process
vector<long long>* _cacheVRAMMisses = NULL;

// Vector to hold time to load data for each LOD for each process
vector<long long>* _loadDataTimePerLOD = NULL;

// Vector to hold time to load textures for each LOD for each process
vector<long long>* _loadTextureTimePerLOD = NULL;

// Operation vector to hold operations for each process
vector<Operation*>* _operationVector = NULL;

// Vector to hold render time for all LODs for each process
vector<long long>* _renderTimeAllLOD = NULL;

// Vector to hold render time for each brick for each LOD for each process
vector<long long>* _renderTimeBrickPerLOD = NULL;

// Vector to hold render time for each LOD for each process
vector<long long>* _renderTimePerLOD = NULL;

/*--------------------------------------------------------------------------*/

int allocate_storage() {

  // Allocate memory for cacheRAM hits for each LOD for each process
  _cacheRAMHits = new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_cacheRAMHits == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for cacheRAM misses for each LOD for each process
  _cacheRAMMisses = new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_cacheRAMMisses == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for cacheVRAM hits for each LOD for each process
  _cacheVRAMHits = new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_cacheVRAMHits == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for cacheVRAM misses for each LOD for each process
  _cacheVRAMMisses = new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_cacheVRAMMisses == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for time to load data for each LOD for each process
  _loadDataTimePerLOD = 
    new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_loadDataTimePerLOD == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for time to load textures for each LOD for each process
  _loadTextureTimePerLOD = 
    new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_loadTextureTimePerLOD == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for operations from each process
  _operationVector = new vector<Operation*>[_numberOfProcesses];

  // Check for error
  if (_operationVector == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for render time for all LODs for each process
  _renderTimeAllLOD = new vector<long long>[_numberOfProcesses];

  // Check for error
  if (_renderTimeAllLOD == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for render time for each LOD for each process
  _renderTimeBrickPerLOD = 
    new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_renderTimeBrickPerLOD == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Allocate memory for render time for each LOD for each process
  _renderTimePerLOD = new vector<long long>[_numberOfProcesses * _numberOfLOD];

  // Check for error
  if (_renderTimePerLOD == NULL) {
    fprintf(stderr, "\nMemory allocation error\n");
    return ERROR;
  }


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int analyze_all() {

  fprintf(stdout,
          "---------------------------------------------------------------\n");
  fflush(stdout);

  long long rhits = 0;
  long long rmisses = 0;
  long long vrhits = 0;
  long long vrmisses = 0;

  long double averageLoadDataTime = 0.0;
  long long numberOfLoadDataTimeEntries = 0;
  long double stdDevLoadDataTime = 0.0;

  long double averageLoadTextureTime = 0.0;
  long long numberOfLoadTextureTimeEntries = 0;
  long double stdDevLoadTextureTime = 0.0;

  long double averageRenderTimeAllLOD = 0.0;
  long long numberOfRenderTimeAllLODEntries = 0;
  long double stdDevRenderTimeAllLOD = 0.0;

  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ;  
           i < _cacheRAMHits[(proc*_numberOfLOD)+level].size() ;
           i++) {
        rhits += _cacheRAMHits[(proc*_numberOfLOD)+level].at(i);
      }
    }
  }
  fprintf(stdout, 
          "All CacheRAM Hits:                     %lld\n", 
          rhits);

  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ;  
           i < _cacheRAMMisses[(proc*_numberOfLOD)+level].size() ;
           i++) {
        rmisses += _cacheRAMMisses[(proc*_numberOfLOD)+level].at(i);
      }
    }
  }
  fprintf(stdout, 
          "All CacheRAM Misses:                   %lld\n", 
          rmisses);

  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ;  
           i < _cacheVRAMHits[(proc*_numberOfLOD)+level].size() ;
           i++) {
        vrhits += _cacheVRAMHits[(proc*_numberOfLOD)+level].at(i);
      }
    }
  }
  fprintf(stdout, 
          "All CacheVRAM Hits:                    %lld\n", 
          vrhits);

  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ;  
           i < _cacheVRAMMisses[(proc*_numberOfLOD)+level].size() ;
           i++) {
        vrmisses += _cacheVRAMMisses[(proc*_numberOfLOD)+level].at(i);
      }
    }
  }
  fprintf(stdout, 
          "All CacheVRAM Misses:                  %lld\n", 
          vrmisses);


  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ;  
           i < _loadDataTimePerLOD[(proc*_numberOfLOD)+level].size() ;
           i++) {
        averageLoadDataTime += 
          (long double) _loadDataTimePerLOD[(proc*_numberOfLOD)+level].at(i);
        numberOfLoadDataTimeEntries++;
      }
    }
  }
  averageLoadDataTime /= (long double) numberOfLoadDataTimeEntries;
  fprintf(stdout,
          "Number of data loads:                  %lld\n",
          numberOfLoadDataTimeEntries);
  fprintf(stdout, 
          "All average load data time:            %Lfus", 
          averageLoadDataTime);

  for (int p = 1 ; p < _numberOfProcesses ; p++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ; 
           i < _loadDataTimePerLOD[(p*_numberOfLOD)+level].size() ; 
           i++) {
        stdDevLoadDataTime += 
          ((long double) _loadDataTimePerLOD[(p*_numberOfLOD)+level].at(i)- 
           averageLoadDataTime) *
          ((long double) _loadDataTimePerLOD[(p*_numberOfLOD)+level].at(i)- 
           averageLoadDataTime);
      }
    }
  }
  stdDevLoadDataTime /= (long double) (numberOfLoadDataTimeEntries-1);
  stdDevLoadDataTime = sqrt(stdDevLoadDataTime);
  fprintf(stdout, 
          " sd: %Lfus\n", 
          stdDevLoadDataTime);


  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ;  
           i < _loadTextureTimePerLOD[(proc*_numberOfLOD)+level].size() ;
           i++) {
        averageLoadTextureTime += 
          (long double) _loadTextureTimePerLOD[(proc*_numberOfLOD)+level].
          at(i);
        numberOfLoadTextureTimeEntries++;
      }
    }
  }
  averageLoadTextureTime /= (long double) numberOfLoadTextureTimeEntries;
  fprintf(stdout,
          "Number of texture loads:               %lld\n",
          numberOfLoadTextureTimeEntries);
  fprintf(stdout, 
          "All average load texture time:         %Lfus", 
          averageLoadTextureTime);

  for (int p = 1 ; p < _numberOfProcesses ; p++) {
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int i = 0 ; 
           i < _loadTextureTimePerLOD[(p*_numberOfLOD)+level].size() ; 
           i++) {
        stdDevLoadTextureTime += 
          ((long double) _loadTextureTimePerLOD[(p*_numberOfLOD)+level].at(i)- 
           averageLoadTextureTime) *
          ((long double) _loadTextureTimePerLOD[(p*_numberOfLOD)+level].at(i)- 
           averageLoadTextureTime);
      }
    }
  }
  stdDevLoadTextureTime /= (long double) (numberOfLoadTextureTimeEntries-1);
  stdDevLoadTextureTime = sqrt(stdDevLoadTextureTime);
  fprintf(stdout, 
          " sd: %Lfus\n", 
          stdDevLoadTextureTime);


  for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
    for (int i = 0 ; i < _renderTimeAllLOD[proc].size() ; i++) {
      averageRenderTimeAllLOD += (long double) _renderTimeAllLOD[proc].at(i);
      numberOfRenderTimeAllLODEntries++;
    }    
  }
  averageRenderTimeAllLOD /= (long double) numberOfRenderTimeAllLODEntries;
  fprintf(stdout,
          "Number of complete frames:             %lld\n",
          numberOfRenderTimeAllLODEntries);
  fprintf(stdout, 
          "All render complete frame time:        %Lfus", 
          averageRenderTimeAllLOD);


  for (int p = 1 ; p < _numberOfProcesses ; p++) {
    for (int i = 0 ; i < _renderTimeAllLOD[p].size() ; i++) {
      stdDevRenderTimeAllLOD += 
        ((long double) _renderTimeAllLOD[p].at(i) - averageRenderTimeAllLOD) *
        ((long double) _renderTimeAllLOD[p].at(i) - averageRenderTimeAllLOD);
    }
  }
  stdDevRenderTimeAllLOD /= (long double) (numberOfRenderTimeAllLODEntries-1);
  stdDevRenderTimeAllLOD = sqrt(stdDevRenderTimeAllLOD);
  fprintf(stdout, 
          " sd: %Lfus\n", 
          stdDevRenderTimeAllLOD);

  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int analyze_each_level() {

  fprintf(stdout,
          "---------------------------------------------------------------\n");
  fflush(stdout);

  for (int level = 0 ; level < _numberOfLOD ; level++) {

    fprintf(stdout, "Level: %d\n", level);
    fflush(stdout);

    long long rhits = 0;
    long long rmisses = 0;
    long long vrhits = 0;
    long long vrmisses = 0;

    long double averageLoadDataTime = 0.0;
    long long numberOfLoadDataTimeEntries = 0;
    long double stdDevLoadDataTime = 0.0;

    long double averageLoadTextureTime = 0.0;
    long long numberOfLoadTextureTimeEntries = 0;
    long double stdDevLoadTextureTime = 0.0;

    long double averageRenderTimeAllLOD = 0.0;
    long long numberOfRenderTimeAllLODEntries = 0;
    long double stdDevRenderTimeAllLOD = 0.0;

    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int i = 0 ;  
           i < _cacheRAMHits[(proc*_numberOfLOD)+level].size() ;
           i++) {
        rhits += _cacheRAMHits[(proc*_numberOfLOD)+level].at(i);
      }
    }
    fprintf(stdout, 
            "  CacheRAM Hits:                     %lld\n", 
            rhits);

    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int i = 0 ;  
           i < _cacheRAMMisses[(proc*_numberOfLOD)+level].size() ;
           i++) {
        rmisses += _cacheRAMMisses[(proc*_numberOfLOD)+level].at(i);
      }
    }
    fprintf(stdout, 
            "  CacheRAM Misses:                   %lld\n", 
            rmisses);

    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int i = 0 ;  
           i < _cacheVRAMHits[(proc*_numberOfLOD)+level].size() ;
           i++) {
        vrhits += _cacheVRAMHits[(proc*_numberOfLOD)+level].at(i);
      }
    }
    fprintf(stdout, 
            "  CacheVRAM Hits:                    %lld\n", 
            vrhits);

    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int i = 0 ;  
           i < _cacheVRAMMisses[(proc*_numberOfLOD)+level].size() ;
           i++) {
        vrmisses += _cacheVRAMMisses[(proc*_numberOfLOD)+level].at(i);
      }
    }
    fprintf(stdout, 
            "  CacheVRAM Misses:                  %lld\n", 
            vrmisses);


    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int j = 0 ; 
           j < _loadDataTimePerLOD[(proc * _numberOfLOD) + level].size() ; 
           j++) {
        averageLoadDataTime += 
          (long double) _loadDataTimePerLOD[(proc * _numberOfLOD) + level].
          at(j);
      }
      numberOfLoadDataTimeEntries += 
        _loadDataTimePerLOD[(proc * _numberOfLOD) + level].size();
    }
    averageLoadDataTime /= (long double) numberOfLoadDataTimeEntries;
    
    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int j = 0 ; 
           j < _loadDataTimePerLOD[(proc * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevLoadDataTime += 
          ((long double) _loadDataTimePerLOD[(proc*_numberOfLOD) + level]
           .at(j) - averageLoadDataTime) *
          ((long double) _loadDataTimePerLOD[(proc*_numberOfLOD) + level]
           .at(j) - averageLoadDataTime);
      }
    }
    stdDevLoadDataTime /= (long double) (numberOfLoadDataTimeEntries - 1);
    stdDevLoadDataTime = sqrt(stdDevLoadDataTime);

    // Print 
    fprintf(stdout, 
            "  Average load data time:            %Lfus  ", 
            averageLoadDataTime);
    fprintf(stdout, 
            "sd: %Lfus\n", 
            stdDevLoadDataTime);


    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int j = 0 ; 
           j < _loadTextureTimePerLOD[(proc * _numberOfLOD) + level].size() ; 
           j++) {
        averageLoadTextureTime += 
          (long double) _loadTextureTimePerLOD[(proc * _numberOfLOD) + level].
          at(j);
      }
      numberOfLoadTextureTimeEntries += 
        _loadTextureTimePerLOD[(proc * _numberOfLOD) + level].size();
    }
    averageLoadTextureTime /= (long double) numberOfLoadTextureTimeEntries;
    
    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int j = 0 ; 
           j < _loadTextureTimePerLOD[(proc * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevLoadTextureTime += 
          ((long double) _loadTextureTimePerLOD[(proc*_numberOfLOD) + level]
           .at(j) - averageLoadTextureTime) *
          ((long double) _loadTextureTimePerLOD[(proc*_numberOfLOD) + level]
           .at(j) - averageLoadTextureTime);
      }
    }
    stdDevLoadTextureTime /= (long double) (numberOfLoadTextureTimeEntries- 1);
    stdDevLoadTextureTime = sqrt(stdDevLoadTextureTime);

    // Print 
    fprintf(stdout, 
            "  Average load texture time:         %Lfus  ", 
            averageLoadTextureTime);
    fprintf(stdout, 
            "sd: %Lfus\n", 
            stdDevLoadTextureTime);


    // Average render time per frame for this level
    long double averageRenderTimePerLOD = 0.0;
    long long numPerLOD = 0;
    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int j = 0 ; 
           j < _renderTimePerLOD[(proc * _numberOfLOD) + level].size() ; 
           j++) {
        averageRenderTimePerLOD += 
          (long double) _renderTimePerLOD[(proc * _numberOfLOD) + level].at(j);
        numPerLOD++;
      }
    }
    averageRenderTimePerLOD /= 
      (long double) numPerLOD;

    // Standard deviation of the render time per frame for this level
    long double stdDevRenderTimePerLOD = 0.0;
    for (int proc = 1 ; proc < _numberOfProcesses ; proc++) {
      for (int j = 0 ; 
           j < _renderTimePerLOD[(proc * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevRenderTimePerLOD += 
          ((long double) _renderTimePerLOD[(proc*_numberOfLOD)+level].at(j) - 
           averageRenderTimePerLOD) *
          ((long double) _renderTimePerLOD[(proc*_numberOfLOD)+level].at(j) - 
           averageRenderTimePerLOD);
      }
    }
    stdDevRenderTimePerLOD /= 
      (long double) (numPerLOD - 1);
    stdDevRenderTimePerLOD = sqrt(stdDevRenderTimePerLOD);
    
    // Print 
    fprintf(stdout, 
            "  Average time to render:            %Lfus  ", 
            averageRenderTimePerLOD);
    fprintf(stdout, 
            "sd: %Lfus\n", 
            stdDevRenderTimePerLOD);
    
  }

  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int analyze_each_process() {

  fprintf(stdout,
          "---------------------------------------------------------------\n");
  fflush(stdout);

  // One process at a time
  for (int i = 1 ; i < _numberOfProcesses ; i++) {

    // Print process number
    fprintf(stdout, "Process number:                  %d\n", i);

    // Print number of complete frames
    fprintf(stdout, "Number of complete frames:       %d\n", 
            _renderTimeAllLOD[i].size());

    // Average render time per frame for all LODs
    long double averageRenderTimeAllLOD = 0.0;
    for (int j = 0 ; j < _renderTimeAllLOD[i].size() ; j++) {
      averageRenderTimeAllLOD += (long double) _renderTimeAllLOD[i].at(j);
    }
    averageRenderTimeAllLOD /= (long double) _renderTimeAllLOD[i].size();

    // Standard deviation of the render time per frame for all LODs
    long double stdDevRenderTimeAllLOD = 0.0;
    for (int j = 0 ; j < _renderTimeAllLOD[i].size() ; j++) {
      stdDevRenderTimeAllLOD += 
        ((long double) _renderTimeAllLOD[i].at(j) - averageRenderTimeAllLOD) *
        ((long double) _renderTimeAllLOD[i].at(j) - averageRenderTimeAllLOD);
    }
    stdDevRenderTimeAllLOD /= (long double) (_renderTimeAllLOD[i].size() - 1);
    stdDevRenderTimeAllLOD = sqrt(stdDevRenderTimeAllLOD);

    // Print average time to render all LODs of a frame
    fprintf(stdout, 
            "Average time to render all LODs:                  %Lfus\n", 
            averageRenderTimeAllLOD);

    // Print standard deviation of time to render all LODs of a frame
    fprintf(stdout, 
            "Standard deviation of time to render all LODs:    %Lfus\n", 
            stdDevRenderTimeAllLOD);


    // Average time to load data for this process
    long double averageLoadDataTimePerProcess = 0.0;
    long long numberOfLoadDataTimeEntries = 0;
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _loadDataTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        averageLoadDataTimePerProcess += 
          (long double) _loadDataTimePerLOD[(i * _numberOfLOD) + level].
          at(j);
      }
      numberOfLoadDataTimeEntries += 
        _loadDataTimePerLOD[(i * _numberOfLOD) + level].size();
    }
    averageLoadDataTimePerProcess /= 
      (long double) numberOfLoadDataTimeEntries;

    // Standard deviation of the load data time for this process
    long double stdDevLoadDataTimePerProcess = 0.0;
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _loadDataTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevLoadDataTimePerProcess += 
          ((long double) _loadDataTimePerLOD[(i*_numberOfLOD) + level].at(j) - 
           averageLoadDataTimePerProcess) *
          ((long double) _loadDataTimePerLOD[(i*_numberOfLOD) + level].at(j) - 
           averageLoadDataTimePerProcess);
      }
    }
    stdDevLoadDataTimePerProcess /= 
      (long double) (numberOfLoadDataTimeEntries - 1);
    stdDevLoadDataTimePerProcess = sqrt(stdDevLoadDataTimePerProcess);

    // Print 
    fprintf(stdout, 
            "Average load data time:                           %Lfus\n", 
            averageLoadDataTimePerProcess);
    fprintf(stdout, 
            "Standard deviation of average load data time:     %Lfus\n", 
            stdDevLoadDataTimePerProcess);


    // Average time to load texture for this process
    long double averageLoadTextureTimePerProcess = 0.0;
    long long numberOfLoadTextureTimeEntries = 0;
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _loadTextureTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        averageLoadTextureTimePerProcess += 
          (long double) _loadTextureTimePerLOD[(i * _numberOfLOD) + level].
          at(j);
      }
      numberOfLoadTextureTimeEntries += 
        _loadTextureTimePerLOD[(i * _numberOfLOD) + level].size();
    }
    averageLoadTextureTimePerProcess /= 
      (long double) numberOfLoadTextureTimeEntries;

    // Standard deviation of the load texture time for this process
    long double stdDevLoadTextureTimePerProcess = 0.0;
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _loadTextureTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevLoadTextureTimePerProcess += 
          ((long double) _loadTextureTimePerLOD[(i*_numberOfLOD)+level].at(j)- 
           averageLoadTextureTimePerProcess) *
          ((long double) _loadTextureTimePerLOD[(i*_numberOfLOD)+level].at(j)- 
           averageLoadTextureTimePerProcess);
      }
    }
    stdDevLoadTextureTimePerProcess /= 
      (long double) (numberOfLoadTextureTimeEntries - 1);
    stdDevLoadTextureTimePerProcess = sqrt(stdDevLoadTextureTimePerProcess);

    // Print 
    fprintf(stdout, 
            "Average load texture time:                        %Lfus\n", 
            averageLoadTextureTimePerProcess);
    fprintf(stdout, 
            "Standard deviation of average load texture time:  %Lfus\n", 
            stdDevLoadTextureTimePerProcess);


    // Cache (V)RAM hits and misses
    long long trhits = 0;
    long long trmisses = 0;
    long long tvrhits = 0;
    long long tvrmisses = 0;

    // Calculate all cacheRAM hits for this process
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _cacheRAMHits[(i * _numberOfLOD) + level].size() ; 
           j++) {
        trhits += _cacheRAMHits[(i * _numberOfLOD) + level].at(j);
      }
    }

    // Calculate all cacheRAM misses for this process
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _cacheRAMMisses[(i * _numberOfLOD) + level].size() ; 
           j++) {
        trmisses += _cacheRAMMisses[(i * _numberOfLOD) + level].at(j);
      }
    }

    // Print 
    fprintf(stdout, 
            "CacheRAM Hits:                                    %lld\n", 
            trhits);
    fprintf(stdout, 
            "CacheRAM Misses:                                  %lld\n", 
            trmisses);

    // Calculate all cacheVRAM hits for this process
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _cacheVRAMHits[(i * _numberOfLOD) + level].size() ; 
           j++) {
        tvrhits += _cacheVRAMHits[(i * _numberOfLOD) + level].at(j);
      }
    }

    // Calculate all cacheVRAM misses for this process
    for (int level = 0 ; level < _numberOfLOD ; level++) {
      for (int j = 0 ; 
           j < _cacheVRAMMisses[(i * _numberOfLOD) + level].size() ; 
           j++) {
        tvrmisses += _cacheVRAMMisses[(i * _numberOfLOD) + level].at(j);
      }
    }

    // Print 
    fprintf(stdout, 
            "CacheVRAM Hits:                                   %lld\n", 
            tvrhits);
    fprintf(stdout, 
            "CacheVRAM Misses:                                 %lld\n", 
            tvrmisses);


    // One level at a time
    for (int level = 0 ; level < _numberOfLOD ; level++) {

      // Print level number
      fprintf(stdout, "  Level number:                  %d\n", level);

      // Cache (V)RAM hits and misses
      long long rhits = 0;
      long long rmisses = 0;
      long long vrhits = 0;
      long long vrmisses = 0;

      // Calculate cacheRAM hits
      for (int j = 0 ; 
           j < _cacheRAMHits[(i * _numberOfLOD) + level].size() ; 
           j++) {
        rhits += _cacheRAMHits[(i * _numberOfLOD) + level].at(j);
      }

      // Calculate cacheRAM misses
      for (int j = 0 ; 
           j < _cacheRAMMisses[(i * _numberOfLOD) + level].size() ; 
           j++) {
        rmisses += _cacheRAMMisses[(i * _numberOfLOD) + level].at(j);
      }

      // Calculate cacheVRAM hits
      for (int j = 0 ; 
           j < _cacheVRAMHits[(i * _numberOfLOD) + level].size() ; 
           j++) {
        vrhits += _cacheVRAMHits[(i * _numberOfLOD) + level].at(j);
      }

      // Calculate cacheVRAM misses
      for (int j = 0 ; 
           j < _cacheVRAMMisses[(i * _numberOfLOD) + level].size() ; 
           j++) {
        vrmisses += _cacheVRAMMisses[(i * _numberOfLOD) + level].at(j);
      }

      // Print 
      fprintf(stdout, "  CacheRAM Hits:                 %lld\n", rhits);
      fprintf(stdout, "  CacheRAM Misses:               %lld\n", rmisses);
      fprintf(stdout, "  CacheVRAM Hits:                %lld\n", vrhits);
      fprintf(stdout, "  CacheVRAM Misses:              %lld\n", vrmisses);


      // Average time to load data for this level
      long double averageLoadDataTimePerLOD = 0.0;
      for (int j = 0 ; 
           j < _loadDataTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        averageLoadDataTimePerLOD += 
          (long double) _loadDataTimePerLOD[(i * _numberOfLOD) + level].
          at(j);
      }
      averageLoadDataTimePerLOD /= 
        (long double) _loadDataTimePerLOD[(i * _numberOfLOD) + level].size();

      // Standard deviation of the load data time for this level
      long double stdDevLoadDataTimePerLOD = 0.0;
      for (int j = 0 ; 
           j < _loadDataTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevLoadDataTimePerLOD += 
          ((long double) _loadDataTimePerLOD[(i*_numberOfLOD) + level].at(j) - 
           averageLoadDataTimePerLOD) *
          ((long double) _loadDataTimePerLOD[(i*_numberOfLOD) + level].at(j) - 
           averageLoadDataTimePerLOD);
      }
      stdDevLoadDataTimePerLOD /= 
        (long double) (_loadDataTimePerLOD[(i*_numberOfLOD)+level].size() - 1);
      stdDevLoadDataTimePerLOD = sqrt(stdDevLoadDataTimePerLOD);

      // Print 
      fprintf(stdout, 
              "  Average load data time:                           %Lfus\n", 
              averageLoadDataTimePerLOD);
      fprintf(stdout, 
              "  Standard deviation of average load data time:     %Lfus\n", 
              stdDevLoadDataTimePerLOD);


      // Average time to load texture for this level
      long double averageLoadTextureTimePerLOD = 0.0;
      for (int j = 0 ; 
           j < _loadTextureTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        averageLoadTextureTimePerLOD += 
          (long double) _loadTextureTimePerLOD[(i*_numberOfLOD) + level].
          at(j);
      }
      averageLoadTextureTimePerLOD /= 
        (long double) _loadTextureTimePerLOD[(i*_numberOfLOD) + level].size();

      // Standard deviation of the load texture time for this level
      long double stdDevLoadTextureTimePerLOD = 0.0;
      for (int j = 0 ; 
           j < _loadTextureTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevLoadTextureTimePerLOD += 
          ((long double) _loadTextureTimePerLOD[(i*_numberOfLOD)+level].at(j)- 
           averageLoadTextureTimePerLOD) *
          ((long double) _loadTextureTimePerLOD[(i*_numberOfLOD)+level].at(j)- 
           averageLoadTextureTimePerLOD);
      }
      stdDevLoadTextureTimePerLOD /= 
        (long double)(_loadTextureTimePerLOD[(i*_numberOfLOD)+level].size()-1);
      stdDevLoadTextureTimePerLOD = sqrt(stdDevLoadTextureTimePerLOD);

      // Print 
      fprintf(stdout, 
              "  Average load texture time:                        %Lfus\n", 
              averageLoadTextureTimePerLOD);
      fprintf(stdout, 
              "  Standard deviation of average load texture time:  %Lfus\n", 
              stdDevLoadTextureTimePerLOD);


      // Average render time per frame for this level
      long double averageRenderTimePerLOD = 0.0;
      for (int j = 0 ; 
           j < _renderTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        averageRenderTimePerLOD += 
          (long double) _renderTimePerLOD[(i * _numberOfLOD) + level].at(j);
      }
      averageRenderTimePerLOD /= 
        (long double) _renderTimePerLOD[(i * _numberOfLOD) + level].size();

      // Standard deviation of the render time per frame for this level
      long double stdDevRenderTimePerLOD = 0.0;
      for (int j = 0 ; 
           j < _renderTimePerLOD[(i * _numberOfLOD) + level].size() ; 
           j++) {
        stdDevRenderTimePerLOD += 
          ((long double) _renderTimePerLOD[(i * _numberOfLOD) + level].at(j) - 
           averageRenderTimePerLOD) *
          ((long double) _renderTimePerLOD[(i * _numberOfLOD) + level].at(j) - 
           averageRenderTimePerLOD);
      }
      stdDevRenderTimePerLOD /= 
        (long double) (_renderTimeAllLOD[(i*_numberOfLOD)+level].size() - 1);
      stdDevRenderTimePerLOD = sqrt(stdDevRenderTimePerLOD);

      // Print 
      fprintf(stdout, 
              "  Average time to render:                           %Lfus\n", 
              averageRenderTimePerLOD);
      fprintf(stdout, 
              "  Standard deviation of average render time:        %Lfus\n", 
              stdDevRenderTimePerLOD);

    }

  }

  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int brick_size() {

  // Line
  char* line = NULL;

  // Line length
  size_t len = 0;

  // Size of line read
  ssize_t read;


  // Check for valid file pointer
  if (_inputFilePtr == NULL) {

    // Return error
    return ERROR;

  }


  // Nice message
  fprintf(stdout, "Determining brick size... ");
  fflush(stdout);


  // Read each line looking for brick size
  while ((read = getline(&line, &len, _inputFilePtr)) != -1) {
    
    // Temporary operation object
    Operation test(line);

    // Find an operation that has brick width
    if ((!strcmp(test.GetClassName(), "OctreeNode")) &&
        (strstr(test.GetInfo(), "W:") != NULL)) {
      
      // Find width
      char* info = test.GetInfo();
      char tmp[128];
      memset(tmp, 0, 128);
      int j = 0;      
      for (int i = 0 ; i < strlen(info) ; i++) {
        char c = info[i];
        if (isdigit(c)) {
          tmp[j++] = c;
        }
      }

      // Convert to decimal
      _brickSize[0] = atoi(tmp);

    }

    // Find an operation that has brick height
    if ((!strcmp(test.GetClassName(), "OctreeNode")) &&
        (strstr(test.GetInfo(), "H:") != NULL)) {
      
      // Find width
      char* info = test.GetInfo();
      char tmp[128];
      memset(tmp, 0, 128);
      int j = 0;      
      for (int i = 0 ; i < strlen(info) ; i++) {
        char c = info[i];
        if (isdigit(c)) {
          tmp[j++] = c;
        }
      }

      // Convert to decimal
      _brickSize[1] = atoi(tmp);

    }

    // Find an operation that has brick height
    if ((!strcmp(test.GetClassName(), "OctreeNode")) &&
        (strstr(test.GetInfo(), "D:") != NULL) &&
        (strstr(test.GetInfo(), "ID:") == NULL)) {
      
      // Find width
      char* info = test.GetInfo();
      char tmp[128];
      memset(tmp, 0, 128);
      int j = 0;      
      for (int i = 0 ; i < strlen(info) ; i++) {
        char c = info[i];
        if (isdigit(c)) {
          tmp[j++] = c;
        }
      }

      // Convert to decimal
      _brickSize[2] = atoi(tmp);

    }

  }


  // Nice message
  fprintf(stdout, "%d x %d x %d\n", 
          _brickSize[0], _brickSize[1], _brickSize[2]);
  fflush(stdout);


  // Set file pointer to beginning of file
  rewind(_inputFilePtr);


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int cacheRAM_size() {

  // Line
  char* line = NULL;

  // Line length
  size_t len = 0;

  // Size of line read
  ssize_t read;


  // Check for valid file pointer
  if (_inputFilePtr == NULL) {

    // Return error
    return ERROR;

  }


  // Nice message
  fprintf(stdout, "Determining final number of bricks in CacheRAM... ");
  fflush(stdout);


  // Read each line looking for CacheRAM operations
  while ((read = getline(&line, &len, _inputFilePtr)) != -1) {
    
    // Temporary operation object
    Operation test(line);

    // Find an operation that has maximum size of CacheRAM
    if ((!strcmp(test.GetClassName(), "CacheRAM")) &&
        (strstr(test.GetInfo(), "Maximum Size") != NULL)) {
      
      // Find size
      char* info = test.GetInfo();
      char tmp[128];
      memset(tmp, 0, 128);
      int j = 0;      
      for (int i = 0 ; i < strlen(info) ; i++) {
        char c = info[i];
        if (isdigit(c)) {
          tmp[j++] = c;
        }
      }

      // Convert to decimal
      _cacheRAMSize = atoi(tmp);

    }

  }


  // Nice message
  fprintf(stdout, "%d\n", _cacheRAMSize);
  fflush(stdout);


  // Set file pointer to beginning of file
  rewind(_inputFilePtr);


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int cacheVRAM_size() {

  // Line
  char* line = NULL;

  // Line length
  size_t len = 0;

  // Size of line read
  ssize_t read;


  // Check for valid file pointer
  if (_inputFilePtr == NULL) {

    // Return error
    return ERROR;

  }


  // Nice message
  fprintf(stdout, "Determining final number of bricks in CacheVRAM... ");
  fflush(stdout);


  // Read each line looking for CacheRAM operations
  while ((read = getline(&line, &len, _inputFilePtr)) != -1) {
    
    // Temporary operation object
    Operation test(line);

    // Find an operation that has maximum size of CacheRAM
    if ((!strcmp(test.GetClassName(), "CacheVRAM")) &&
        (strstr(test.GetInfo(), "Maximum Size") != NULL)) {
      
      // Find size
      char* info = test.GetInfo();
      char tmp[128];
      memset(tmp, 0, 128);
      int j = 0;      
      for (int i = 0 ; i < strlen(info) ; i++) {
        char c = info[i];
        if (isdigit(c)) {
          tmp[j++] = c;
        }
      }

      // Convert to decimal
      _cacheVRAMSize = atoi(tmp);

    }

  }


  // Nice message
  fprintf(stdout, "%d\n", _cacheVRAMSize);
  fflush(stdout);


  // Set file pointer to beginning of file
  rewind(_inputFilePtr);


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int deallocate_storage() {

  if (_cacheRAMHits != NULL) {
    delete [] _cacheRAMHits;
  }

  if (_cacheRAMMisses != NULL) {
    delete [] _cacheRAMMisses;
  }

  if (_cacheVRAMHits != NULL) {
    delete [] _cacheVRAMHits;
  }

  if (_cacheVRAMMisses != NULL) {
    delete [] _cacheVRAMMisses;
  }

  if (_loadDataTimePerLOD != NULL) {
    delete [] _loadDataTimePerLOD;
  }

  if (_loadTextureTimePerLOD != NULL) {
    delete [] _loadTextureTimePerLOD;
  }

  if (_operationVector != NULL) {
    for (int i = 0 ; i < _numberOfProcesses ; i++) {
      for (int j = 0 ; j < _operationVector[i].size() ; j++) {
        Operation* tmp = _operationVector[i].at(j);
        if (tmp != NULL) {
          delete tmp;
        }
      }
    }
    delete [] _operationVector;
  }

  if (_renderTimeAllLOD != NULL) {
    delete [] _renderTimeAllLOD;
  }

  if (_renderTimeBrickPerLOD != NULL) {
    delete [] _renderTimeBrickPerLOD;
  }

  if (_renderTimePerLOD != NULL) {
    delete [] _renderTimePerLOD;
  }

}

/*--------------------------------------------------------------------------*/

int num_lod() {

  // Line
  char* line = NULL;

  // Line length
  size_t len = 0;

  // Size of line read
  ssize_t read;


  // Check for valid file pointer
  if (_inputFilePtr == NULL) {

    // Return error
    return ERROR;

  }


  // Nice message
  fprintf(stdout, "Determining number of LODs... ");
  fflush(stdout);


  // Read each line looking for process numbers
  while ((read = getline(&line, &len, _inputFilePtr)) != -1) {
    
    // Temporary operation object
    Operation test(line);

    // Find an operation that has a level number in it
    if ((!strcmp(test.GetClassName(), "VolumeRenderer")) &&
        (strstr(test.GetInfo(), "Render level") != NULL)) {
      
      // Find level number
      char* info = test.GetInfo();
      char tmp[128];
      int j = 0;      
      for (int i = 0 ; i < strlen(info) ; i++) {
        char c = info[i];
        if (isdigit(c)) {
          tmp[j++] = c;
        }
      }

      // Check for greatest number of LOD
      if (atoi(tmp) + 1 > _numberOfLOD) {
        _numberOfLOD = atoi(tmp) + 1;
      }

    }

  }


  // Nice message
  fprintf(stdout, "%d\n", _numberOfLOD);
  fflush(stdout);


  // Set file pointer to beginning of file
  rewind(_inputFilePtr);


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int num_processes() {

  // Line
  char* line = NULL;

  // Line length
  size_t len = 0;

  // Size of line read
  ssize_t read;


  // Check for valid file pointer
  if (_inputFilePtr == NULL) {

    // Return error
    return ERROR;

  }


  // Nice message
  fprintf(stdout, "Determining number of processes... ");
  fflush(stdout);


  // Read each line looking for process numbers
  while ((read = getline(&line, &len, _inputFilePtr)) != -1) {
      
    // Temporary operation object
    Operation test(line);

    // Check for greatest process number
    if (test.GetProcessNumber() + 1 > _numberOfProcesses) {
      _numberOfProcesses = test.GetProcessNumber() + 1;
    }

  }


  // Nice message
  fprintf(stdout, "%d\n", _numberOfProcesses);
  fflush(stdout);


  // Set file pointer to beginning of file
  rewind(_inputFilePtr);


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int open_file(char* filename) {

  // Nice message
  fprintf(stdout, "Opening input file %s... ", filename);
  fflush(stdout);

  // Attempt to open input file
  if ((_inputFilePtr = fopen(filename, "r")) == NULL) {
    fprintf(stdout, "Failed to open input file %s.\n", filename);
    return ERROR;
  }

  // Nice message
  fprintf(stdout, "Success\n");
  fflush(stdout);

  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int process() {

  // Line
  char* line = NULL;

  // Line length
  size_t len = 0;

  // Size of line read
  ssize_t read;


  // Nice message
  fprintf(stdout, "Processing... ");
  fflush(stdout);


  // Begin and end of a render operation flags for each process
  bool* renderBeginFound = new bool[_numberOfProcesses];
  bool* renderEndFound = new bool[_numberOfProcesses];
  for (int i = 0 ; i < _numberOfProcesses ; i++) {
    renderBeginFound[i] = false;
    renderEndFound[i] = false;
  }


  // Read each line
  while ((read = getline(&line, &len, _inputFilePtr)) != -1) {


    // Create new operation
    Operation* op = new Operation(line);


    // Check if it's a begin
    if ((!strcmp(op -> GetClassName(), "VolumeRenderer")) &&
        (strstr(op -> GetInfo(), "Render begin") != NULL)) {

      // Mark begin flag
      renderBeginFound[op -> GetProcessNumber()] = true;

      // Add operation to end of list
      _operationVector[op -> GetProcessNumber()].push_back(op);

    }


    // Check if it's an end
    else if ((!strcmp(op -> GetClassName(), "VolumeRenderer")) &&
             (strstr(op -> GetInfo(), "Render end") != NULL)) {      
   
      // Mark end flag
      renderEndFound[op -> GetProcessNumber()] = true;

      // Add operation to end of list
      _operationVector[op -> GetProcessNumber()].push_back(op);

    }


    // Operation is between a begin and an end; keep it
    else if (renderBeginFound[op -> GetProcessNumber()] == true && 
             renderEndFound[op -> GetProcessNumber()] == false) {

      // Add operation to end of list
      _operationVector[op -> GetProcessNumber()].push_back(op);

    }


    // If it's outside a begin and an end
    else {

      // Delete operation
      delete op;

      // Go to top of loop and process next event
      continue;

    }    


    // Get process number
    int procNum = op -> GetProcessNumber();


    // Found both a begin and an end for current process number
    if (renderBeginFound[procNum] == true && 
        renderEndFound[procNum] == true) {
      
      // Get information string
      char* opStr = 
        _operationVector[procNum].at(_operationVector[procNum].size() - 1) -> 
        GetInfo();

      // Check for a complete frame
      if(strstr(opStr, "Completed") != NULL) {


        // Time to render all LODs for a complete frame
        long long startTime = 
          (_operationVector[procNum].at(0)) 
          -> GetTime();
        long long endTime = 
          (_operationVector[procNum].at(_operationVector[procNum].size() - 1)) 
          -> GetTime();
        long long totalTime = endTime - startTime;
        _renderTimeAllLOD[procNum].push_back(totalTime);

        
        // Time to render each LOD
        // CacheRAM Hits and Misses
        // CacheVRAM Hits and Misses

        // Cache RAM hits and misses
        long long cacheRAMHits = 0;
        long long cacheRAMMisses = 0;

        // Cache VRAM hits and misses
        long long cacheVRAMHits = 0;
        long long cacheVRAMMisses = 0;

        // Current level
        int level = 0;

        // Level begin and end flags
        bool levelBegin = false;
        bool levelEnd = false;

        // Load data begin and end flags
        bool loadDataBegin = false;
        bool loadDataEnd = false;

        // Load data start and end times
        long long loadDataBeginTime = 0;
        long long loadDataEndTime = 0;

        // Load texture begin and end flags
        bool loadTextureBegin = false;
        bool loadTextureEnd = false;

        // Load texture start and end times
        long long loadTextureBeginTime = 0;
        long long loadTextureEndTime = 0;

        // Go through each operation in current frame
        for (int i = 0 ; i < _operationVector[procNum].size() ; i++) {

          // Get operation
          Operation* tmp = _operationVector[procNum].at(i);

          // Check if it's a level begin
          if ((!strcmp(tmp -> GetClassName(), "VolumeRenderer")) &&
              (strstr(tmp -> GetInfo(), "Render level") != NULL) &&
              (strstr(tmp -> GetInfo(), "begin") != NULL)) {

            // Start time
            startTime = tmp -> GetTime();

            // Mark begin flag
            levelBegin = true;

          }

          // Check if it's a level end
          else if ((!strcmp(tmp -> GetClassName(), "VolumeRenderer")) &&
                   (strstr(tmp -> GetInfo(), "Render level") != NULL) &&
                   (strstr(tmp -> GetInfo(), "end") != NULL)) {

            // End time
            endTime = tmp -> GetTime();

            // Mark end flag
            levelEnd = true;

          }

          // Operation is between a level begin and a level end; process it
          else if (levelBegin == true && levelEnd == false){

            // CacheRAM Hit
            if ((!strcmp(tmp -> GetClassName(), "CacheRAM")) &&
                (strstr(tmp -> GetInfo(), "loaded? True") != NULL)) {
              cacheRAMHits++;
            }

            // CacheRAM Miss
            else if ((!strcmp(tmp -> GetClassName(), "CacheRAM")) &&
                     (strstr(tmp -> GetInfo(), "loaded? False") != NULL)) {
              cacheRAMMisses++;
            }

            // CacheVRAM Hit
            else if ((!strcmp(tmp -> GetClassName(), "CacheVRAM")) &&
                     (strstr(tmp -> GetInfo(), "loaded? True") != NULL)) {
              cacheVRAMHits++;
            }

            // CacheVRAM Miss
            else if ((!strcmp(tmp -> GetClassName(), "CacheVRAM")) &&
                     (strstr(tmp -> GetInfo(), "loaded? False") != NULL)) {
              cacheVRAMMisses++;
            }

            // Begin of load data
            else if ((!strcmp(tmp -> GetClassName(), "OctreeNode")) &&
                     (strstr(tmp -> GetInfo(), "LoadData") != NULL) &&
                     (strstr(tmp -> GetInfo(), "begin") != NULL)) {

              // Set flag
              loadDataBegin = true;

              // Get start time
              loadDataBeginTime = tmp -> GetTime();

            }

            // End of load data
            else if ((!strcmp(tmp -> GetClassName(), "OctreeNode")) &&
                     (strstr(tmp -> GetInfo(), "LoadData") != NULL) &&
                     (strstr(tmp -> GetInfo(), "end") != NULL)) {

              // Set flag
              loadDataEnd = true;

              // Get end time
              loadDataEndTime = tmp -> GetTime();

              // Check to make sure there's a matching begin and end
              if (loadDataBegin == true && loadDataEnd == true) {

                // Save time to load data
                _loadDataTimePerLOD[(procNum * _numberOfLOD) + level].
                  push_back(loadDataEndTime - loadDataBeginTime);

              }

              // Reset flags
              loadDataBegin = false;
              loadDataEnd = false;

            }

            // Begin of load texture
            else if ((!strcmp(tmp -> GetClassName(), "OctreeNode")) &&
                     (strstr(tmp -> GetInfo(), "LoadTexture") != NULL) &&
                     (strstr(tmp -> GetInfo(), "begin") != NULL)) {

              // Set flag
              loadTextureBegin = true;

              // Get start time
              loadTextureBeginTime = tmp -> GetTime();

            }

            // End of load texture
            else if ((!strcmp(tmp -> GetClassName(), "OctreeNode")) &&
                     (strstr(tmp -> GetInfo(), "LoadTexture") != NULL) &&
                     (strstr(tmp -> GetInfo(), "end") != NULL)) {

              // Set flag
              loadTextureEnd = true;

              // Get end time
              loadTextureEndTime = tmp -> GetTime();

              // Check to make sure there's a matching begin and end
              if (loadTextureBegin == true && loadTextureEnd == true) {

                // Save time to load data
                _loadTextureTimePerLOD[(procNum * _numberOfLOD) + level].
                  push_back(loadTextureEndTime - loadTextureBeginTime);

              }

              // Reset flags
              loadTextureBegin = false;
              loadTextureEnd = false;

            }

          }

          // Operation is outside a level begin and a level end
          else {

            // Go to top of loop
            continue;

          }

          // Found both a begin and an end
          if (levelBegin == true && levelEnd == true) {

            // Calcualte time
            totalTime = endTime - startTime;

            // Save time
            _renderTimePerLOD[(procNum * _numberOfLOD) + level].
              push_back(totalTime);

            // Save cacheRAM hits
            _cacheRAMHits[(procNum * _numberOfLOD) + level].
              push_back(cacheRAMHits);

            // Save cacheRAM misses
            _cacheRAMMisses[(procNum * _numberOfLOD) + level].
              push_back(cacheRAMMisses);

            // Save cacheVRAM hits
            _cacheVRAMHits[(procNum * _numberOfLOD) + level].
              push_back(cacheVRAMHits);

            // Save cacheVRAM misses
            _cacheVRAMMisses[(procNum * _numberOfLOD) + level].
              push_back(cacheVRAMMisses);

            // Reset cacheRAM stats
            cacheRAMHits = 0;
            cacheRAMMisses = 0;

            // Reset cacheVRAM stats
            cacheVRAMHits = 0;
            cacheVRAMMisses = 0;

            // Reset flags
            levelBegin = false;
            levelEnd = false;

            // Increment level
            level++;

          }

        }


      }

      // Clear flags
      renderBeginFound[procNum] = false;
      renderEndFound[procNum] = false;
      
      // Clean up operations      
      for (int i = 0 ; i < _operationVector[procNum].size() ; i++) {
        Operation* tmp = _operationVector[procNum].at(i);
        if (tmp != NULL) {
          delete tmp;
        }
      }
      _operationVector[procNum].clear();
      
    }    
  

  }


  // Clean up render begin flag
  if (renderBeginFound != NULL) {
    delete [] renderBeginFound;
  }

  // Clean up render end flag
  if (renderEndFound != NULL) {
    delete [] renderEndFound;
  }


  // Nice message
  fprintf(stdout, "Done\n");
  fflush(stdout);


  // Return OK
  return OK;

}

/*--------------------------------------------------------------------------*/

int main(int argc, char** argv) {

  // Nice message
  fprintf(stdout, "Benchmark Analyzer for Volume Rendering Application\n");
  fflush(stdout);

  // Check command line arguments
  if (argc != 2) {
    fprintf(stderr, "Usage: Analyzer inputFile\n");
    exit(1);
  }

  // Open input file
  open_file(argv[1]);

  // Determine number of processes
  num_processes();

  // Determine number of levels-of-detail
  num_lod();

  // Determine size of CacheRAM
  cacheRAM_size();

  // Determine size of CacheVRAM
  cacheVRAM_size();

  // Determine brick size
  brick_size();

  // Allocate storage
  allocate_storage();

  // Process data
  process();

  // Analyze data and print out results
  analyze_each_process();

  // Analyze data for each level
  analyze_each_level();

  // Analyze all processes
  analyze_all();

  // Deallocate storage
  deallocate_storage();

  // Return
  return 0;

}

/*--------------------------------------------------------------------------*/
