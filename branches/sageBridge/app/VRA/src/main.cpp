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

#include <stdlib.h>
#include <unistd.h>

#include "Communicator.h"
#include "Log.h"
#include "Master.h"
#include "Slave.h"
#include "SynchronizerServer.h"

/*--------------------------------------------------------------------------*/

Communicator* communicator;

/*--------------------------------------------------------------------------*/

int main(int argc, char** argv) {

  // Communicator
  communicator = new Communicator;

  // Initialize communication between nodes
  communicator -> Init(argc, argv);

  // Start logging
  Log::Start();

  // Master process
  if (communicator -> GetRank() == 0) {

    // Sync all nodes
    communicator -> Barrier();

    // Create master
    Master* master = new Master(communicator);

    // Sync all nodes
    communicator -> Barrier();

    // Initialize master
    master -> Init("config");

    // Run master
    master -> Run();

    // Clean up master
    delete master;

  }

  // Slave processes
  else {
    
    // Sync all nodes
    communicator -> Barrier();

    // Create slave
    Slave* slave = new Slave(communicator);

    // Sync all nodes
    communicator -> Barrier();

    // Run slave
    slave -> Run();

    // Clean up slave
    delete slave;

  }

  // Stop logging
  Log::Stop();

  // Wait for all communication to end
  communicator -> Destroy();
 
  // Clean up communicator
  delete communicator;

  // Exit
  return 0;

}

/*--------------------------------------------------------------------------*/
