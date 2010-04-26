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

#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <pthread.h>
#include <queue.h>
#include <unistd.h>

#include "DataPacker.h"
#include "Fl_Connect.h"
#include "Fl_Dialog.h"
#include "Fl_Lambda.h"
#include "Fl_Load_Data.h"
#include "Fl_Main_Window.h"
#include "Fl_Overview.h"
#include "Fl_Slice_Frequency.h"
#include "Fl_Transfer_Function.h"
#include "Trackball.h"
#include "UserInterfaceClient.h"
#include "UserInterfaceClientConcreteCommand.h"
#include "Vertex.h"

/*--------------------------------------------------------------------------*/

static void message_cb(Fl_Button* b, void* v);
static void stopLambda_cb(Fl_Button* b, void* v);
static void message_data_cb(Fl_Button* b, void* v);
static void connect_cb(Fl_Button* b, void* v);
static void exit_cb(Fl_Button* b, void* v);
static void render_cb();
void *lambda_thread_function(void* ptr);
static void mes_script_cb(Fl_Button* b, void* v);
static void overview_cb(Fl_Widget *w, void *data);
static void map_cb(void* d);
static void menu_axis_cb();
static void menu_bounding_cb();
static void menu_brick_cb();
static void slice_frequency();
static void slice_ok_cb(Fl_Button* b, void* v);
static void slice_cancel_cb(Fl_Button* b, void* v);
void run_script(const char* filename);

/*--------------------------------------------------------------------------*/

float goodx = 0.0;
float goody = 0.0;

// Menu items
static Fl_Menu_Item g_miAxis = 
  {"Axis", 0, (Fl_Callback*) menu_axis_cb, 0, FL_MENU_TOGGLE};
static Fl_Menu_Item g_miBoundingBox = 
  {"Bounding Box", 0, (Fl_Callback*) menu_bounding_cb, 0, FL_MENU_TOGGLE};
static Fl_Menu_Item g_miBrickBoxes = 
  {"Brick Boxes", 0, (Fl_Callback*) menu_brick_cb, 0, FL_MENU_TOGGLE};

// User interface client
static UserInterfaceClient* client = NULL;

// Dataset list size
static int datasetListSize = 0;

// Dataset list file
static char** datasetListFile = NULL;

// Dataset list name
static char** datasetListName = NULL;

// Dataset list prefix
static char** datasetListPrefix = NULL;

// User interface client observer
static UserInterfaceClientConcreteCommand* observer = NULL;

// Message queue
static queue<unsigned char*> messageQueue;

// Message queue mutex
static pthread_mutex_t messageMutex = PTHREAD_MUTEX_INITIALIZER;

// Connect window
static Fl_Connect* connectWindow = NULL;

// Lambda window
static Fl_Lambda* lambdaWindow = NULL;

// Load data window
static Fl_Load_Data* loadDataWindow = NULL;

// Status bar
static Fl_Box* statusBar = NULL;

// Data bar
static Fl_Box* dataBar = NULL;

// Progress bar
static Fl_Box* progressBar = NULL;

// Dataset type
static int datasetType = 8;

// Main window
static Fl_Main_Window* mainWindow = NULL;

// Message window
static Fl_Dialog* messageWindow = NULL;

// Stop LambdaTracker window
static Fl_Dialog* stopLambdaWindow = NULL;

// Overview widget
static Fl_Overview* overview = NULL;

// Transfer function widget
static Fl_Transfer_Function* widget = NULL;

// Slice frequency widget
static Fl_Slice_Frequency* slice = NULL;

// Slice frequency
static double sliceFrequency = 1.0;

// Progress level for script thread
static int progressLevel = -1;
static int totalLevel = -1;

// Thread listening for server updates
pthread_t _thread;

// Thread listening for LabmdaTracker updates
pthread_t _lambdaThread;

// Structure for script thread
typedef struct {
  UserInterfaceClient* client;
  const char* scriptfile;
  bool* cancelScript;
  void (*cb)(unsigned char* message);
} scriptData;

// Structure for LambdaTracker thread
typedef struct {
  int port;
  bool stopFlag;
} lambdaData;

// Data for lambda thread
lambdaData info;

// Script info mutex
static pthread_mutex_t scriptMutex = PTHREAD_MUTEX_INITIALIZER;

// User cancels script
bool _cancelScript = false;

// LambdaTable transforms
float world_rotate[3] = {0.0, 0.0, 0.0};
float world_scale[3] = {0.0, 0.0, 0.0};
float world_scale_angle = 0.0;
float world_translate[3] = {0.0, 0.0, 0.0};

// LambdaTable state
bool rotateUp = true;
bool scaleUp = true;
bool translateUp = true;

// LambdaTable position
float rotatePuckPos[2] = {0.0, 0.0};
float translatePuckPos[2] = {0.0, 0.0};
float scalePuckPos[2] = {0.0, 0.0};

// Total display frustum
float _totalDisplayFrustum[6] = {-1.0, 1.0, -1.0, 1.0, -1.0, 1.0};

float scaleDeltaX = 0.0;
float scaleDeltaY = 0.0;
float oriX = 0.0;
float oriY = 0.0;
float wtnX = 0.0;
float scale_pos_f[3] = {0.0, 0.0, 0.0};
float scalePuckPosI[2] = {0.0, 0.0};
float scaleAngleI = 0.0;

// Identity matrices
float RI[16] = {1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0};
float SI[16] = {1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0};
float TI[16] = {1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0};

// Trackball interaction for LambdaTracker
Trackball lambdaTrackball;

/*--------------------------------------------------------------------------*/

static void reset_transforms() {

  // Reset translation matrix
  overview -> SetTranslationMatrix(TI);
  client -> SendTranslationMatrix(TI);
  
  // Reset scale matrix
  overview -> SetScaleMatrix(SI);
  client -> SendScaleMatrix(SI);

  // Reset rotation matrix
  overview -> SetRotationMatrix(RI);
  client -> SendRotationMatrix(RI);

  // Reset overview
  overview -> ResetView();

  // Reset master trackball for SAGE events
  client -> ResetView();

  // Reset trackball
  lambdaTrackball.Clear();

  // Force a sleep for one second
  usleep(1000000);

  // Send render command
  client -> SendCommandRender();

}

/*--------------------------------------------------------------------------*/

static void reset_view(Fl_Button* b, void* v) {

  // Reset view transforms
  reset_transforms();

}

/*--------------------------------------------------------------------------*/

static void load_tf_file(char* name) {

  // File pointer
  FILE* fptr = NULL;
  
  // Attempt to open file
  if ((fptr = fopen(name, "r")) == NULL) {
    fl_message("Could not open file %s", name);
    return;
  }

  // Operation
  char operation[128];
  memset(operation, 0, 128);

  // Maximum scalar value
  float max = 0.0;

  // Entry
  float* s = NULL;
  float* r = NULL;
  float* g = NULL;
  float* b = NULL;
  float* o = NULL;

  // Number of entries
  int num = 0;

  // Read maximum scalar value
  fscanf(fptr, "%f", &max);

  // Read operation
  fscanf(fptr, "%s %d", operation, &num);

  // Check for color tag
  if (strcmp(operation, "COLOR")) {
    fl_message("Invalid format.");
    fclose(fptr);
    return;
  }

  // Allocate space for points
  s = new float[num];
  r = new float[num];
  g = new float[num];
  b = new float[num];

  // Read entries
  for (int i = 0 ; i < num ; i++) {
    fscanf(fptr, "%f %f %f %f", &s[i], &r[i], &g[i], &b[i]);
  }

  // Restore color points
  widget -> RestoreColorControlPoints(num, max, s, r, g, b);

  // Deallocate storage
  delete [] s;
  delete [] r;
  delete [] g;
  delete [] b;

  // Read operation
  fscanf(fptr, "%s %d", operation, &num);

  // Check for color tag
  if (strcmp(operation, "OPACITY")) {
    fl_message("Invalid format.");
    fclose(fptr);
    return;
  }

  // Allocate space for points
  s = new float[num];
  o = new float[num];

  // Read entries
  for (int i = 0 ; i < num ; i++) {
    fscanf(fptr, "%f %f", &s[i], &o[i]);
  }

  // Restore color points
  widget -> RestoreOpacityControlPoints(num, max, s, o);

  // Deallocate storage
  delete [] s;
  delete [] o;

  // Close file
  fclose(fptr);

  // Set maximum scalar value
  if (datasetType == 8) {
    widget -> SetMaximumScalar(255.0);
  }
  else if (datasetType == 16) {
    widget -> SetMaximumScalar(65536.0);
  }

  // Redraw widget
  widget -> Redraw();

  // Load map
  map_cb(widget);

}

/*--------------------------------------------------------------------------*/

static void load_tf() {

  // Show file chooser
  char* filename = fl_file_chooser("Load Transfer Function", "*.tf", NULL);

  // Check if a file was selected
  if (filename != NULL) {
    load_tf_file(filename);
  }

}

/*--------------------------------------------------------------------------*/

static void save_tf_file(char* name) {

  // File pointer
  FILE* fptr = NULL;
  
  // Attempt to open file
  if ((fptr = fopen(name, "w")) == NULL) {
    fl_message("Could not open file %s", name);
    return;
  }

  
  // Maximum value
  float min = 0.0;
  float max = 0.0;
  widget -> GetScalarRange(&min, &max);
  fprintf(fptr, "%f\n", max);


  // Operation
  fprintf(fptr, "COLOR ");

  // Get number of color control points
  int num = widget -> GetNumberOfColorControlPoints();
  fprintf(fptr, "%d\n", num);

  // Entry
  float s = 0.0;
  float r = 0.0;
  float g = 0.0;
  float b = 0.0;
  float o = 0.0;

  // Get color control points
  for (int i = 0 ; i < num ; i++) {
    widget -> GetColorControlPoint(i, &s, &r, &g, &b);
    fprintf(fptr, "%f %f %f %f\n", s, r, g, b);
  }


  // Operation
  fprintf(fptr, "OPACITY ");

  // Get number of color control points
  num = widget -> GetNumberOfOpacityControlPoints();
  fprintf(fptr, "%d\n", num);

  // Get color control points
  for (int i = 0 ; i < num ; i++) {
    widget -> GetOpacityControlPoint(i, &s, &o);
    fprintf(fptr, "%f %f\n", s, o);
  }


  // Redraw widget
  widget -> Redraw();

  // Load map
  map_cb(widget);


  // Close
  fclose(fptr);

}

/*--------------------------------------------------------------------------*/

static void save_tf() {

  // Show file chooser
  char* filename = fl_file_chooser("Save Transfer Function", NULL, NULL);

  // Check if a file was selected
  if (filename != NULL) {
    save_tf_file(filename);
  }

}

/*--------------------------------------------------------------------------*/

static void slice_frequency() {

  // Calculate position of message window
  int posx = (mainWindow -> x()) +
    (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
    (225 / 2);
  int posy = (mainWindow -> y()) +
    (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
    (80 / 2);
  
  // Create new connect window
  slice = new Fl_Slice_Frequency(posx, posy, 225, 80, "Slice Frequency");

  // Set slice frequency value
  slice -> SetSliceFrequency(sliceFrequency);

  // Set modal
  slice -> set_modal();

  // Show connect window
  slice -> show();

  // Set callback
  slice -> callback((Fl_Callback*) slice_cancel_cb);

  // Set connect button callback
  slice -> SetOKButtonCallback((Fl_Callback*) slice_ok_cb, NULL);

  // Set exit button callback
  slice -> SetCancelButtonCallback((Fl_Callback*) slice_cancel_cb, NULL);

}

/*--------------------------------------------------------------------------*/

static void slice_ok_cb(Fl_Button* b, void* v) {

  // Hide slice window
  slice -> hide();

  // Get slice frequency
  double frequency = slice -> GetSliceFrequency();
  sliceFrequency = frequency;

  // Set slice frequency
  client -> SendSliceFrequency(frequency);

  // Delete slice window
  delete slice;
  slice = NULL;

}

/*--------------------------------------------------------------------------*/

static void slice_cancel_cb(Fl_Button* b, void* v) {

  // Hide slice window
  slice -> hide();

  // Delete slice window
  delete slice;
  slice = NULL;

}

/*--------------------------------------------------------------------------*/

static void menu_axis_cb() {

  // Check for client
  if (client != NULL) {

    // Turn off axis
    if (g_miAxis.value()) {
      client -> SendAxisOff();
      g_miAxis.clear();
    }

    // Turn on axis
    else {
      client -> SendAxisOn();
      g_miAxis.set();
    }

  }

}

/*--------------------------------------------------------------------------*/

static void menu_bounding_cb() {

  // Check for client
  if (client != NULL) {

    // Turn off bounding box
    if (g_miBoundingBox.value()) {
      client -> SendBoundingBoxOff();
      g_miBoundingBox.clear();
    }

    // Turn on bounding box
    else {
      client -> SendBoundingBoxOn();
      g_miBoundingBox.set();
    }

  }

}

/*--------------------------------------------------------------------------*/

static void menu_brick_cb() {

  // Check for client
  if (client != NULL) {

    // Turn off brick box
    if (g_miBrickBoxes.value()) {
      client -> SendBrickBoxOff();
      g_miBrickBoxes.clear();
    }

    // Turn on brick box
    else {
      client -> SendBrickBoxOn();
      g_miBrickBoxes.set();
    }

  }

}

/*--------------------------------------------------------------------------*/

static void add_message(unsigned char* message) {

  // Lock message mutex
  pthread_mutex_lock(&messageMutex);

  // Push message to back of queue
  messageQueue.push(message);

  // Unlock message mutex
  pthread_mutex_unlock(&messageMutex);

  // Interrupt Fl::wait in the main thread
  //Fl::awake(NULL);

}

/*--------------------------------------------------------------------------*/

static void map_cb(void* d) {

  // 8-bit map
  if (datasetType == 8) {

    // Color and opacity map
    unsigned char map[256*4];
  
    // Transfer function values
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float o = 0.0;

    // Build map
    for (int i = 0 ; i < 256 ; i++) {
      
      // Get color values from widget
      ((Fl_Transfer_Function*) d) -> GetColor((float) i, &r, &g, &b);
      
      // Get opacity value from widget
      o = ((Fl_Transfer_Function*) d) -> GetOpacity((float) i);
      
      // Convert from 0.0 - 1.0 to 0 - 255
      map[(i*4)] = (unsigned char) (r * 255.0);
      map[(i*4) + 1] = (unsigned char) (g * 255.0);
      map[(i*4) + 2] = (unsigned char) (b * 255.0);
      map[(i*4) + 3] = (unsigned char) (o * 255.0);
      
    }
    
    // Send map
    if (client != NULL) {
      client -> SendMap(map);
      client -> SendCommandRender();
      client -> SendCommandRender();
    }

  }

  // 16-bit map
  else if (datasetType == 16) {

    // Color and opacity map
    unsigned char map[65536*4];
  
    // Transfer function values
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float o = 0.0;

    // Build map
    for (int i = 0 ; i < 65536 ; i++) {
      
      // Get color values from widget
      ((Fl_Transfer_Function*) d) -> GetColor((float) i, &r, &g, &b);
      
      // Get opacity value from widget
      o = ((Fl_Transfer_Function*) d) -> GetOpacity((float) i);
      
      // Convert from 0.0 - 1.0 to 0 - 255
      map[(i*4)] = (unsigned char) (r * 255.0);
      map[(i*4) + 1] = (unsigned char) (g * 255.0);
      map[(i*4) + 2] = (unsigned char) (b * 255.0);
      map[(i*4) + 3] = (unsigned char) (o * 255.0);
     
    }
    
    // Send map
    if (client != NULL) {
      client -> SendMap16(map);
      client -> SendCommandRender();
      client -> SendCommandRender();
    }

  }

}

/*--------------------------------------------------------------------------*/

static void connect_show() {

  // Calculate position of message window
  int posx = (mainWindow -> x()) +
    (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
    (255 / 2);
  int posy = (mainWindow -> y()) +
    (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
    (115 / 2);
  
  // Create new connect window
  connectWindow = new Fl_Connect(posx, posy, 255, 115, "Connect");

  // Set modal
  connectWindow -> set_modal();

  // Show connect window
  connectWindow -> show();

  // Set callback
  connectWindow -> callback(overview_cb);

  // Set connect button callback
  connectWindow -> SetConnectButtonCallback((Fl_Callback*) connect_cb, NULL);

  // Set exit button callback
  connectWindow -> SetExitButtonCallback((Fl_Callback*) exit_cb, NULL);

}

/*--------------------------------------------------------------------------*/

static void listenToLambdaTracker(int port) {

  // Setup data
  info.port = port;
  info.stopFlag = false;

  // Create thread listening for LambdaTracker updates
  pthread_create(&_lambdaThread,
		 NULL,
		 lambda_thread_function,
		 &info);

  // Calculate position of message window
  int posx = (mainWindow -> x()) +
      (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
      (195 / 2);
  int posy = (mainWindow -> y()) +
      (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
      (75 / 2);

  // Create new message window
  stopLambdaWindow = new Fl_Dialog(posx, posy, 195, 75, "LambdaTracker");

  // Set ok button callback
  stopLambdaWindow -> 
      SetOKButtonCallback((Fl_Callback*) stopLambda_cb, (void*) &info);
    
  // Set callback
  //stopLambdaWindow -> callback(overview_cb);

  // Set message
  stopLambdaWindow -> SetMessage("Stop listening?");

  // Set modal
  //messageWindow -> set_modal();

  // Show dialog
  stopLambdaWindow -> show();

  // Update scale for LambdaTable
  float Sm[16];
  overview -> GetScaleMatrix(Sm);
  world_scale[0] = Sm[0];
  world_scale[1] = Sm[5];
  world_scale[2] = Sm[10];

  // Update translation for LambdaTable
  float Tm[16];
  overview -> GetTranslationMatrix(Tm);
  world_translate[0] = Tm[12];
  world_translate[1] = Tm[13];

  /*  
  // Update connection
  char hostport[1024];
  sprintf(hostport, "Connected to %s:%d", host, port);
  statusBar -> copy_label(hostport);

  // Deactivate connect menu item
  mainWindow -> DeactivateConnect();

  // Activate disconnect menu item
  mainWindow -> ActivateDisconnect();

  // Activate shutdown menu item
  mainWindow -> ActivateShutdown();

  // Get data
  client -> GetData();

  // Get number of datasets
  client -> GetDatasetListSize();

  // Get total display dimenstions
  client -> GetTotalDisplayDimensions();

  // Get total display frustum
  client -> GetTotalDisplayFrustum();

  // Get overview vertices
  client -> GetOverviewVertices();

  // Get dataset type
  client -> GetDatasetType();

  // Get rotation matrix
  client -> GetRotationMatrix();

  // Get scale matrix
  client -> GetScaleMatrix();

  // Get translation matrix
  client -> GetTranslationMatrix();

  // Get axis state
  client -> GetAxis();

  // Get bounding box state
  client -> GetBoundingBox();

  // Get brick box state
  client -> GetBrickBox();

  // Get slice frequency
  client -> GetSliceFrequency();

  // Redraw tf widget
  widget -> Redraw();

  // Load map
  map_cb(widget);
    */

}

/*--------------------------------------------------------------------------*/

static void connect(char* host, int port) {

  // Connect to server
  if ((client -> Init(host, port)) != USER_INTERFACE_CLIENT_OK) {

    // Calculate position of message window
    int posx = (mainWindow -> x()) +
      (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
      (195 / 2);
    int posy = (mainWindow -> y()) +
      (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
      (75 / 2);

    // Create new message window
    messageWindow = new Fl_Dialog(posx, posy, 195, 75, "Error");

    // Set ok button callback
    messageWindow -> SetOKButtonCallback((Fl_Callback*) message_cb, NULL);
    
    // Set callback
    messageWindow -> callback(overview_cb);

    // Set message
    messageWindow -> SetMessage("Can not connect to server.");

    // Set modal
    messageWindow -> set_modal();

    // Show dialog
    messageWindow -> show();

    // Return
    return;

  }
  
  // Update connection
  char hostport[1024];
  sprintf(hostport, "Connected to %s:%d", host, port);
  statusBar -> copy_label(hostport);

  // Deactivate connect menu item
  mainWindow -> DeactivateConnect();

  // Activate disconnect menu item
  mainWindow -> ActivateDisconnect();

  // Activate shutdown menu item
  mainWindow -> ActivateShutdown();

  // Get data
  client -> GetData();

  // Get number of datasets
  client -> GetDatasetListSize();

  // Get total display dimenstions
  client -> GetTotalDisplayDimensions();

  // Get total display frustum
  client -> GetTotalDisplayFrustum();

  // Get overview vertices
  client -> GetOverviewVertices();

  // Get dataset type
  client -> GetDatasetType();

  // Get rotation matrix
  client -> GetRotationMatrix();

  // Get scale matrix
  client -> GetScaleMatrix();

  // Get translation matrix
  client -> GetTranslationMatrix();

  // Get axis state
  client -> GetAxis();

  // Get bounding box state
  client -> GetBoundingBox();

  // Get brick box state
  client -> GetBrickBox();

  // Get slice frequency
  client -> GetSliceFrequency();

  // Redraw tf widget
  widget -> Redraw();

  // Load map
  map_cb(widget);

}

/*--------------------------------------------------------------------------*/

static void disconnect() {

  // Disconnect from server
  client -> Disconnect();

  // Update connection
  statusBar -> copy_label("Not connected.");

  // Update data bar
  dataBar -> copy_label("No data.");

  // Update progress bar
  progressBar -> copy_label("Level");

  // Activate connect menu item
  mainWindow -> ActivateConnect();

  // Deactivate disconnect menu item
  mainWindow -> DeactivateDisconnect();

  // Deactivate shutdown menu item
  mainWindow -> DeactivateShutdown();

  // Show connect window
  connect_show();

}

/*--------------------------------------------------------------------------*/

static void data_ok_cb(Fl_Button* b, void* v) {

  // Hide message window
  loadDataWindow -> hide();

  // Get selected data set
  int selected = loadDataWindow -> GetSelected();

  // Delete load data window
  delete loadDataWindow;
  loadDataWindow = NULL;

  // Load data
  if (selected != 0) {

    // Send data prefix
    client -> SendPrefix(datasetListPrefix[selected - 1]);

    // Send data file name
    client -> SendData(datasetListFile[selected - 1]);

    // Get overview vertices
    client -> GetOverviewVertices();

    // Get dataset type
    client -> GetDatasetType();

    // Get rotation matrix
    //client -> GetRotationMatrix();

    // Get scale matrix
    //client -> GetScaleMatrix();

    // Get translation matrix
    //client -> GetTranslationMatrix();

    // Redraw tf widget
    widget -> Redraw();

    // Send transfer function
    map_cb(widget);

    // Update data bar
    dataBar -> copy_label(datasetListFile[selected - 1]);

    // Reset view transforms
    reset_transforms();

  }

  // If user didn't select anything
  else {

    // Calculate position of message window
    int posx = (mainWindow -> x()) +
      (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
      (195 / 2);
    int posy = (mainWindow -> y()) +
      (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
      (75 / 2);

    // Create new message window
    messageWindow = new Fl_Dialog(posx, posy, 195, 75, "Error");

    // Set ok button callback
    messageWindow -> SetOKButtonCallback((Fl_Callback*) message_data_cb, NULL);

    // Set message
    messageWindow -> SetMessage("Select a dataset to load.");

    // Set modal
    messageWindow -> set_modal();

    // Show dialog
    messageWindow -> show();

    // Return
    return;

  }

}

/*--------------------------------------------------------------------------*/

static void data_cancel_cb(Fl_Button* b, void* v) {

  // Hide message window
  loadDataWindow -> hide();

  // Delete load data window
  delete loadDataWindow;
  loadDataWindow = NULL;

}

/*--------------------------------------------------------------------------*/

void *lambda_thread_function(void* ptr) {

  // Cast data
  lambdaData* info = (lambdaData*) ptr;

  // Socket info
  struct sockaddr_in serverAddress;
  int serverSocketFileDescriptor;

  // Create server socket
  if ((serverSocketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("ListenToLambdaTracker: Error creating socket");

    // Exit thread
    pthread_exit(NULL);

  }

  // Set SO_REUSEADDR
  int optVal = 1;
  int optLen = sizeof(optVal);
  if(setsockopt(serverSocketFileDescriptor, 
                SOL_SOCKET, 
                SO_REUSEADDR, 
                (void*) &optVal, 
                (socklen_t) optLen) != 0) {
    perror("ListenToLambdaTrakcer: Error setting SO_REUSEADDR");
  }

  // Initialize server address
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(info -> port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  
  // Bind address
  if (bind(serverSocketFileDescriptor, 
           (struct sockaddr*) &serverAddress, 
           sizeof(serverAddress)) != 0) {
    perror("ListenToLambdaTracker: Error binding address");
    close(serverSocketFileDescriptor);
  }
  
  // Listen for messages from LambdaTracker
  while(info -> stopFlag == false) {

    // Message variables
    char msg[60];
    int ret;
    socklen_t sin_length;

    // Get message
    sin_length = sizeof(struct sockaddr_in);
    ret = recvfrom(serverSocketFileDescriptor, 
		   msg, 60, 0, (struct sockaddr*) &serverAddress, &sin_length);

    //printf("%d bytes received from %s (port %d)\n",
    //ret, inet_ntoa(serverAddress.sin_addr), serverAddress.sin_port);
    //for (int i = 0 ; i < 60 ; i++) {
    //  printf("%c", msg[i]);
    //}
    //printf("\n");

    // Buffer
    unsigned char* buffer = new unsigned char[2048];
    memset(buffer, 0 , 2048);

    // Command
    char command[8];
    memset(command, 0, 8);
    strcpy(command, "CMD_LTM");

    // Destination
    int destination = 0;

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Pack data
    for (int i = 0 ; i < 60 ; i++) {
	packer.PackChar(msg[i]);
    }

    // Add message to queue
    add_message(buffer);

  }

  // Close socket
  close(serverSocketFileDescriptor);

  // Exit thread
  pthread_exit(NULL);

}

/*--------------------------------------------------------------------------*/

static void lambda_listen_cb(Fl_Button* b, void* v) {

  // Get port
  int port = 0;
  port = lambdaWindow -> GetPort();

  // Set non modal
  lambdaWindow -> set_non_modal();

  // Hide lambda window
  lambdaWindow -> hide();

  // Delete lambda window
  delete lambdaWindow;
  lambdaWindow = NULL;

  // Listen to LambdaTracker
  listenToLambdaTracker(port);

}

/*--------------------------------------------------------------------------*/

static void lambda_cancel_cb(Fl_Button* b, void* v) {

  // Set non modal
  lambdaWindow -> set_non_modal();

  // Hide lambda window
  lambdaWindow -> hide();

  // Delete lambda window
  delete lambdaWindow;
  lambdaWindow = NULL;  

}

/*--------------------------------------------------------------------------*/

static void listen_lambda() {

  // Calculate position of message window
  int posx = (mainWindow -> x()) +
    (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
    (255 / 2);
  int posy = (mainWindow -> y()) +
    (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
    (115 / 2);
  
  // Create new connect window
  lambdaWindow = 
      new Fl_Lambda(posx, posy, 255, 115, "Listen to LambdaTracker");

  // Set modal
  lambdaWindow -> set_modal();

  // Show connect window
  lambdaWindow -> show();

  // Set callback
  lambdaWindow -> callback((Fl_Callback*) lambda_cancel_cb);

  // Set listen button callback
  lambdaWindow -> 
      SetListenButtonCallback((Fl_Callback*) lambda_listen_cb, NULL);

  // Set cancel button callback
  lambdaWindow -> 
      SetCancelButtonCallback((Fl_Callback*) lambda_cancel_cb, NULL);

}

/*--------------------------------------------------------------------------*/

static void load_data() {

  // Calculate position of load data window
  int posx = (mainWindow -> x()) +
    (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
    (220 / 2);
  int posy = (mainWindow -> y()) +
    (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
    (305 / 2);

  // Create new load data winodw
  loadDataWindow = new Fl_Load_Data(posx, posy, 220, 305, "Load Data");

  // Set ok button callback
  loadDataWindow -> SetOKButtonCallback((Fl_Callback*) data_ok_cb, NULL);

  // Set cancel button callback
  loadDataWindow -> 
    SetCancelButtonCallback((Fl_Callback*) data_cancel_cb, NULL);

  // Set modal
  loadDataWindow -> set_modal();

  // Populate browser with dataset names
  for (int i = 0 ; i < datasetListSize ; i++) {
    loadDataWindow -> Add(datasetListName[i]);
  }

  // Show dialog
  loadDataWindow -> show();

}

/*--------------------------------------------------------------------------*/

static void load_script() {

  // Show file chooser
  char* filename = fl_file_chooser("Load Script", NULL, NULL);

  // Check if a file was selected
  if (filename != NULL) {

    // Run script
    run_script(filename);

  }

}

/*--------------------------------------------------------------------------*/

void *thread_function(void* ptr) {

  // Cast data
  scriptData* info = (scriptData*) ptr;

  // Get data members
  const char* filename = info -> scriptfile;
  UserInterfaceClient* client = info -> client;
  bool* cancelScript = info -> cancelScript;
  void (*cb)(unsigned char* message) = info -> cb;

  // File pointer
  FILE* fptr = NULL;
    
  // Attempt to open file
  if ((fptr = fopen(filename, "r")) == NULL) {
    
    // Tell main process that the script is finished

    // Buffer
    unsigned char* buffer = new unsigned char[2048];
    memset(buffer, 0 , 2048);

    // Command
    char command[8];
    memset(command, 0, 8);
    strcpy(command, "SCR_COM");

    // Destination
    int destination = 0;

    // Packer
    DataPacker packer;
    packer.SetBuffer(buffer, 2048);

    // Pack command
    for (int i = 0 ; i < 8 ; i++) {
      packer.PackChar(command[i]);
    }

    // Pack destination
    packer.PackInt(destination);

    // Create message
    char m[1024];
    sprintf(m, "Can not open script file %s", filename);

    // Pack data
    for (int i = 0 ; i < 1024 ; i++) {
      packer.PackChar(m[i]);
    }

    // Add message
    if (cb != NULL) {
      cb(buffer);
    }

    // Exit thread
    pthread_exit(NULL);

  }

  // Data read from file
  char data[1024];

  // Command number
  int commandNumber = 0;

  // Continue processing commands as long as they're in the file
  while (fscanf(fptr, "%s", data) != EOF) {

    // Check if script is canceled
    if (*cancelScript == true) {

      // Tell main process that the script is finished

      // Buffer
      unsigned char* buffer = new unsigned char[2048];
      memset(buffer, 0 , 2048);

      // Command
      char command[8];
      memset(command, 0, 8);
      strcpy(command, "SCR_CAN");

      // Destination
      int destination = 0;

      // Packer
      DataPacker packer;
      packer.SetBuffer(buffer, 2048);

      // Pack command
      for (int i = 0 ; i < 8 ; i++) {
        packer.PackChar(command[i]);
      }

      // Pack destination
      packer.PackInt(destination);

      // Close script file
      fclose(fptr);

      // Add message
      if (cb != NULL) {
        cb(buffer);
      }

      // Exit thread
      pthread_exit(NULL);

    }

    // Check if first command
    if (commandNumber == 0) {

      if(strcmp(data, "VRA_UI_SCRIPT")) {
        
        // Tell main process that the script is finished

        // Buffer
        unsigned char* buffer = new unsigned char[2048];
        memset(buffer, 0 , 2048);
        
        // Command
        char command[8];
        memset(command, 0, 8);
        strcpy(command, "SCR_COM");

        // Destination
        int destination = 0;

        // Packer
        DataPacker packer;
        packer.SetBuffer(buffer, 2048);
        
        // Pack command
        for (int i = 0 ; i < 8 ; i++) {
          packer.PackChar(command[i]);
        }

        // Pack destination
        packer.PackInt(destination);

        // Create message
        char m[1024];
        sprintf(m, "File %s is not a valid script file.", filename);

        // Pack data
        for (int i = 0 ; i < 1024 ; i++) {
          packer.PackChar(m[i]);
        }

        // Close file
        fclose(fptr);

        // Add message
        if (cb != NULL) {
          cb(buffer);
        }

        // Exit thread
        pthread_exit(NULL);

      }
      
      // Increment command number
      commandNumber++;

      // Start at top of loop
      continue;

    }

    // Load data
    if (!strcmp(data, "CMD_DAT")) {
      
      // Data name
      char dataname[1024];

      // Cache sizes
      int ramSize = -1;
      int vramSize = -1;
      
      // Read filename
      if (fscanf(fptr, "%s %d %d", dataname, &ramSize, &vramSize) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      
      // Send command
      client -> SendData(dataname, ramSize, vramSize);
      
    }
    
    // Disconnect
    else if (!strcmp(data, "CMD_DIS")) {

      // Disconnect
      disconnect();
      
    }
    
    // Exit
    else if (!strcmp(data, "CMD_EXI")) {
      
      // Send command
      //client -> SendCommandExit();
      
    }
    
    // Data prefix
    else if (!strcmp(data, "CMD_PRE")) {

      // Data name
      char dataprefix[1024];
      
      // Read filename
      if (fscanf(fptr, "%s", dataprefix) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      
      // Send command
      client -> SendPrefix(dataprefix);
      
      }
    
    // Render
    else if (!strcmp(data, "CMD_REN")) {
      
      // Send command
      client -> SendCommandRender();
      
    }
    
    // Rotation
    else if (!strcmp(data, "CMD_ROT")) {
      
      // Rotation matrix
      float R[16];
      
      // Read rotation matrix
      if (fscanf(fptr, "%f %f %f %f", 
                 &R[0], &R[1], &R[2], &R[3]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &R[4], &R[5], &R[6], &R[7]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &R[8], &R[9], &R[10], &R[11]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &R[12], &R[13], &R[14], &R[15]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      
      // Send command
      client -> SendRotationMatrix(R);
      
    }

    // Scale
    else if (!strcmp(data, "CMD_SCA")) {

      // Scale matrix
      float S[16];

      // Read scale matrix
      if (fscanf(fptr, "%f %f %f %f", 
                 &S[0], &S[1], &S[2], &S[3]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &S[4], &S[5], &S[6], &S[7]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &S[8], &S[9], &S[10], &S[11]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &S[12], &S[13], &S[14], &S[15]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      
      // Send command
      client -> SendScaleMatrix(S);

    }
    
    // Translation
    else if (!strcmp(data, "CMD_TRA")) {
      
      // Translation matrix
      float T[16];

      // Read translation matrix
      if (fscanf(fptr, "%f %f %f %f", 
                 &T[0], &T[1], &T[2], &T[3]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &T[4], &T[5], &T[6], &T[7]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &T[8], &T[9], &T[10], &T[11]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      if (fscanf(fptr, "%f %f %f %f", 
                 &T[12], &T[13], &T[14], &T[15]) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }
      
      // Send command
      client -> SendTranslationMatrix(T);
      
    }

    // Wait
    else if (!strcmp(data, "WAIT")) {
      
      // Time to sleep
      int time = 0;

      // Read time to sleep
      if (fscanf(fptr, "%d", &time) == EOF) {
        fprintf(stderr, "Error reading script file.\n");
      }      
      
      // Wait until all levels-of-detail are rendered
      if (time < 0) {

        // Wait until all levels have been rendered
        while((progressLevel + 1) < totalLevel) {
          usleep(100000);
        }

      }

      // Sleep a determined amount of seconds
      else {
       
        // Sleep
        usleep(time * 1000000);
        
      }

    }

    // Unknown command
    else {
      fprintf(stderr, "Unknown command %s. Attempting to continue...\n", 
              data);
    }
    
    // Increment command number
    commandNumber++;
    
  }
  
  // Close file
  fclose(fptr);
  
  // Tell main process that the script is finished
  
  // Buffer
  unsigned char* buffer = new unsigned char[2048];
  memset(buffer, 0 , 2048);
  
  // Command
  char command[8];
  memset(command, 0, 8);
  strcpy(command, "SCR_COM");

  // Destination
  int destination = 0;
  
  // Packer
  DataPacker packer;
  packer.SetBuffer(buffer, 2048);

  // Pack command
  for (int i = 0 ; i < 8 ; i++) {
    packer.PackChar(command[i]);
  }
  
  // Pack destination
  packer.PackInt(destination);
  
  // Create message
  char m[1024];
  sprintf(m, "Script completed.");

  // Pack data
  for (int i = 0 ; i < 1024 ; i++) {
    packer.PackChar(m[i]);
  }

  // Add message
  if (cb != NULL) {
    cb(buffer);
  }

  // Exit thread
  pthread_exit(NULL);

}

/*--------------------------------------------------------------------------*/

void run_script(const char* filename) {

  // Check if a file was selected
  if (filename != NULL) {

    // Setup script thread info
    scriptData info;
    info.client = client;
    info.scriptfile = filename;
    info.cancelScript = &_cancelScript;
    info.cb = add_message;

    // Reset cancel script
    _cancelScript = false;

    // Lock message mutex
    pthread_mutex_lock(&scriptMutex);

    // Unlock message mutex
    pthread_mutex_unlock(&scriptMutex);

    // Create thread listening for server updates
    pthread_create(&_thread,
                   NULL,
                   thread_function,
                   &info);

    // Calculate position of message window
    int posx = (mainWindow -> x()) +
      (((mainWindow -> x() + mainWindow -> w()) - mainWindow -> x()) / 2) -
      (195 / 2);
    int posy = (mainWindow -> y()) +
      (((mainWindow -> y() + mainWindow -> h()) - mainWindow -> y()) / 2) -
      (75 / 2);

    // Create new message window
    messageWindow = new Fl_Dialog(posx, posy, 195, 75, "Script");

    // Set ok button callback
    messageWindow -> SetOKButtonCallback((Fl_Callback*) mes_script_cb, NULL);

    // Set message
    messageWindow -> SetMessage("Script running. Cancel script?");

    // Set modal
    messageWindow -> set_modal();

    // Show dialog
    messageWindow -> show();
  }

  // Return
  return;

}

/*--------------------------------------------------------------------------*/

static void connect_cb(Fl_Button* b, void* v) {

  // Get host
  char host[1024];
  strcpy(host, connectWindow -> GetHost());

  // Get port
  int port = 0;
  port = connectWindow -> GetPort();

  // Set non modal
  connectWindow -> set_non_modal();

  // Hide connect window
  connectWindow -> hide();

  // Delete connect window
  delete connectWindow;
  connectWindow = NULL;

  // Connect to server
  connect(host, port);

}

/*--------------------------------------------------------------------------*/

static void exit_server() {

  // Send disconnect command
  if (client != NULL) {
    client -> SendCommandExit();
  }

  // Update connection
  statusBar -> copy_label("Not connected.");

  // Update progress bar
  progressBar -> copy_label("Level");

  // Update data bar
  dataBar -> copy_label("No data.");

  // Activate connect menu item
  mainWindow -> ActivateConnect();

  // Deactivate disconnect menu item
  mainWindow -> DeactivateDisconnect();

  // Deactivate shutdown menu item
  mainWindow -> DeactivateShutdown();

  // Show connect window
  connect_show();

}

/*--------------------------------------------------------------------------*/

static void exit_application() {

  // Send disconnect command
  if (client != NULL) {
    client -> SendCommandExit();
  }

  // Clean up observer
  if (observer != NULL) {
    delete observer;
  }

  // Clean up client
  if (client != NULL) {
    delete client;
  }

  // Clean up widget
  if (widget != NULL) {
    delete widget;
  }

  // Clean up overview
  if (overview != NULL) {
    delete overview;
  }

  // Clean up connect window
  if (connectWindow != NULL) {
    delete connectWindow;
  }

  // Clean up load data window
  if (loadDataWindow != NULL) {
    delete loadDataWindow;
  }

  // Clean up dataset list dataset file
  for (int i = 0 ; i < datasetListSize ; i++) {
    delete [] datasetListFile[i];
  }
  delete [] datasetListFile;

  // Clean up dataset list dataset name
  for (int i = 0 ; i < datasetListSize ; i++) {
    delete [] datasetListName[i];
  }
  delete [] datasetListName;

  // Clean up dataset list dataset prefix
  for (int i = 0 ; i < datasetListSize ; i++) {
    delete [] datasetListPrefix[i];
  }
  delete [] datasetListPrefix;

  // Destroy mutex
  pthread_mutex_destroy(&messageMutex);

  // Exit
  exit(0);

}

/*--------------------------------------------------------------------------*/

static void exit_cb(Fl_Button* b, void* v) {

  // Exit
  exit_application();

}

/*--------------------------------------------------------------------------*/

void message_data_cb(Fl_Button* b, void* v) {

  // Set non modal
  messageWindow -> set_non_modal();

  // Hide message window
  messageWindow -> hide();

  // Delete message window
  delete messageWindow;
  messageWindow = NULL;

  // Show connect window
  load_data();

}

/*--------------------------------------------------------------------------*/

void stopLambda_cb(Fl_Button* b, void* v) {

  // Set stop flag
  lambdaData* info = (lambdaData*) v;
  info -> stopFlag = true;

  // Hide message window
  stopLambdaWindow -> hide();

  // Delete message window
  delete stopLambdaWindow;
  stopLambdaWindow = NULL;

}

/*--------------------------------------------------------------------------*/

void message_cb(Fl_Button* b, void* v) {

  // Set non modal
  messageWindow -> set_non_modal();

  // Hide message window
  messageWindow -> hide();

  // Delete message window
  delete messageWindow;
  messageWindow = NULL;

  // Show connect window
  connect_show();

}

/*--------------------------------------------------------------------------*/

void mes_script_cb(Fl_Button* b, void* v) {

  // Canceling script
  _cancelScript = true;

}

/*--------------------------------------------------------------------------*/

static void overview_cb(Fl_Widget *w, void *data) {

  // Exit
  exit_application();

}

/*--------------------------------------------------------------------------*/

static void process_lambda_message(char* msg) {

  // Message state variables
  int id = 0;
  float xpos = 0.0;
  float ypos = 0.0;
  float angle = 0.0;
  int type = 0;

  // id
  char idb[11];
  memset(idb, 0, 11);
  strncpy(idb, &msg[10], 10);
  id = atoi(idb);

  // xpos
  char xposb[11];
  memset(xposb, 0, 11);
  strncpy(xposb, &msg[20], 10);
  xpos = atof(xposb);

  // ypos
  char yposb[11];
  memset(yposb, 0, 11);
  strncpy(yposb, &msg[30], 10);
  ypos = atof(yposb);

  // angle
  char angleb[11];
  memset(angleb, 0, 11);
  strncpy(angleb, &msg[40], 10);
  angle = atof(angleb);

  // type
  char typeb[11];
  memset(typeb, 0, 11);
  strncpy(typeb, &msg[50], 10);
  type = atoi(typeb);


  //printf("Message: %d %f %f %f %d\n", id, xpos, ypos, angle, type);


  // Translate operation
  if (id == 4) {

    // Up
    if (type == 1) {
      // End of push
      if (translateUp == false) {
	client -> SendCommandRender();
      }
      translateUp = true;

    }

    // Mouse down
    else if (type == 2) {
      
      // Start of push
      if (translateUp == true) {
	translatePuckPos[0] = xpos;
        translatePuckPos[1] = ypos;
      }
      translateUp = false;
      
    }
    
    // Calculate amount the puck has moved in world space
    float delta[2] = {((xpos - translatePuckPos[0]) * 
		       (_totalDisplayFrustum[1] - _totalDisplayFrustum[0])), 
		      ((ypos - translatePuckPos[1]) *
		       (_totalDisplayFrustum[3] - _totalDisplayFrustum[2]))};
    
    // The puck is activated by the user
    if (translateUp == false) {

      // Add the offset
      world_translate[0] += delta[0];
      world_translate[1] += delta[1];

      // Determine translation matrix
      float T[16];
      calc_trans(T, 
		 world_translate[0], 
		 world_translate[1], 
		 0.0);

      // Set the translation matrix
      overview -> SetTranslationMatrix(T);
      client -> SendTranslationMatrix(T);
    }
    
    // Update the translation puck position
    translatePuckPos[0] = xpos;
    translatePuckPos[1] = ypos;
    
  }

  
  // Scale operation
  if (id == 7) {
    
    // Up
    if (type == 1) {
      // End of push
      if (scaleUp == false) {
 	client -> SendCommandRender();
      }
      scaleUp = true;
      
    }
    
    // Mouse down
    else if (type == 2) {

      // Start of push
      if (scaleUp == true) {
	scalePuckPos[0] = xpos;
        scalePuckPos[1] = ypos;

	scalePuckPosI[0] = xpos;
        scalePuckPosI[1] = ypos;

        scaleAngleI = angle - 87.6;
        
        //float scale_pos_f[3] = {0.0, 0.0, 0.0};
        scale_pos_f[0] = 
          (scalePuckPos[0] *
           (_totalDisplayFrustum[1] -_totalDisplayFrustum[0])) +
          _totalDisplayFrustum[0];
        scale_pos_f[1] = 
          (scalePuckPos[1] * 
           (_totalDisplayFrustum[3] -_totalDisplayFrustum[2])) +
          _totalDisplayFrustum[2];

        // Position of the puck when clicked in WSC
        oriX = scale_pos_f[0];
        oriY = scale_pos_f[1];

        scale_pos_f[0] -= world_translate[0];
        scale_pos_f[1] -= world_translate[1];    
        scale_pos_f[0] /= world_scale[0];
        scale_pos_f[1] /= world_scale[1];

        goodx = scale_pos_f[0];
        goody = scale_pos_f[1];

      }
      scaleUp = false;
      
      // Calculate amount puck has been rotated
      float deltaA = 0.0;
      if ((angle - world_scale_angle) >= 100.0) {
        deltaA = -1.0;
      }
      else if((angle - world_scale_angle) < -100.0) {
        deltaA = 1.0;
      }
      else {
        deltaA = angle - world_scale_angle;
      }

      // Calculate amount puck has moved
      float delta1[2] = {xpos - scalePuckPos[0],
                         ypos - scalePuckPos[1]};

      // The puck is activated by the user
      if (scaleUp == false) {

        // Determine the base offset along puck axis
        float newX = (cos(-scaleAngleI*0.0174532925) * delta1[0]) + 
          (-sin(-scaleAngleI*0.0174532925) * delta1[1]);
        float newY = (-sin(scaleAngleI*0.0174532925) * delta1[0]) + 
          (cos(-scaleAngleI*0.0174532925) * delta1[1]);
        
        // Update world scale
        world_scale[0] *= 1.0 - ((newX * 2.25) + (newY * 2.25));
        world_scale[1] *= 1.0 - ((newX * 2.25) + (newY * 2.25));
        world_scale[2] *= 1.0 - ((newX * 2.25) + (newY * 2.25));

        // Don't let scale go below 1.0
        if (world_scale[0] < 1.0) world_scale[0] = 1.0;
        if (world_scale[1] < 1.0) world_scale[1] = 1.0;
        if (world_scale[2] < 1.0) world_scale[2] = 1.0;

        // Determine scale matrix
        float S[16];
        calc_scale(S, 
                   world_scale[0], 
                   world_scale[1], 
                   world_scale[2]);
        
        
        float scale_pos_f[3] = {0.0, 0.0, 0.0};
        scale_pos_f[0] = 
          (scalePuckPosI[0] * 
           (_totalDisplayFrustum[1] -_totalDisplayFrustum[0])) + 
          _totalDisplayFrustum[0];
        scale_pos_f[1] = 
          (scalePuckPosI[1] * 
           (_totalDisplayFrustum[3] -_totalDisplayFrustum[2])) + 
          _totalDisplayFrustum[2];
        
        // Determine new translation based on the scale
        world_translate[0] = (scale_pos_f[0]) - (goodx*world_scale[0]);
        world_translate[1] = (scale_pos_f[1]) - (goody*world_scale[1]);

        // Determine new translation matrix
        float T[16];
        calc_trans(T, 
                   world_translate[0], 
                   world_translate[1], 
                   0.0);
        
        // Update matrices
        overview -> SetScaleMatrix(S);
        client -> SendScaleMatrix(S);
        overview -> SetTranslationMatrix(T);
        client -> SendTranslationMatrix(T);
        
      }
      
    }

    // Update puck position and angle
    scalePuckPos[0] = xpos;
    scalePuckPos[1] = ypos;
    world_scale_angle = angle;
    
  }


  // Rotate operation
  if (id == 8) {
    
    // Up
    if (type == 1) {
      // End of push
      if (rotateUp == false) {
	client -> SendCommandRender();
      }
      rotateUp = true;
      
    }
    
    // Mouse down
    else if (type == 2) {
      
      // Start of push
      if (rotateUp == true) {

        // Normalize object's translation to (-1.0, 1.0)
        float nTx = (world_translate[0] - _totalDisplayFrustum[0]) /
          (_totalDisplayFrustum[1] - _totalDisplayFrustum[0]);
        nTx = (nTx * 2.0) - 1.0;
        float nTy = (world_translate[1] - _totalDisplayFrustum[2]) /
          (_totalDisplayFrustum[3] - _totalDisplayFrustum[2]);
        nTy = (nTy * 2.0) - 1.0;
        
        // Normalize puck's position to (-1.0, 1.0)
        float nXpos = (xpos * 2.0) - 1.0;
        float nYpos = (ypos * 2.0) - 1.0;
        
        // Start trackball
        lambdaTrackball.Start(nXpos - nTx, nYpos - nTy);

      }
      rotateUp = false;
     
    }
    
    // The puck is activated by the user
    if (rotateUp == false) {

      // Normalize object's translation to (-1.0, 1.0)
      float nTx = (world_translate[0] - _totalDisplayFrustum[0]) /
        (_totalDisplayFrustum[1] - _totalDisplayFrustum[0]);
      nTx = (nTx * 2.0) - 1.0;
      float nTy = (world_translate[1] - _totalDisplayFrustum[2]) /
        (_totalDisplayFrustum[3] - _totalDisplayFrustum[2]);
      nTy = (nTy * 2.0) - 1.0;

      // Normalize puck's position to (-1.0, 1.0)
      float nXpos = (xpos * 2.0) - 1.0;
      float nYpos = (ypos * 2.0) - 1.0;

      // Update trackball
      lambdaTrackball.Update(nXpos - nTx, nYpos - nTy);
      
      // Get rotation from trackball
      float R[16];
      lambdaTrackball.GetRotationMatrix(R);

      // Update matrix
      overview -> SetRotationMatrix(R);
      client -> SendRotationMatrix(R);

    }
    
  }

}

/*--------------------------------------------------------------------------*/

static void process_message(unsigned char* message) {
  
  // Command
  char command[8];
  memset(command, 0, 8);

  // Destination
  int destination = 0;

  // Data packer
  DataPacker unpacker;
  unpacker.SetBuffer(message, 2048);

  // Unpack command from buffer
  for (int i = 0 ; i < 8 ; i++) {
    unpacker.UnpackChar(&command[i]);
  }
  
  // Unpack destination from buffer
  unpacker.UnpackInt(&destination);
  

  // axis off
  if (!strcmp(command, "CMD_AOF")) {

    // Clear check box
    //g_miAxis.clear();
    //g_miAxis.show();
    //fprintf(stderr, "AxisOff\n");

  }


  // lambda table message
  else if (!strcmp(command, "CMD_LTM")) {

    // Unpack message
    char msg[60];
    for (int i = 0 ; i < 60 ; i++) {
      unpacker.UnpackChar(&msg[i]);
    }

    // Process
    process_lambda_message(msg);

  }


  // axis on
  else if (!strcmp(command, "CMD_AON")) {

    // Set check box
    //g_miAxis.set();
    //g_miAxis.show();
    //fprintf(stderr, "AxisOn %d\n", g_miAxis.value());

  }


  // clipping planes
  else if (!strcmp(command, "CMD_CLI")) {
    
    // Unpack data from buffer
    float near = 0.0;
    float far = 0.0;
    unpacker.UnpackFloat(&near);
    unpacker.UnpackFloat(&far);
    
  }
  
  
  // exit
  else if(!strcmp(command, "CMD_EXI")) {

    // Exit
    //exit_application();
    disconnect();

  }
  
  
  // data
  else if(!strcmp(command, "CMD_DAT")) {
    
    // Unpack data from buffer
    char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }

    // Update data bar
    if (strlen(data) <= 0) {
      dataBar -> copy_label("No data.");
    }
    else {
      dataBar -> copy_label(data);
    }

  }


  // dataset list file
  else if(!strcmp(command, "CMD_DSF")) {
    
    // Unpack data from buffer
    char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }

    // Copy data
    if (datasetListFile != NULL) {
      strcpy(datasetListFile[destination], data);
    }

  }


  // dataset list name
  else if(!strcmp(command, "CMD_DSN")) {
    
    // Unpack data from buffer
    char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }

    // Copy data
    if (datasetListName != NULL) {
      strcpy(datasetListName[destination], data);
    }

  }


  // dataset list prefix
  else if(!strcmp(command, "CMD_DSP")) {
    
    // Unpack data from buffer
    char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }

    // Copy data
    if (datasetListPrefix != NULL) {
      strcpy(datasetListPrefix[destination], data);
    }

  }


  // data set list size
  else if(!strcmp(command, "CMD_DSS")) {
    
    // Unpack data from buffer
    int data;
    unpacker.UnpackInt(&data);

    // Clean up dataset list dataset file
    for (int i = 0 ; i < datasetListSize ; i++) {
      delete [] datasetListFile[i];
    }
    delete [] datasetListFile;
    
    // Clean up dataset list dataset name
    for (int i = 0 ; i < datasetListSize ; i++) {
      delete [] datasetListName[i];
    }
    delete [] datasetListName;
    
    // Clean up dataset list dataset prefix
    for (int i = 0 ; i < datasetListSize ; i++) {
      delete [] datasetListPrefix[i];
    }
    delete [] datasetListPrefix;

    // Update data
    datasetListSize = data;

    // Allocate memory for dataset list dataset file name
    datasetListFile = new char*[datasetListSize];
    for (int i = 0 ; i < datasetListSize ; i++) {
      datasetListFile[i] = new char[1024];
    }

    // Allocate memory for dataset list dataset name
    datasetListName = new char*[datasetListSize];
    for (int i = 0 ; i < datasetListSize ; i++) {
      datasetListName[i] = new char[1024];
    }
    
    // Allocate memory for dataset list dataset prefix
    datasetListPrefix = new char*[datasetListSize];
    for (int i = 0 ; i < datasetListSize ; i++) {
      datasetListPrefix[i] = new char[1024];
    }

    // Update dataset lists
    for (int i = 0 ; i < datasetListSize ; i++) {
      client -> GetDatasetListFile(i);
      client -> GetDatasetListName(i);
      client -> GetDatasetListPrefix(i);
    }

  }
  
  
  // display size
  else if (!strcmp(command, "CMD_DIS")) {

    // Unpack data from buffer
    int width = 0;
    int height = 0;
    int flag = 0;
    float left = 0.0;
    float right = 0.0;
    float bottom = 0.0;
    float top = 0.0;
    unpacker.UnpackInt(&width);
    unpacker.UnpackInt(&height);
    unpacker.UnpackInt(&flag);
    unpacker.UnpackFloat(&left);
    unpacker.UnpackFloat(&right);
    unpacker.UnpackFloat(&bottom);
    unpacker.UnpackFloat(&top);
    
    // Full screen flag
    bool fullScreen = false;
    if (flag == 1) {
      fullScreen = true;
    }
    
  }


  // dataset type
  else if(!strcmp(command, "CMD_DTP")) {
    
    // Unpack data from buffer
    int data;
    unpacker.UnpackInt(&data);

    // Copy dataset type
    datasetType = data;

    // Set maximum scalar value
    if (datasetType == 8) {
      widget -> SetMaximumScalar(255.0);
    }
    else if (datasetType == 16) {
      widget -> SetMaximumScalar(65536.0);
    }

  }
  
  
  // frustum
  else if (!strcmp(command, "CMD_FRU")) {
    
    // Unpack data from buffer
    float left = 0.0;
    float right = 0.0;
    float bottom = 0.0;
    float top = 0.0;
    unpacker.UnpackFloat(&left);
    unpacker.UnpackFloat(&right);
    unpacker.UnpackFloat(&bottom);
    unpacker.UnpackFloat(&top);
    
  }
  
  
  // map
  else if (!strcmp(command, "CMD_MAP")) {
    
    // Unpack data from buffer
    unsigned char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackUnsignedChar(&data[i]);
    }
    
  }


  // overview vertices
  else if (!strcmp(command, "CMD_OVV")) {
    
    // Unpack data from buffer
    float data[24];
    for (int i = 0 ; i < 24 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }

    // Set vertices
    Vertex vertex[8];
    vertex[0].SetXYZ(data[0], data[1], data[2]);
    vertex[1].SetXYZ(data[3], data[4], data[5]);
    vertex[2].SetXYZ(data[6], data[7], data[8]);
    vertex[3].SetXYZ(data[9], data[10], data[11]);
    vertex[4].SetXYZ(data[12], data[13], data[14]);
    vertex[5].SetXYZ(data[15], data[16], data[17]);
    vertex[6].SetXYZ(data[18], data[19], data[20]);
    vertex[7].SetXYZ(data[21], data[22], data[23]);
    
    // Set property
    if (overview != NULL) {
      overview -> SetVertices(vertex);
    }

  }


  // progress
  else if (!strcmp(command, "CMD_PRO")) {
    
    // Unpack data from buffer
    int value = 0;
    int total = 0;
    unpacker.UnpackInt(&value);
    unpacker.UnpackInt(&total);

    // Update progress bar
    char progress[128];
    sprintf(progress, "Level %d of %d", value + 1, total);
    progressBar -> copy_label(progress);
    progressLevel = value;
    totalLevel = total;
    //fprintf(stderr, "Update Progress Bar: %d of %d\n", value, total);

  }
  
  
  // rotation matrix
  else if (!strcmp(command, "CMD_ROT")) {
    
    // Unpack data from buffer
    float data[16];
    for (int i = 0 ; i < 16 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }

    // Set property
    if (overview != NULL) {
      overview -> SetRotationMatrix(data);
    }
    
  }
  
  
  // scale matrix
  else if (!strcmp(command, "CMD_SCA")) {
    
    // Unpack data from buffer
    float data[16];
    for (int i = 0 ; i < 16 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }

    // Set property
    if (overview != NULL) {
      overview -> SetScaleMatrix(data);
    }
    
  }


  // slice frequency
  else if (!strcmp(command, "CMD_SLF")) {
    
    // Unpack data from buffer
    float data = 0.0;
    unpacker.UnpackFloat(&data);

    // Set slice frequency
    sliceFrequency = data;

  }
  

  // total display dimensions
  else if (!strcmp(command, "CMD_TDD")) {
    
    // Unpack data from buffer
    int w = 0;
    int h = 0;
    unpacker.UnpackInt(&w);
    unpacker.UnpackInt(&h);

    // Set property
    if (mainWindow != NULL) {
      mainWindow -> size(640, ((int) (((float) h / (float) w) * 640.0)) + 50);
    }

    // Set property
    if (overview != NULL) {
      overview -> size(640, (int) (((float) h / (float) w) * 640.0));
    }    

    // Set property
    if (statusBar != NULL) {
      statusBar -> position(0, mainWindow -> h() - 20);
    }    

    // Set progress
    if (progressBar != NULL) {
      progressBar -> position(213, mainWindow -> h() - 20);
    }    

    // Set property
    if (dataBar != NULL) {
      dataBar -> position(427, mainWindow -> h() - 20);
    }    
    
  }


  // total display frustum
  else if (!strcmp(command, "CMD_TDF")) {
    
    // Unpack data from buffer
    float data[6];
    unpacker.UnpackFloat(&data[0]);
    unpacker.UnpackFloat(&data[1]);
    unpacker.UnpackFloat(&data[2]);
    unpacker.UnpackFloat(&data[3]);
    unpacker.UnpackFloat(&data[4]);
    unpacker.UnpackFloat(&data[5]);

    // Set for LambdaTracker
    _totalDisplayFrustum[0] = data[0];
    _totalDisplayFrustum[1] = data[1];
    _totalDisplayFrustum[2] = data[2];
    _totalDisplayFrustum[3] = data[3];
    _totalDisplayFrustum[4] = data[4];
    _totalDisplayFrustum[5] = data[5];

    // Set property
    if (overview != NULL) {
      overview -> SetFrustum(data);
    }
    
  }
  
  
  // translation matrix
  else if (!strcmp(command, "CMD_TRA")) {
    
    // Unpack data from buffer
    float data[16];
    for (int i = 0 ; i < 16 ; i++) {
      unpacker.UnpackFloat(&data[i]);
    }
     
    // Set property
    if (overview != NULL) {
      overview -> SetTranslationMatrix(data);
    }

  }
  
  
  // vertical field of view
  else if (!strcmp(command, "CMD_VFV")) {
    
    // Unpack data from buffer
    float vFOV = 0.0;
    unpacker.UnpackFloat(&vFOV);
    
  }
  
  
  // viewport matrix
  else if (!strcmp(command, "CMD_VIE")) {
    
    // Unpack data from buffer
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    unpacker.UnpackInt(&x);
    unpacker.UnpackInt(&y);
    unpacker.UnpackInt(&w);
    unpacker.UnpackInt(&h);
    
  }


  // script canceled
  else if (!strcmp(command, "SCR_CAN")) {
    
    // Set non modal
    messageWindow -> set_non_modal();

    // Hide message window
    messageWindow -> hide();

    // Delete message window
    delete messageWindow;
    messageWindow = NULL;

    // Print message
    fl_message("Script canceled.");

  }


  // script complete
  else if (!strcmp(command, "SCR_COM")) {
    
    // Set non modal
    messageWindow -> set_non_modal();

    // Hide message window
    messageWindow -> hide();

    // Delete message window
    delete messageWindow;
    messageWindow = NULL;

    // Unpack data from buffer
    char data[1024];
    for (int i = 0 ; i < 1024 ; i++) {
      unpacker.UnpackChar(&data[i]);
    }

    // Print message
    fl_message("%s", data);

  }  
  

  // undefined command
  else {
  }
  

  // Redraw overview
  overview -> redraw();

  // Redraw transfer function widget
  widget -> Redraw();

}

/*--------------------------------------------------------------------------*/

static void render_cb() {

  // Send render command
  if (client != NULL) {
    client -> SendCommandRender();
  }

}

/*--------------------------------------------------------------------------*/

static void rotation_cb(float* m) {

  // Send rotation matrix
  if (client != NULL) {
    client -> SendRotationMatrix(m);
  }

}

/*--------------------------------------------------------------------------*/

static void scale_cb(float* m) {

  // Update scale for LambdaTable
  world_scale[0] = m[0];
  world_scale[1] = m[5];
  world_scale[2] = m[10];

  // Send scale matrix
  if (client != NULL) {
    client -> SendScaleMatrix(m);
  }

}

/*--------------------------------------------------------------------------*/

static void translation_cb(float* m) {

  // Update translation for LambdaTable
  world_translate[0] = m[12];
  world_translate[1] = m[13];

  // Send translation matrix
  if (client != NULL) {
    client -> SendTranslationMatrix(m);
  }

}

/*--------------------------------------------------------------------------*/

static void widget_cb(Fl_Widget *w, void *data) {

  // Do nothing
  return;

}

/*--------------------------------------------------------------------------*/

int main(int argc, char** argv) {

  // Create new client
  client = new UserInterfaceClient();
  if (client == NULL) {
    fprintf(stderr, "Could not create UserInterfaceClient.\n");
    exit_application();
  }

  // Create new command observer
  observer = new UserInterfaceClientConcreteCommand(add_message);
  if (observer == NULL) {
    fprintf(stderr, "Could not create UserInterfaceClientConcreteCommand.\n");
    exit_application();
  }

  // Set client observer
  client -> SetObserver(observer);


  // Create new widget
  widget = new Fl_Transfer_Function(100, 560, 640, 400, "Transfer Function");

  // Set maximum scalar value
  widget -> SetMaximumScalar(255.0);

  // Set callbacks
  widget -> callback(widget_cb);
  widget -> SetUpdateCallback(map_cb, widget);

  // Show widget
  widget -> show();


  // Create new overview
  overview = new Fl_Overview(0, 30, 640, 480);

  // Set callbacks
  overview -> SetRenderCallback(render_cb);
  overview -> SetRotationCallback(rotation_cb);
  overview -> SetScaleCallback(scale_cb);
  overview -> SetTranslationCallback(translation_cb);


  // Create new status bar
  statusBar = new Fl_Box(0, 510, 213, 20);

  // Set box type
  statusBar -> box(FL_FLAT_BOX);
  statusBar -> align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
  statusBar -> label("Not connected.");


  // Create new progress bar
  progressBar = new Fl_Box(213, 510, 214, 20);

  // Set box type
  progressBar -> box(FL_FLAT_BOX);
  progressBar -> align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
  progressBar -> label("Level");


  // Create new data bar
  dataBar = new Fl_Box(427, 510, 213, 20);

  // Set box type
  dataBar -> box(FL_FLAT_BOX);
  dataBar -> align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);
  dataBar -> label("No Data.");


  // Create new main window
  mainWindow = 
    new Fl_Main_Window(100, 0, 640, 530, "Volume Rendering Application");

  // Add overview window
  mainWindow -> add(overview);

  // Add status bar
  mainWindow -> add(statusBar);

  // Add progress bar
  mainWindow -> add(progressBar);

  // Add data bar
  mainWindow -> add(dataBar);

  // Create menu items
  Fl_Menu_Item menuitems[] = {
    {"&File", 0, 0, 0, FL_SUBMENU},
    {"&Connect to Server...", FL_CTRL + 'c', (Fl_Callback*) connect_show},
    {"&Disconnect from Server", FL_CTRL + 'd', (Fl_Callback*) disconnect, 
     0, FL_MENU_DIVIDER},
    {"&Shutdown Server", FL_CTRL + 's', (Fl_Callback*) exit_server, 0, 
     FL_MENU_DIVIDER},
#ifdef SAGE
    {"Listen to LambdaTracker...", 0, (Fl_Callback*) listen_lambda, 0,
     FL_MENU_DIVIDER | FL_MENU_INACTIVE},
#else
    {"Listen to LambdaTracker...", 0, (Fl_Callback*) listen_lambda, 0,
     FL_MENU_DIVIDER},
#endif
    {"&Load Data...", FL_CTRL + 'l', (Fl_Callback*) load_data, 0, 
     FL_MENU_DIVIDER},
    {"L&oad Script...", FL_CTRL + 'o', (Fl_Callback*) load_script, 0, 
     FL_MENU_DIVIDER},
    {"Load Transfer Function...", 0, (Fl_Callback*) load_tf},
    {"Save Transfer Function...", 0, (Fl_Callback*) save_tf, 0, 
     FL_MENU_DIVIDER},
    {"E&xit", FL_CTRL + 'q', (Fl_Callback*) exit_application, 0},
    {0},

    {"&View", 0, 0, 0, FL_SUBMENU},
    g_miAxis,
    g_miBoundingBox,
    g_miBrickBoxes,
    {"Slice Frequency...", 0, (Fl_Callback*) slice_frequency, 0, 
     FL_MENU_DIVIDER},
    {"Reset View", 0, (Fl_Callback*) reset_view, 0},
    {0},

    {0}
  };

  // Set menu items
  mainWindow -> SetMenuItems(menuitems);

  // Set callback
  mainWindow -> callback(overview_cb);

  // Show main window
  mainWindow -> show();


  // Check if user entered server on command line
  if (argc == 3) {

    // Connect to server 
    connect(argv[1], atoi(argv[2]));

  }

  // User did not enter server on command line
  else {

    // Show connect window
    connect_show();

  }

  // Event loop
  while (true) { 

    // Wait for an event
    Fl::wait(0.1);

    // Lock message mutex
    pthread_mutex_lock(&messageMutex);

    // Process messages in queue
    while (messageQueue.empty() == false) {
      
      // Get message
      unsigned char* message = messageQueue.front();

      // Remove message from queue
      messageQueue.pop();

      // Process message
      process_message(message);

      // Clean up message
      delete [] message;

    }

    // Unlock message mutex
    pthread_mutex_unlock(&messageMutex);

  }


  // Return
  return 0;

}

/*--------------------------------------------------------------------------*/
