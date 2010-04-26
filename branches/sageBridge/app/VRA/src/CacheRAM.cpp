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

#include "CacheRAM.h"

/*--------------------------------------------------------------------------*/

CacheRAM::CacheRAM(int size) {

  // Message
  Assert("Constructor begin.");

  // Initialize size
  _currentSize = 0;

  // Initialize maximum size
  _maximumSize = size;

  // Initialize front
  _front = NULL;

  // Message
  Assert("Constructor end.");

}
  
/*--------------------------------------------------------------------------*/

CacheRAM::~CacheRAM() {

  // Message
  Assert("Destructor begin.");

  // Current and next node pointers used to iterate through cache
  CacheRAMNode* current = NULL;
  CacheRAMNode* next = NULL;

  // Iterate through cache deleting each node
  while (current != NULL) {
    next = current -> GetNext();
    delete current;
    current = next;
  }

  // Message
  Assert("Destructor end.");

}

/*--------------------------------------------------------------------------*/

void CacheRAM::Assert(const char* message) {

#ifdef LOG

  // Print message to log
  char buffer[1024];
  sprintf(buffer, "CacheRAM: %s", message);
  Log::Assert(buffer);

#endif

}

/*--------------------------------------------------------------------------*/

bool CacheRAM::IsFull() {

  // Check if cache is full
  if (_currentSize >= _maximumSize) {

    // Message
    Assert("Is cache full? True.");

    // Return true
    return true;
  }

  // Message
  Assert("Is cache full? False.");

  // Return false
  return false;

}

/*--------------------------------------------------------------------------*/

bool CacheRAM::IsLoaded(OctreeNode* node) {

  // Message
  char message[1024];
  sprintf(message, "Is node %d loaded? Checking...", node -> GetID());
  Assert(message);

  // Node pointers used for iteration
  CacheRAMNode* tmp = _front;
 
  // Iterate through cache until node is found based on its id
  while (tmp != NULL) {

    // Check if node is found
    if (tmp -> GetOctreeNode() -> GetID() == node -> GetID()) {

      // Message
      sprintf(message, "Is node %d loaded? True.", node -> GetID());
      Assert(message);

      // Return true
      return true;
    }

    // Next node
    tmp = tmp -> GetNext();

  }

  // Message
  sprintf(message, "Is node %d loaded? False.", node -> GetID());
  Assert(message);

  // Return false
  return false;

}

/*--------------------------------------------------------------------------*/

void CacheRAM::Load(OctreeNode* node) {

  // Message
  char message[1024];
  sprintf(message, "Load node %d begin.", node -> GetID());
  Assert(message);

  // Allocate new node
  CacheRAMNode* tmp = new CacheRAMNode;
  if (tmp == NULL) {
    fprintf(stderr, "CacheRAM: Memory allocation error.\n");
    return;
  }

  // If cache is full, remove LRU node at end of cache
  if (IsFull() == true) {
    RemoveLast();
  }

  // Load octree node's data
  node -> LoadData();

  // Set cache node's parameters
  tmp -> SetOctreeNode(node);
  tmp -> SetNext(_front);
  tmp -> SetPrevious(NULL);

  // Place in front
  if (_front != NULL) {
    _front -> SetPrevious(tmp);
  }

  // Set front
  _front = tmp;

  // Increment size
  _currentSize++;

  // Message
  sprintf(message, "Load node %d end.", node -> GetID());
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void CacheRAM::MakeCurrent(OctreeNode* node) {

  // Message
  char message[1024];
  sprintf(message, "Make node %d current begin.", node -> GetID());
  Assert(message);

  // If octree node data is loaded, move it to the front
  if (IsLoaded(node) == true) {

    // Message
    sprintf(message, 
            "Make node %d current. Move node to front.", 
            node -> GetID());
    Assert(message);

    // Move node to front
    MoveToFront(node);
  }

  // Load node, automatically placing it at front
  else {

    // Message
    sprintf(message, 
            "Make node %d current. Load node.", 
            node -> GetID());
    Assert(message);

    // Load node
    Load(node);
  }

  // Message
  sprintf(message, "Make node %d current end.", node -> GetID());
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void CacheRAM::MoveToFront(OctreeNode* node) {

  // Message
  char message[1024];
  sprintf(message, "Move node %d to front begin.", node -> GetID());
  Assert(message);

  // Node pointers used for iteration
  CacheRAMNode* tmp = _front;

  // Iterate through cache until node is found based on its id
  while (tmp != NULL) {
    if (tmp -> GetOctreeNode() -> GetID() == node -> GetID()) {
      break;
    }
    tmp = tmp -> GetNext();
  }

  // Node is not found
  if (tmp == NULL) {
    return;
  }

  // Node is already at the front
  if (tmp -> GetPrevious() == NULL) {
    return;
  }

  // Connect the node's previous and next node
  (tmp -> GetPrevious()) -> SetNext(tmp -> GetNext());
  if (tmp -> GetNext() != NULL) {
    (tmp -> GetNext()) -> SetPrevious(tmp -> GetPrevious());
  }

  // Place at front
  tmp -> SetPrevious(NULL);
  if (_front != NULL) {
    _front -> SetPrevious(tmp);
  }
  tmp -> SetNext(_front);

  // Set front
  _front = tmp;

  // Message
  sprintf(message, "Move node %d to front end.", node -> GetID());
  Assert(message);

}

/*--------------------------------------------------------------------------*/

void CacheRAM::Print() {

  // Message
  Assert("Printing CacheRAM begin.");

  // Message
  char message[1024];
  sprintf(message, "  Current Size: %d", _currentSize);
  Assert(message);
  sprintf(message, "  Maximum Size: %d", _maximumSize);
  Assert(message);

  // Front of queue
  CacheRAMNode* current = _front;

  // Traverse queue
  while (current != NULL) {

    // Print node id
    sprintf(message, 
            "  Node ID: %d ", 
            current -> GetOctreeNode() -> GetID());
    Assert(message);

    // Print level id
    sprintf(message, 
            "    Node Level: %d", 
            current -> GetOctreeNode() -> GetLevel());
    Assert(message);

    // Print data loaded flag
    if (current -> GetOctreeNode() -> IsDataLoaded() == true) {
      sprintf(message, 
              "    Data Loaded: true");
    }
    else {
      sprintf(message, 
              "    Data Loaded: false");
    }
    Assert(message);

    // Print texture loaded flag
    if (current -> GetOctreeNode() -> IsTextureLoaded() == true) {
      sprintf(message, 
              "    Texture Loaded: true");
    }
    else {
      sprintf(message, 
              "    Texture Loaded: false");
    }
    Assert(message);

    // Next node
    current = current -> GetNext();

  }

  // Message
  Assert("Printing CacheRAM end.");

}

/*--------------------------------------------------------------------------*/

void CacheRAM::RemoveLast() {

  // Message
  Assert("Removing last node begin.");

  // Current and next node pointers used to iterate through cache
  CacheRAMNode* previous = NULL;
  CacheRAMNode* current = _front;

  // Empty cache
  if (current == NULL) {
    return;
  }

  // Decrement size
  _currentSize--;

  // Iterate until the end of the cache is reached
  while (current != NULL) {
    previous = current;
    current = current -> GetNext();
  }

  // Get the last node
  current = previous;
  previous = current -> GetPrevious();

  // Remove last node rom cache
  if (previous == NULL) {
    _front = NULL;
  }
  else {
    previous -> SetNext(NULL);
  }

  // Unload octree nodes' data
  current -> GetOctreeNode() -> UnloadData();

  // Clean up node
  delete current;

  // Message
  Assert("Removing last node end.");

}
  
/*--------------------------------------------------------------------------*/
