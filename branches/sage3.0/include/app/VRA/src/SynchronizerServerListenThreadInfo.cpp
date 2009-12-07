/*---------------------------------------------------------------------------*/
/* Volume Rendering Application                                              */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                  */
/*                                                                           */
/* This software is free software; you can redistribute it and/or modify it  */
/* under the terms of the GNU Lesser General Public License as published by  */
/* the Free Software Foundation; either Version 2.1 of the License, or       */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This software is distributed in the hope that it will be useful, but      */
/* WITHOUT ANY WARRANTY; without even the implied warranty of                */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser   */
/* General Public License for more details.                                  */
/*                                                                           */
/* You should have received a copy of the GNU Lesser Public License along    */
/* with this library; if not, write to the Free Software Foundation, Inc.,   */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                     */
/*---------------------------------------------------------------------------*/

#include "SynchronizerServerListenThreadInfo.h"

/*---------------------------------------------------------------------------*/

SynchronizerServerListenThreadInfo::SynchronizerServerListenThreadInfo() {

  // Initialize to NULL
  _clientInfo = NULL;

  // Initialize to NULL
  _clientThread = NULL;

  // Initialize to NULL
  _conditionMutex = NULL;

  // Initialize to NULL
  _conditionVariable = NULL;

  // Initialize to NULL
  _finalizeFlag = NULL;

  // Initialize to NULL
  _level = NULL;

  // Initialize to NULL
  _numberOfClients = NULL;

  // Initialize to NULL
  _numberOfClientsWaiting = NULL;

  // Initialize to NULL
  _port = NULL;

  // Initialize to NULL
  _progressObserver = NULL;

  // Initialize to NULL
  _serverAddress = NULL;

  // Initialize to NULL
  _serverSocketFileDescriptor = NULL;

}

/*---------------------------------------------------------------------------*/

SynchronizerServerListenThreadInfo::~SynchronizerServerListenThreadInfo() {
}

/*---------------------------------------------------------------------------*/
