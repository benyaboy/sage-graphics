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

#include <GL/glut.h>
#include <stdlib.h>
#include <unistd.h>

#include "VolumeRenderer.h"

/*--------------------------------------------------------------------------*/

VolumeRenderer::VolumeRenderer() {

  // Message
  Assert("Constructor begin.");

  // Initialize observers
  m_abortRenderObserver = NULL;
  m_swapBuffersObserver = NULL;

  // Initialize axis flag
  m_axisFlag = false;

  // Initialize axis position
  m_axisPosition[0] = 0.0;
  m_axisPosition[1] = 0.0;
  m_axisPosition[2] = 0.0;

  // Initialize bounding box flag
  m_boundingBoxFlag = false;

  // Initialize brick box flag
  m_brickBoxFlag = false;

  // Initialize RAM cache
  m_cacheRAM = NULL;
  m_cacheRAM = new CacheRAM(50);

  // Initialize VRAM cache
  m_cacheVRAM = NULL;
  m_cacheVRAM = new CacheVRAM(10);

  // Initialize data
  m_data = NULL;
  
  // Initialize depth list
  m_depthList = NULL;

  // Initialize frustum
  m_frustum[0] = 0.0;
  m_frustum[1] = 0.0;
  m_frustum[2] = 0.0;
  m_frustum[3] = 0.0;
  m_frustum[4] = 0.0;
  m_frustum[5] = 0.0;
  
  // Initialize initialized flag
  m_isInitialized = false;
  
  // Initialize map loaded flag
  m_isMapLoaded = false;

  // Initialize map
  m_map = NULL;

  // Initialize map dimensions
  m_mapDimensions[0] = 0;
  m_mapDimensions[1] = 1;
  
  // Initialize map texture
  m_mapTexture = 0;

  // Initialize number of nodes on render list
  m_numberOfNodesOnRenderList = 0;

  // Initialize original map
  m_originalMap = m_map;

  // Initialize original slice frequency
  m_originalSliceFrequency = 1.0;

  // Initialize queue for rendering
  m_queue = NULL;

  // Initialize traversal queue
  m_queueTraversal = NULL;

  // Initialize visible node queue
  m_queueVisible = NULL;
  
  // Initialize render list
  m_renderList = NULL;

  // Initialize rotation matrix
  m_R[0] = 1.0; m_R[4] = 0.0; m_R[8] = 0.0; m_R[12] = 0.0;
  m_R[1] = 0.0; m_R[5] = 1.0; m_R[9] = 0.0; m_R[13] = 0.0;
  m_R[2] = 0.0; m_R[6] = 0.0; m_R[10] = 1.0; m_R[14] = 0.0;
  m_R[3] = 0.0; m_R[7] = 0.0; m_R[11] = 0.0; m_R[15] = 1.0;
  
  // Initialize scale matrix
  m_S[0] = 1.0; m_S[4] = 0.0; m_S[8] = 0.0; m_S[12] = 0.0;
  m_S[1] = 0.0; m_S[5] = 1.0; m_S[9] = 0.0; m_S[13] = 0.0;
  m_S[2] = 0.0; m_S[6] = 0.0; m_S[10] = 1.0; m_S[14] = 0.0;
  m_S[3] = 0.0; m_S[7] = 0.0; m_S[11] = 0.0; m_S[15] = 1.0;

  // Initialize slice frequency
  m_sliceFrequency = m_originalSliceFrequency;
  
  // Initialize translation matrix
  m_T[0] = 1.0; m_T[4] = 0.0; m_T[8] = 0.0; m_T[12] = 0.0;
  m_T[1] = 0.0; m_T[5] = 1.0; m_T[9] = 0.0; m_T[13] = 0.0;
  m_T[2] = 0.0; m_T[6] = 0.0; m_T[10] = 1.0; m_T[14] = 0.0;
  m_T[3] = 0.0; m_T[7] = 0.0; m_T[11] = 0.0; m_T[15] = 1.0;
  
  // Initialize viewport
  m_viewport[0] = 0;
  m_viewport[1] = 0;
  m_viewport[2] = 0;
  m_viewport[3] = 0;
  
  // Message
  Assert("Constructor end.");

}

/*--------------------------------------------------------------------------*/

VolumeRenderer::~VolumeRenderer() {

  // Message
  Assert("Destructor begin.");

  // Delete queue
  if (m_queue != NULL) {
    delete m_queue;
  }

  // Delete old traversal queue
  if (m_queueTraversal != NULL) {
    delete m_queueTraversal;
  }

  // Delete old visible node queue
  if (m_queueVisible != NULL) {
    delete m_queueVisible;
  }

  // Delete old render list
  if (m_renderList != NULL) {
    delete m_renderList;
  }

  // Delete old depth list
  if (m_depthList != NULL) {
    delete m_depthList;
  }

 // Delete cache
  if (m_cacheRAM != NULL) {
    delete m_cacheRAM;
  }

 // Delete cache
  if (m_cacheVRAM != NULL) {
    delete m_cacheVRAM;
  }

  // Delete data
  if (m_data != NULL) {
    delete m_data;
  }

  // Delete map
  if (m_map != NULL) {
    delete [] m_map;
  }

  // Delete original color and opacity map
  if (m_originalMap != NULL) {
    delete [] m_originalMap;
  }

  // Uninitialize Cg
  if (m_isInitialized == true) {
    UnInitCg();
  }

  // Message
  Assert("Destructor end.");

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::Assert(const char* message) {

#ifdef LOG

  // Print message to log
  char buffer[1024];
  sprintf(buffer, "VolumeRenderer: %s", message);
  Log::Assert(buffer);

#endif

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::BuildVisibleNodeQueue() {

  // Message
  Assert("Build visible node queue begin.");

  // Current node
  OctreeNode* current = NULL;

  // Child node
  OctreeNode* child = NULL;

  // ModelView matrix
  float mv[16];

  // Projection matrix
  float p[16];

  // A node's relation to current view
  int result = 0;

  // Node vertices
  float x0, y0, z0;
  float x1, y1, z1;
  float x2, y2, z2;
  float x3, y3, z3;
  float x4, y4, z4;
  float x5, y5, z5;
  float x6, y6, z6;
  float x7, y7, z7;


  // Get MV and P from OpenGL
  glLoadIdentity();
  glPushMatrix();
  glMultMatrixf(m_T);
  glMultMatrixf(m_S);
  glMultMatrixf(m_R);
  glGetFloatv(GL_MODELVIEW_MATRIX, mv);
  glGetFloatv(GL_PROJECTION_MATRIX, p);
  glPopMatrix();
  glLoadIdentity();

  // Setup frustum planes calculator
  m_frustumPlanes.SetModelViewMatrix(mv);
  m_frustumPlanes.SetProjectionMatrix(p);
  m_frustumPlanes.CalculateFrustumPlanes();


  // Clear traversal queue
  m_queueTraversal -> Clear();

  // Clear queue of visible nodes
  m_queueVisible -> Clear();


  // Check if there is an octree
  if (m_data == NULL) {

    // No octree so return
    return;

  }

  // Get root node
  current = m_data -> GetRoot();


  // Check if there is a root node
  if (current == NULL) {

    // No root node so return
    return;

  }

  // Get root node's vertices
  current -> GetVertex(0, &x0, &y0, &z0);
  current -> GetVertex(1, &x1, &y1, &z1);
  current -> GetVertex(2, &x2, &y2, &z2);
  current -> GetVertex(3, &x3, &y3, &z3);
  current -> GetVertex(4, &x4, &y4, &z4);
  current -> GetVertex(5, &x5, &y5, &z5);
  current -> GetVertex(6, &x6, &y6, &z6);
  current -> GetVertex(7, &x7, &y7, &z7);

  // Determine root node's relation to current view
  result = m_frustumPlanes.CalculateRelationToFrustum(x0, y0, z0,
                                                      x1, y1, z1,
                                                      x2, y2, z2,
                                                      x3, y3, z3,
                                                      x4, y4, z4,
                                                      x5, y5, z5,
                                                      x6, y6, z6,
                                                      x7, y7, z7);

  // Check if root node is in current view
  if (result == INSIDE_FRUSTUM || result == INTERSECT_FRUSTUM) {

    // Insert root node in traversal queue
    m_queueTraversal -> Insert(current);

  }


  // Traverse octree
  while (m_queueTraversal -> IsEmpty() == false) {
    
    // Remove node from traversal queue
    current = m_queueTraversal -> Remove();

    // Insert node in visible node queue
    m_queueVisible -> Insert(current);

    // Get each of the current node's children
    for (int i = 0 ; i < 8 ; i++) {

      // Get child i
      child = current -> GetChild(i);

      // Check if child exists
      if (child != NULL) {

        // Get child node's vertices
        child -> GetVertex(0, &x0, &y0, &z0);
        child -> GetVertex(1, &x1, &y1, &z1);
        child -> GetVertex(2, &x2, &y2, &z2);
        child -> GetVertex(3, &x3, &y3, &z3);
        child -> GetVertex(4, &x4, &y4, &z4);
        child -> GetVertex(5, &x5, &y5, &z5);
        child -> GetVertex(6, &x6, &y6, &z6);
        child -> GetVertex(7, &x7, &y7, &z7);

        // Determine child node's relation to current view
        result = m_frustumPlanes.CalculateRelationToFrustum(x0, y0, z0,
                                                            x1, y1, z1,
                                                            x2, y2, z2,
                                                            x3, y3, z3,
                                                            x4, y4, z4,
                                                            x5, y5, z5,
                                                            x6, y6, z6,
                                                            x7, y7, z7);

        // Check if root node is in current view
        if (result == INSIDE_FRUSTUM || result == INTERSECT_FRUSTUM) {

          // Insert child in traversal queue
          m_queueTraversal -> Insert(child);

        }

      }

    }

  }

  // Message
  Assert("Build visible node queue end.");

  // Message
  //Assert("Printing visible node queue begin.");

  // Print visible node queue
  //_queueVisible -> Print();

  // Message
  //Assert("Printing visible node queue end.");

  // Return
  return;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::DisableCg() {

  // Disable profile
  cgGLDisableProfile(m_cgFProfile);

  // Disable parameters
  cgGLDisableTextureParameter(m_cgFMapTexture);
  cgGLDisableTextureParameter(m_cgFDataTexture);

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::DisableTextureGL() {

  // Disable blending
  glDisable(GL_BLEND);

  // Disable alpha test
  glDisable(GL_ALPHA_TEST);

  // Disable 3D textures
  glDisable(GL_TEXTURE_3D);

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::EnableCg() {

  // Bind program
  cgGLBindProgram(m_cgFProgram);

  // Enable profile
  cgGLEnableProfile(m_cgFProfile);

  // Set parameters
  cgGLSetTextureParameter(m_cgFMapTexture, m_mapTexture);
  cgGLSetTextureParameter(m_cgFDataTexture, 0);
  
  // Enable parameters
  cgGLEnableTextureParameter(m_cgFMapTexture);
  cgGLEnableTextureParameter(m_cgFDataTexture);

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::EnableTextureGL() {

  // Disable lighting
  glDisable(GL_LIGHTING);

  // Disable culling
  glDisable(GL_CULL_FACE);

  // Set polygon modes
  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_FILL);

  // Enable 3D textures
  glEnable(GL_TEXTURE_3D);

  // Enable alpha test
  glEnable(GL_ALPHA_TEST);

  // Enable blending
  glEnable(GL_BLEND);

  // Set blend function
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

/*---------------------------------------------------------------------------*/

Octree* VolumeRenderer::GetData() {

  // Return data
  return m_data;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetFrustum(float* left, float* right, float* bottom,
                                float* top, float* near, float* far) {

  // Get frustum
  *left = m_frustum[0];
  *right = m_frustum[1];
  *bottom = m_frustum[2];
  *top = m_frustum[3];
  *near = m_frustum[4];
  *far = m_frustum[5];

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetFrustum(float frustum[6]) {

  // Get frustum
  frustum[0] = m_frustum[0];
  frustum[1] = m_frustum[1];
  frustum[2] = m_frustum[2];
  frustum[3] = m_frustum[3];
  frustum[4] = m_frustum[4];
  frustum[5] = m_frustum[5];

}

/*---------------------------------------------------------------------------*/

unsigned char* VolumeRenderer::GetMap() {

  // Return map
  return m_map;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetR(double m[16]) {

  // Get rotation matrix
  m[0] = m_R[0];  m[4] = m_R[4];  m[8] = m_R[8];    m[12] = m_R[12];
  m[1] = m_R[1];  m[5] = m_R[5];  m[9] = m_R[9];    m[13] = m_R[13];
  m[2] = m_R[2];  m[6] = m_R[6];  m[10] = m_R[10];  m[14] = m_R[14];
  m[3] = m_R[3];  m[7] = m_R[7];  m[11] = m_R[11];  m[15] = m_R[15];

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetS(double m[16]) {

  // Get scale matrix
  m[0] = m_S[0];  m[4] = m_S[4];  m[8] = m_S[8];    m[12] = m_S[12];
  m[1] = m_S[1];  m[5] = m_S[5];  m[9] = m_S[9];    m[13] = m_S[13];
  m[2] = m_S[2];  m[6] = m_S[6];  m[10] = m_S[10];  m[14] = m_S[14];
  m[3] = m_S[3];  m[7] = m_S[7];  m[11] = m_S[11];  m[15] = m_S[15];

}

/*---------------------------------------------------------------------------*/

double VolumeRenderer::GetSliceFrequency() {

  // Return slice frequency
  return m_sliceFrequency;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetT(double m[16]) {

  // Get translation matrix
  m[0] = m_T[0];  m[4] = m_T[4];  m[8] = m_T[8];    m[12] = m_T[12];
  m[1] = m_T[1];  m[5] = m_T[5];  m[9] = m_T[9];    m[13] = m_T[13];
  m[2] = m_T[2];  m[6] = m_T[6];  m[10] = m_T[10];  m[14] = m_T[14];
  m[3] = m_T[3];  m[7] = m_T[7];  m[11] = m_T[11];  m[15] = m_T[15];

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetViewport(int* x, int* y, int* w, int* h) {

  // Get viewport
  *x = m_viewport[0];
  *y = m_viewport[1];
  *w = m_viewport[2];
  *h = m_viewport[3];

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::GetViewport(int viewport[4]) {

  // Get viewport
  viewport[0] = m_viewport[0];
  viewport[1] = m_viewport[1];
  viewport[2] = m_viewport[2];
  viewport[3] = m_viewport[3];

}

/*--------------------------------------------------------------------------*/

bool VolumeRenderer::Init() {
  
  // Initialize Cg
  return InitCg();

}

/*---------------------------------------------------------------------------*/

bool VolumeRenderer::InitCg() {

  // Required profile is not supported
  if (!cgGLIsProfileSupported(CG_PROFILE_ARBFP1)) {

    // Return false
    return false;

  }

  // Set profile
  m_cgFProfile = CG_PROFILE_ARBFP1;

  // Create Cg context
  m_cgContext = cgCreateContext();

  // Create program
  m_cgFProgram = cgCreateProgramFromFile(m_cgContext,
                                         CG_SOURCE,
                                         "Fragment.cg",
                                         m_cgFProfile,
                                         "Fragment",
                                         0);

  // Load program
  cgGLLoadProgram(m_cgFProgram);

  // Get parameters
  m_cgFMapTexture = cgGetNamedParameter(m_cgFProgram, "MapTexture");
  m_cgFDataTexture = cgGetNamedParameter(m_cgFProgram, "DataTexture");

  // Set initialized flag
  m_isInitialized = true;

  // Return true
  return true;

}

/*---------------------------------------------------------------------------*/

int VolumeRenderer::Intersect(double p0[3], double p1[3],       
                              double t0[3], double t1[3],
                              double v0[3], double v1[3],
                              double sp[3], double sn[3],
                              double pnew[3], double tnew[3], double vnew[3]) {
  
  float t = 
    ((sn[0] * (sp[0] - p0[0]) + 
      sn[1] * (sp[1] - p0[1]) + 
      sn[2] * (sp[2] - p0[2])) / 
     (sn[0] * (p1[0] - p0[0]) + 
      sn[1] * (p1[1] - p0[1]) + 
      sn[2] * (p1[2] - p0[2])));
 
  // Intersection
  if ((t >= 0) && (t <= 1))  {

    // Compute line intersection
    pnew[0] = p0[0] + t * (p1[0] - p0[0]); 
    pnew[1] = p0[1] + t * (p1[1] - p0[1]); 
    pnew[2] = p0[2] + t * (p1[2] - p0[2]);

    // Compute texture interseciton
    tnew[0] = t0[0] + t * (t1[0] - t0[0]); 
    tnew[1] = t0[1] + t * (t1[1] - t0[1]); 
    tnew[2] = t0[2] + t * (t1[2] - t0[2]);

    // Compute view coordinate intersections
    vnew[0] = v0[0] + t * (v1[0] - v0[0]); 
    vnew[1] = v0[1] + t * (v1[1] - v0[1]); 
    vnew[2] = v0[2] + t * (v1[2] - v0[2]);

    // Return intersection
    return 1;

  }

  // No intersection
  return 0;

} 

/*--------------------------------------------------------------------------*/

bool VolumeRenderer::IsInitialized() {

  // Check if object is initialized
  if (m_isInitialized == false) {
    fprintf(stderr, "VolumeRenderer: Call Init before any other method.\n");
  }
  
  return m_isInitialized;
  
}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::LoadMapTexture(int w, int h) {

  // Enable 2D textures
  glEnable(GL_TEXTURE_2D);

  // Generate texture
  glGenTextures(1, &m_mapTexture);

  // Bind texture
  glBindTexture(GL_TEXTURE_2D, m_mapTexture);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Load texture data
  glTexImage2D(GL_TEXTURE_2D, 
               0, 
               GL_RGBA8, 
               w,
               h,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE, 
               m_map);

  // Disable 2D textures
  glDisable(GL_TEXTURE_2D);

  // Set flag
  m_isMapLoaded = true;

}

/*---------------------------------------------------------------------------*/

bool VolumeRenderer::Render() {

  // Message
  char message[4096];
  sprintf(message, "Render begin.");
  Assert(message);

  // Message
  sprintf(message,
          "  T: %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
          m_T[0], m_T[4], m_T[8], m_T[12], m_T[1], m_T[5], m_T[9], m_T[13],
          m_T[2], m_T[6], m_T[10], m_T[14], m_T[3], m_T[7], m_T[11], m_T[15]);
  Assert(message);

  // Message
  sprintf(message,
          "  R: %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
          m_R[0], m_R[4], m_R[8], m_R[12], m_R[1], m_R[5], m_R[9], m_R[13],
          m_R[2], m_R[6], m_R[10], m_R[14], m_R[3], m_R[7], m_R[11], m_R[15]);
  Assert(message);

  // Message
  sprintf(message,
          "  S: %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
          m_S[0], m_S[4], m_S[8], m_S[12], m_S[1], m_S[5], m_S[9], m_S[13],
          m_S[2], m_S[6], m_S[10], m_S[14], m_S[3], m_S[7], m_S[11], m_S[15]);
  Assert(message);

  // Number of times swap buffer callback is executed
  int called = 0;

  // ModelView matrix
  float mv[16];

  // Octree node
  OctreeNode* node = NULL;

  // Previous level rendered
  int preLevel = 0;

  // Transformed node center used for sorting
  float transformedCenter[3];

  // Untransformed node center used for sorting
  float untransformedCenter[3];

  // Node vertices used for sorting
  Vertex v0, v1, v2, v3, v4;

  // Level 0 vertices used for bounding box
  Vertex ov0, ov1, ov2, ov3, ov4, ov5, ov6, ov7;


  // Check if user called Init
  IsInitialized();


  // Check that render list exists
  if (m_renderList == NULL) {

    // Message
    sprintf(message, "Render end. No render list.");
    Assert(message);

    // Return
    return false;

  }

  // Check that depth list exists
  if (m_depthList == NULL) {

    // Message
    sprintf(message, "Render end. No depth list.");
    Assert(message);

    // Return
    return false;

  }


  // Build queue of visible nodes for current view
  BuildVisibleNodeQueue();


  // Clear number of nodes on render list
  m_numberOfNodesOnRenderList = 0;


  // Set viewport and frustum
  SetViewportAndFrustum();


  // Get MV from OpenGL
  glLoadIdentity();
  glPushMatrix();
  glMultMatrixf(m_T);
  glMultMatrixf(m_S);
  glMultMatrixf(m_R);
  glGetFloatv(GL_MODELVIEW_MATRIX, mv);
  glPopMatrix();
  glLoadIdentity();


  // Clear the back buffer
  glClear(GL_COLOR_BUFFER_BIT);


  // Render axis
  if (m_axisFlag == true) {
    RenderAxis();
  }


  // Render all levels except the last

  // Render each node
  while (m_queueVisible -> IsEmpty() == false) {
    
    // Get next node from queue
    node = m_queueVisible -> Remove();

    // Save vertices for bounding box
    if (node -> GetLevel() == 0) {
      node -> GetVertex(0, &ov0);
      node -> GetVertex(1, &ov1);
      node -> GetVertex(2, &ov2);
      node -> GetVertex(3, &ov3);
      node -> GetVertex(4, &ov4);
      node -> GetVertex(5, &ov5);
      node -> GetVertex(6, &ov6);
      node -> GetVertex(7, &ov7);
    }

    // Check if the node just removed from queue is from a different level
    // or if the end of the queue has been reached
    if (node -> GetLevel() != preLevel) {

      // Delete old map
      if (m_map != NULL) {
        delete [] m_map;
      }

      // Allocate memory for a new map
      m_map = new unsigned char[m_mapDimensions[0] * m_mapDimensions[1] * 4];

      // Scale alpha values according to the slice frequency
      //ScaleAlpha(m_originalSliceFrequency * (preLevel+1), 
      //           m_originalSliceFrequency, m_map, 
      //           m_originalMap, m_mapDimensions[0], m_mapDimensions[1]);
      ScaleAlpha(m_originalSliceFrequency, 
                 m_originalSliceFrequency, m_map, 
                 m_originalMap, m_mapDimensions[0], m_mapDimensions[1]);
//ScaleAlpha(m_originalSliceFrequency*(m_data->GetNumberOfLevels()-preLevel+1),
//             m_originalSliceFrequency,
//             m_map, 
//             m_originalMap, m_mapDimensions[0], m_mapDimensions[1]);

      // Load map texture
      LoadMapTexture(m_mapDimensions[0], m_mapDimensions[1]);

      // Message
      sprintf(message, "Render level %d begin.", preLevel);
      Assert(message);

      // Calculate depth of each node on render list
      for (int i = 0 ; i < m_numberOfNodesOnRenderList ; i++) {

        // Get node vertices
        m_renderList[i] -> GetVertex(0, &v0);
        m_renderList[i] -> GetVertex(1, &v1);
        m_renderList[i] -> GetVertex(2, &v2);
        m_renderList[i] -> GetVertex(3, &v3);
        m_renderList[i] -> GetVertex(4, &v4);

        // Calculate center of each brick
        //untransformedCenter[0] = ((v3.GetX() - v0.GetX()) / 2.0) + v0.GetX();
        //untransformedCenter[1] = ((v4.GetY() - v0.GetY()) / 2.0) + v0.GetY();
        //untransformedCenter[2] = ((v1.GetZ() - v0.GetZ()) / 2.0) + v0.GetZ();
        untransformedCenter[0] = ((v1.GetX() - v0.GetX()) / 2.0) + v0.GetX();
        untransformedCenter[1] = ((v4.GetY() - v0.GetY()) / 2.0) + v0.GetY();
        untransformedCenter[2] = ((v2.GetZ() - v0.GetZ()) / 2.0) + v0.GetZ();

        // Calculate transformed center based on MV
        transformedCenter[0] = 
          mv[0] * untransformedCenter[0] +
          mv[4] * untransformedCenter[1] + 
          mv[8] * untransformedCenter[2] +
          mv[12];
        transformedCenter[1] = 
          mv[1] * untransformedCenter[0] +
          mv[5] * untransformedCenter[1] + 
          mv[9] * untransformedCenter[2] +
          mv[13];
        transformedCenter[2] = 
          mv[2] * untransformedCenter[0] +
          mv[6] * untransformedCenter[1] + 
          mv[10] * untransformedCenter[2];

        // Only use depth
        m_depthList[i] = transformedCenter[2];

      }

      // Sort back-to-front
      for (int i = 0 ; i < m_numberOfNodesOnRenderList ; i++) {
        for (int j = i+1 ; j < m_numberOfNodesOnRenderList ; j++) {
          if (m_depthList[i] > m_depthList[j]) {
            float tmpDepth = m_depthList[i];
            OctreeNode* tmpNode = m_renderList[i];
            m_depthList[i] = m_depthList[j];
            m_renderList[i] = m_renderList[j];
            m_depthList[j] = tmpDepth;
            m_renderList[j] = tmpNode;
          }
        }
      }

      // Message
      //Assert("Printing render list begin.");

      // Print render list
      //for (int i = 0 ; i < _numberOfNodesOnRenderList ; i++) {
      //  _renderList[i] -> Print();
      //}

      // Message
      //Assert("Printing render list end.");

      // Render nodes on render list
      for (int i = 0 ; i < m_numberOfNodesOnRenderList ; i++) {

        // Render node i
        RenderNode(m_renderList[i]);

        // Only allow a render to be aborted if it's not level 0
        if (m_renderList[i] -> GetLevel() > 0) {
        
          // Check for an abort render observer
          if (m_abortRenderObserver != NULL) {
            
            // Check if the render has been aborted
            if (m_abortRenderObserver -> Execute(0) == true) {
              
              // Execute swap buffers as many times as there are levels
              for (int i = called ; i < m_data -> GetNumberOfLevels() ; i++) {
                
                // Check is swap buffers observer exists
                if (m_swapBuffersObserver != NULL) {
                  
                  // Execute swap buffers observer
                  m_swapBuffersObserver -> Execute(-1);
                  
                }
                
                // Clear back buffer
                glClear(GL_COLOR_BUFFER_BIT);

                // Render axis
                if (m_axisFlag == true) {
                  RenderAxis();
                }
                
                // Increment number of times swap buffers observer was executed
                called++;
              }

              // Message
              sprintf(message, "Render end. Level %d aborted.", preLevel);
              Assert(message);
              
              // Return false
              return false;
            }
            
          }

        }
        
      }

      // Render bounding box
      if (m_boundingBoxFlag == true) {
        RenderBoundingBox(ov0, ov1, ov2, ov3, ov4, ov5, ov6, ov7);
      }

      // Execute progress observer
      if (m_progressObserver != NULL) {

        // Execute progress observer
        m_progressObserver -> Execute(preLevel);

      }
      
      // Execute swap buffers observer
      if (m_swapBuffersObserver != NULL) {

        // Execute swap buffers observer
        m_swapBuffersObserver -> Execute(0);

        // Increment number of times swap buffers observer has been executed
        called++;

      }

      // Clear number of nodes on render list
      m_numberOfNodesOnRenderList = 0;

      // Clear the back buffer
      glClear(GL_COLOR_BUFFER_BIT);

      // Render axis
      if (m_axisFlag == true) {
        RenderAxis();
      }

      // Message
      sprintf(message, "Render level %d end.", preLevel);
      Assert(message);

    }

    // Add node to render list
    m_renderList[m_numberOfNodesOnRenderList] = node;

    // Increment number of nodes on render list
    m_numberOfNodesOnRenderList++;

    // Set the previous level
    preLevel = node -> GetLevel();

  }


  // Render the last level: this code is very redundant

  // Message
  sprintf(message, "Render level %d begin.", preLevel);
  Assert(message);

  // Delete old map
  if (m_map != NULL) {
    delete [] m_map;
  }

  // Allocate memory for a new map
  m_map = new unsigned char[m_mapDimensions[0] * m_mapDimensions[1] * 4];

  // Scale alpha values according to the slice frequency
  //ScaleAlpha(m_originalSliceFrequency * (preLevel+1), 
  //           m_originalSliceFrequency, m_map, 
  //           m_originalMap, m_mapDimensions[0], m_mapDimensions[1]);
  ScaleAlpha(m_originalSliceFrequency, 
             m_originalSliceFrequency, m_map, 
            m_originalMap, m_mapDimensions[0], m_mapDimensions[1]);
  //ScaleAlpha(m_originalSliceFrequency*(m_data->GetNumberOfLevels()-preLevel+1),
  //           m_originalSliceFrequency,
  //           m_map, 
  //           m_originalMap, m_mapDimensions[0], m_mapDimensions[1]);

  // Load map texture
  LoadMapTexture(m_mapDimensions[0], m_mapDimensions[1]);

  // Calculate depth of each node on render list
  for (int i = 0 ; i < m_numberOfNodesOnRenderList ; i++) {

    // Get node vertices
    m_renderList[i] -> GetVertex(0, &v0);
    m_renderList[i] -> GetVertex(1, &v1);
    m_renderList[i] -> GetVertex(2, &v2);
    m_renderList[i] -> GetVertex(3, &v3);
    m_renderList[i] -> GetVertex(4, &v4);

    // Calculate center of each brick
    //untransformedCenter[0] = ((v3.GetX() - v0.GetX()) / 2.0) + v0.GetX();
    //untransformedCenter[1] = ((v4.GetY() - v0.GetY()) / 2.0) + v0.GetY();
    //untransformedCenter[2] = ((v1.GetZ() - v0.GetZ()) / 2.0) + v0.GetZ();
    untransformedCenter[0] = ((v1.GetX() - v0.GetX()) / 2.0) + v0.GetX();
    untransformedCenter[1] = ((v4.GetY() - v0.GetY()) / 2.0) + v0.GetY();
    untransformedCenter[2] = ((v2.GetZ() - v0.GetZ()) / 2.0) + v0.GetZ();

    // Calculate transformed center based on MV
    transformedCenter[0] = 
      mv[0] * untransformedCenter[0] +
      mv[4] * untransformedCenter[1] + 
      mv[8] * untransformedCenter[2] +
      mv[12];
    transformedCenter[1] = 
      mv[1] * untransformedCenter[0] +
      mv[5] * untransformedCenter[1] + 
      mv[9] * untransformedCenter[2] +
      mv[13];
    transformedCenter[2] = 
      mv[2] * untransformedCenter[0] +
      mv[6] * untransformedCenter[1] + 
      mv[10] * untransformedCenter[2];
    
    // Only use depth
    m_depthList[i] = transformedCenter[2];
    
  }

  // Sort back-to-front
  for (int i = 0 ; i < m_numberOfNodesOnRenderList ; i++) {
    for (int j = i+1 ; j < m_numberOfNodesOnRenderList ; j++) {
      if (m_depthList[i] > m_depthList[j]) {
        float tmpDepth = m_depthList[i];
        OctreeNode* tmpNode = m_renderList[i];
        m_depthList[i] = m_depthList[j];
        m_renderList[i] = m_renderList[j];
        m_depthList[j] = tmpDepth;
        m_renderList[j] = tmpNode;
      }
    }
  }

  // Message
  //Assert("Printing render list begin.");

  // Print render list
  //for (int i = 0 ; i < _numberOfNodesOnRenderList ; i++) {
  //  _renderList[i] -> Print();
  //}

  // Message
  //Assert("Printing render list end.");

  // Render nodes on render list
  for (int i = 0 ; i < m_numberOfNodesOnRenderList ; i++) {

    // Render node i
    RenderNode(m_renderList[i]);

    // Only allow a render to be aborted if it's not level 0
    if (m_renderList[i] -> GetLevel() > 0) {

      // Check for an abort render observer
      if (m_abortRenderObserver != NULL) {
        
        // Check if the render has been aborted
        if (m_abortRenderObserver -> Execute(0) == true) {
          
          // Execute swap buffers as many times as there are levels
          for (int i = called ; i < m_data -> GetNumberOfLevels() ; i++) {
            
            // Check is swap buffers observer exists
            if (m_swapBuffersObserver != NULL) {

              // Execute swap buffers observer
              m_swapBuffersObserver -> Execute(-1);
              
            }
            
            // Clear back buffer
            glClear(GL_COLOR_BUFFER_BIT);

            // Render axis
            if (m_axisFlag == true) {
              RenderAxis();
            }

            // Increment number of times swap buffers observer was executed
            called++;
          }

          // Message
          sprintf(message, "Render end. Level %d aborted.", preLevel);
          Assert(message);

          // Return false
          return false;
        }
        
      }
      
    }

  }

  // Render bounding box
  if (m_boundingBoxFlag == true) {
    RenderBoundingBox(ov0, ov1, ov2, ov3, ov4, ov5, ov6, ov7);
  }
  
  // Execute progress observer
  if (m_progressObserver != NULL) {
    
    // Execute progress observer
    m_progressObserver -> Execute(preLevel);
    
  }
  
  // Execute swap buffers observer
  if (m_swapBuffersObserver != NULL) {
    
    // Execute swap buffers observer
    m_swapBuffersObserver -> Execute(0);
    
    // Increment number of times swap buffers observer has been executed
    called++;
    
  }

  // Message
  sprintf(message, "Render level %d end.", preLevel);
  Assert(message);


  // Swap buffers to make number of calls to swap equal to number of levels
  for (int i = called ; i < m_data -> GetNumberOfLevels() ; i++) {

    // Execute swap buffers observer
    if (m_swapBuffersObserver != NULL) {
      m_swapBuffersObserver -> Execute(-1);
    }

    // Clear back buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Render axis
    if (m_axisFlag == true) {
      RenderAxis();
    }

    // Increment number of times swap buffers observer is executed
    called++;

  }

  // Message
  sprintf(message, "Render end. Completed.");
  Assert(message);


  // Return true
  return true;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::RenderAxis() {

  // Perform transforms
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(m_axisPosition[0], m_axisPosition[1], m_axisPosition[2]);
  glMultMatrixf(m_R);

  // Set line width
  glLineWidth(4.0);

  // Draw as GL_LINES
  glBegin(GL_LINES);

  // X-axis
  glColor4d(1.0, 1.0, 0.0, 1.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.5, 0.0, 0.0);

  // Y-axis
  glColor4d(0.0, 0.0, 1.0, 1.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 0.5, 0.0);

  // Z-axis
  glColor4d(1.0, 0.0, 0.0, 1.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.5);
  
  // Done
  glEnd();

  // Reset line width
  glLineWidth(1.0);

  // Pop transforms
  glPopMatrix();

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::RenderBoundingBox(OctreeNode* node) {

  // Bounding box vertices
  double volumeBoundingBoxVertex[8][3];

  // Get volume bouding box vertices
  Vertex v;
  node -> GetVertex(0, &v);
  v.GetXYZ(volumeBoundingBoxVertex[0]);
  node -> GetVertex(1, &v);
  v.GetXYZ(volumeBoundingBoxVertex[1]);
  node -> GetVertex(2, &v);
  v.GetXYZ(volumeBoundingBoxVertex[2]);
  node -> GetVertex(3, &v);
  v.GetXYZ(volumeBoundingBoxVertex[3]);
  node -> GetVertex(4, &v);
  v.GetXYZ(volumeBoundingBoxVertex[4]);
  node -> GetVertex(5, &v);
  v.GetXYZ(volumeBoundingBoxVertex[5]);
  node -> GetVertex(6, &v);
  v.GetXYZ(volumeBoundingBoxVertex[6]);
  node -> GetVertex(7, &v);
  v.GetXYZ(volumeBoundingBoxVertex[7]);

  // Set color
  glColor4d(1.0, 1.0, 1.0, 1.0);

  // Draw as GL_LINES
  glBegin(GL_LINES);

  // Bottom
  glVertex3dv(volumeBoundingBoxVertex[0]);
  glVertex3dv(volumeBoundingBoxVertex[1]);
  glVertex3dv(volumeBoundingBoxVertex[1]);
  glVertex3dv(volumeBoundingBoxVertex[3]);
  glVertex3dv(volumeBoundingBoxVertex[3]);
  glVertex3dv(volumeBoundingBoxVertex[2]);
  glVertex3dv(volumeBoundingBoxVertex[2]);
  glVertex3dv(volumeBoundingBoxVertex[0]);

  // Top
  glVertex3dv(volumeBoundingBoxVertex[4]);
  glVertex3dv(volumeBoundingBoxVertex[5]);
  glVertex3dv(volumeBoundingBoxVertex[5]);
  glVertex3dv(volumeBoundingBoxVertex[7]);
  glVertex3dv(volumeBoundingBoxVertex[7]);
  glVertex3dv(volumeBoundingBoxVertex[6]);
  glVertex3dv(volumeBoundingBoxVertex[6]);
  glVertex3dv(volumeBoundingBoxVertex[4]);

  // Connect bottom and top
  glVertex3dv(volumeBoundingBoxVertex[0]);
  glVertex3dv(volumeBoundingBoxVertex[4]);
  glVertex3dv(volumeBoundingBoxVertex[1]);
  glVertex3dv(volumeBoundingBoxVertex[5]);
  glVertex3dv(volumeBoundingBoxVertex[2]);
  glVertex3dv(volumeBoundingBoxVertex[6]);
  glVertex3dv(volumeBoundingBoxVertex[3]);
  glVertex3dv(volumeBoundingBoxVertex[7]);

  // Done
  glEnd();

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::RenderBoundingBox(Vertex v0, Vertex v1, Vertex v2, 
                                       Vertex v3, Vertex v4, Vertex v5, 
                                       Vertex v6, Vertex v7) {

  // Bounding box vertices
  double volumeBoundingBoxVertex[8][3];

  // Get volume bouding box vertices
  v0.GetXYZ(volumeBoundingBoxVertex[0]);
  v1.GetXYZ(volumeBoundingBoxVertex[1]);
  v2.GetXYZ(volumeBoundingBoxVertex[2]);
  v3.GetXYZ(volumeBoundingBoxVertex[3]);
  v4.GetXYZ(volumeBoundingBoxVertex[4]);
  v5.GetXYZ(volumeBoundingBoxVertex[5]);
  v6.GetXYZ(volumeBoundingBoxVertex[6]);
  v7.GetXYZ(volumeBoundingBoxVertex[7]);

  // Set color
  glColor4d(1.0, 1.0, 1.0, 1.0);

  // Perform transforms
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(m_T);
  glMultMatrixf(m_S);
  glMultMatrixf(m_R);

  // Draw as GL_LINES
  glBegin(GL_LINES);

  // Bottom
  glVertex3dv(volumeBoundingBoxVertex[0]);
  glVertex3dv(volumeBoundingBoxVertex[1]);
  glVertex3dv(volumeBoundingBoxVertex[1]);
  glVertex3dv(volumeBoundingBoxVertex[3]);
  glVertex3dv(volumeBoundingBoxVertex[3]);
  glVertex3dv(volumeBoundingBoxVertex[2]);
  glVertex3dv(volumeBoundingBoxVertex[2]);
  glVertex3dv(volumeBoundingBoxVertex[0]);

  // Top
  glVertex3dv(volumeBoundingBoxVertex[4]);
  glVertex3dv(volumeBoundingBoxVertex[5]);
  glVertex3dv(volumeBoundingBoxVertex[5]);
  glVertex3dv(volumeBoundingBoxVertex[7]);
  glVertex3dv(volumeBoundingBoxVertex[7]);
  glVertex3dv(volumeBoundingBoxVertex[6]);
  glVertex3dv(volumeBoundingBoxVertex[6]);
  glVertex3dv(volumeBoundingBoxVertex[4]);

  // Connect bottom and top
  glVertex3dv(volumeBoundingBoxVertex[0]);
  glVertex3dv(volumeBoundingBoxVertex[4]);
  glVertex3dv(volumeBoundingBoxVertex[1]);
  glVertex3dv(volumeBoundingBoxVertex[5]);
  glVertex3dv(volumeBoundingBoxVertex[2]);
  glVertex3dv(volumeBoundingBoxVertex[6]);
  glVertex3dv(volumeBoundingBoxVertex[3]);
  glVertex3dv(volumeBoundingBoxVertex[7]);

  // Done
  glEnd();

  // Pop transforms
  glPopMatrix();

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::RenderNode(OctreeNode* node) {

  // This method follows Algorthm 39-2. View-Aligned Slicing for Volume
  // Rendering found in Chapter 39 "Volume Rendering Techniques" by 
  // Milan Ikits, Joe Kniss, Aaron Lefohn, and Charles Hansen of GPU Gems.
  // Simian, http://www.cs.utah.edu/~jmk/simian/, and Vol-a-Tile are
  // referenced for implementation details.
  //                          
  //       6 +---------+ 7                                       
  //        /|        /|                      
  //       / |       / |
  //      /  |      /  |
  //   4 +---------+ 5 |
  //     |   |     |   |      y axis
  //     | 2 +-----+---+ 3    ^   
  //     |  /      |  /       |   
  //     | /       | /        | 
  //     |/        |/         |
  //   0 +---------+ 1        +-------> x axis 
  //                         /
  //                        /z axis

  // Message
  char message[1024];
  sprintf(message, "Render node %d begin.", node -> GetID());
  Assert(message);

  // Node vertices
  float x0, y0, z0;
  float x1, y1, z1;
  float x2, y2, z2;
  float x3, y3, z3;
  float x4, y4, z4;
  float x5, y5, z5;
  float x6, y6, z6;
  float x7, y7, z7;

  // Get vertices
  node -> GetVertex(0, &x0, &y0, &z0);
  node -> GetVertex(1, &x1, &y1, &z1);
  node -> GetVertex(2, &x2, &y2, &z2);
  node -> GetVertex(3, &x3, &y3, &z3);
  node -> GetVertex(4, &x4, &y4, &z4);
  node -> GetVertex(5, &x5, &y5, &z5);
  node -> GetVertex(6, &x6, &y6, &z6);
  node -> GetVertex(7, &x7, &y7, &z7);

  // Determine scale factor
  float sx = x3 - x0;
  float sy = y4 - y0;
  float sz = -(z0 - z1);

  // Determine translation factor
  float tx = (((x3 - x0) / 2) + x0);
  float ty = (((y4 - y0) / 2) + y0);
  float tz = (((z0 - z1) / 2) + z1);

  // Enable GL for textures
  EnableTextureGL();

  // Enable Cg
  EnableCg();

  // Print CacheRAM
  //_cacheRAM -> Print();
          
  // Make the node current in RAM cache
  m_cacheRAM -> MakeCurrent(node);

  // Print CacheRAM
  //_cacheRAM -> Print();

  // Print CacheVRAM
  //_cacheVRAM -> Print();

  // Make the node current in VRAM cache
  m_cacheVRAM -> MakeCurrent(node);

  // Print CacheVRAM
  //_cacheVRAM -> Print();

  // Bind texture
  glBindTexture(GL_TEXTURE_3D, node -> GetTexture());

  // Set viewport and frustum
  SetViewportAndFrustum();


  // Perform transforms
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(m_T);
  glMultMatrixf(m_S);
  glMultMatrixf(m_R);


  // 1. Transform the volume bounding box vertices into view coordinates
  // using the model-view matrix.

  // Volume bounding box vertices
  double volumeBoundingBoxVertex[8][3];

  // Volume view vertices
  double volumeViewVertex[8][3];

  // Get volume bouding box vertices
  Vertex v;
  /*
  node -> GetVertex(0, &v);
  v.GetXYZ(volumeBoundingBoxVertex[2]);
  node -> GetVertex(1, &v);
  v.GetXYZ(volumeBoundingBoxVertex[0]);
  node -> GetVertex(2, &v);
  v.GetXYZ(volumeBoundingBoxVertex[1]);
  node -> GetVertex(3, &v);
  v.GetXYZ(volumeBoundingBoxVertex[3]);
  node -> GetVertex(4, &v);
  v.GetXYZ(volumeBoundingBoxVertex[6]);
  node -> GetVertex(5, &v);
  v.GetXYZ(volumeBoundingBoxVertex[4]);
  node -> GetVertex(6, &v);
  v.GetXYZ(volumeBoundingBoxVertex[5]);
  node -> GetVertex(7, &v);
  v.GetXYZ(volumeBoundingBoxVertex[7]);
  */
  /*
  node -> GetVertex(0, &v);
  v.GetXYZ(volumeBoundingBoxVertex[0]);
  node -> GetVertex(1, &v);
  v.GetXYZ(volumeBoundingBoxVertex[2]);
  node -> GetVertex(2, &v);
  v.GetXYZ(volumeBoundingBoxVertex[3]);
  node -> GetVertex(3, &v);
  v.GetXYZ(volumeBoundingBoxVertex[1]);
  node -> GetVertex(4, &v);
  v.GetXYZ(volumeBoundingBoxVertex[4]);
  node -> GetVertex(5, &v);
  v.GetXYZ(volumeBoundingBoxVertex[6]);
  node -> GetVertex(6, &v);
  v.GetXYZ(volumeBoundingBoxVertex[7]);
  node -> GetVertex(7, &v);
  v.GetXYZ(volumeBoundingBoxVertex[5]);
  */
  node -> GetVertex(0, &v);
  v.GetXYZ(volumeBoundingBoxVertex[0]);
  node -> GetVertex(1, &v);
  v.GetXYZ(volumeBoundingBoxVertex[1]);
  node -> GetVertex(2, &v);
  v.GetXYZ(volumeBoundingBoxVertex[2]);
  node -> GetVertex(3, &v);
  v.GetXYZ(volumeBoundingBoxVertex[3]);
  node -> GetVertex(4, &v);
  v.GetXYZ(volumeBoundingBoxVertex[4]);
  node -> GetVertex(5, &v);
  v.GetXYZ(volumeBoundingBoxVertex[5]);
  node -> GetVertex(6, &v);
  v.GetXYZ(volumeBoundingBoxVertex[6]);
  node -> GetVertex(7, &v);
  v.GetXYZ(volumeBoundingBoxVertex[7]);

  // Get model-view matrix
  double mv[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mv);

  // Get inverse model-view matrix
  double mvinv[16];
  inverse4x4(mvinv, mv);

  // Transform bounding box vertices into view coordinates using the
  // model-view matrix
  for (int i = 0 ; i < 8 ; i++) {
    translate3V(volumeViewVertex[i], mv, volumeBoundingBoxVertex[i]);
  }



  // 2. Find the minimum and maximum z coordinates of the transformed vertices.
  // Compute the number of sampling planes used between these two values using
  // equidistant spacing from the view origin. The sampling distance is 
  // computed from the voxel size and current sampling rate.

  // Minimum and maximum z coordinates of transformed vertices
  double minZViewCoordinateValue = 10000.0;
  double maxZViewCoordinateValue = -10000.0;
  int minZViewCoordinateVertex = -1;
  int maxZViewCoordinateVertex = -1;

  // Find the minimum and maximum z coordinates of transformed vertices
  for (int i = 0 ; i < 8 ; i++) {

    // Find minimum
    if (minZViewCoordinateValue > minimum(minZViewCoordinateValue,
                                          volumeViewVertex[i][2])) {
      minZViewCoordinateValue = minimum(minZViewCoordinateValue, 
                                        volumeViewVertex[i][2]);
      minZViewCoordinateVertex = i;
    }
 
    // Find maximum
    if (maxZViewCoordinateValue < maximum(maxZViewCoordinateValue,
                                          volumeViewVertex[i][2])) {
      maxZViewCoordinateValue = maximum(maxZViewCoordinateValue, 
                                        volumeViewVertex[i][2]);
      maxZViewCoordinateVertex = i;
    }

  }

  // View plane normal
  double viewPlaneNormal[3] = {0.0, 0.0, 1.0};

  // Normalize view plane normal
  normalize3V(viewPlaneNormal);

  // Slice plane point - vertex farthest from the eye
  // Location where slice planes will start
  double slicePlanePoint[3] = 
    {volumeBoundingBoxVertex[minZViewCoordinateVertex][0],
     volumeBoundingBoxVertex[minZViewCoordinateVertex][1],
     volumeBoundingBoxVertex[minZViewCoordinateVertex][2]};

  // Slice plane normal - normalized VPN in model space
  double slicePlaneNormal[3];
  translate3V(slicePlaneNormal, mvinv, viewPlaneNormal);
  normalize3V(slicePlaneNormal);

  // Translate minimum and maximum z coordinates back to model space
  double minModelCoordinate[3];
  double maxModelCoordinate[3];
  double tmpMin[3] = {0.0, 0.0, minZViewCoordinateValue};
  double tmpMax[3] = {0.0, 0.0, maxZViewCoordinateValue};
  translate3V(minModelCoordinate, mvinv, tmpMin);
  translate3V(maxModelCoordinate, mvinv, tmpMax);

  // Determine distnace between min and max model space coordinates
  double minMaxDistance[3];
  sub3V(minMaxDistance, maxModelCoordinate, minModelCoordinate);

  // Slice distance - distance to slice across
  double sliceDistance = (double) sqrt(minMaxDistance[0] * minMaxDistance[0] +
                                       minMaxDistance[1] * minMaxDistance[1] +
                                       minMaxDistance[2] * minMaxDistance[2]);

  // Slice spacing - distance between slices
  double sliceSpacing = 
    sliceDistance / ((double) (node -> GetW()) * m_sliceFrequency);

  // Number of slices, i.e. sample planes
  int numberOfSlices = (int) (sliceDistance / sliceSpacing);



  // 3. For each plane in front-to-back or back-to-front order:
  for (int slice = 0 ; slice < numberOfSlices ; slice++) {

    // a. Test for intersections with the edges of the bounding box. Add each
    // intersection point to a temporary vertex list. Up to six intersections 
    // are generated, so the maximum size of the list is fixed.

    // Texture coordinates
    double textureCoordinates[8][3] = {{0.0, 0.0, 1.0},
                                       {1.0, 0.0, 1.0},
                                       {0.0, 0.0, 0.0},
                                       {1.0, 0.0, 0.0},
                                       {0.0, 1.0, 1.0},
                                       {1.0, 1.0, 1.0},
                                       {0.0, 1.0, 0.0},
                                       {1.0, 1.0, 0.0}};

    /*
    // Texture coordinates
    double textureCoordinates[8][3] = {{0.0, 0.0, 0.0},
                                       {1.0, 0.0, 0.0},
                                       {0.0, 0.0, 1.0},
                                       {1.0, 0.0, 1.0},
                                       {0.0, 1.0, 0.0},
                                       {1.0, 1.0, 0.0},
                                       {0.0, 1.0, 1.0},
                                       {1.0, 1.0, 1.0}};
    */

    // Temporary vertex lists
    double tmpUVert[6][3];  // untransformed edge intersections
    double tmpUText[6][3];  // untransformed texture intersections
    double tmpTVert[6][3];  // transformed edge intersections
    int numberOfEdges = 0;  // number of edges

    // Distance to increment the slice plane point by
    double d[3] = {slicePlaneNormal[0] * sliceSpacing,
                   slicePlaneNormal[1] * sliceSpacing,
                   slicePlaneNormal[2] * sliceSpacing};

    // Increment the slice plane point
    slicePlanePoint[0] += d[0];
    slicePlanePoint[1] += d[1];
    slicePlanePoint[2] += d[2];

    // Test for intersections with bounding box

    // Front bottom edge 0 1
    numberOfEdges += Intersect(volumeBoundingBoxVertex[0], 
                               volumeBoundingBoxVertex[1],
                               textureCoordinates[0], 
                               textureCoordinates[1],
                               volumeViewVertex[0], 
                               volumeViewVertex[1],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Front left edge 0 2
    numberOfEdges += Intersect(volumeBoundingBoxVertex[0], 
                               volumeBoundingBoxVertex[2],
                               textureCoordinates[0], 
                               textureCoordinates[2],
                               volumeViewVertex[0], 
                               volumeViewVertex[2],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);
 
    // Front right edge 1 3
    numberOfEdges += Intersect(volumeBoundingBoxVertex[1], 
                               volumeBoundingBoxVertex[3],
                               textureCoordinates[1], 
                               textureCoordinates[3],
                               volumeViewVertex[1], 
                               volumeViewVertex[3],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Left bottom edge 4 0
    numberOfEdges += Intersect(volumeBoundingBoxVertex[4], 
                               volumeBoundingBoxVertex[0],
                               textureCoordinates[4], 
                               textureCoordinates[0],
                               volumeViewVertex[4], 
                               volumeViewVertex[0],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Right bottom edge 1 5
    numberOfEdges += Intersect(volumeBoundingBoxVertex[1], 
                               volumeBoundingBoxVertex[5],
                               textureCoordinates[1], 
                               textureCoordinates[5],
                               volumeViewVertex[1], 
                               volumeViewVertex[5],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Front top edge 2 3
    numberOfEdges += Intersect(volumeBoundingBoxVertex[2], 
                               volumeBoundingBoxVertex[3],
                               textureCoordinates[2], 
                               textureCoordinates[3],
                               volumeViewVertex[2], 
                               volumeViewVertex[3],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Back bottom edge 4 5
    numberOfEdges += Intersect(volumeBoundingBoxVertex[4], 
                               volumeBoundingBoxVertex[5],
                               textureCoordinates[4], 
                               textureCoordinates[5],
                               volumeViewVertex[4], 
                               volumeViewVertex[5],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Back left edge 4 6
    numberOfEdges += Intersect(volumeBoundingBoxVertex[4], 
                               volumeBoundingBoxVertex[6],
                               textureCoordinates[4], 
                               textureCoordinates[6],
                               volumeViewVertex[4], 
                               volumeViewVertex[6],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Back right edge 5 7
    numberOfEdges += Intersect(volumeBoundingBoxVertex[5], 
                               volumeBoundingBoxVertex[7],
                               textureCoordinates[5], 
                               textureCoordinates[7],
                               volumeViewVertex[5],
                               volumeViewVertex[7],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Back top edge 6 7
    numberOfEdges += Intersect(volumeBoundingBoxVertex[6], 
                               volumeBoundingBoxVertex[7],
                               textureCoordinates[6], 
                               textureCoordinates[7],
                               volumeViewVertex[6], 
                               volumeViewVertex[7],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Left top edge 2 6
    numberOfEdges += Intersect(volumeBoundingBoxVertex[2],
                               volumeBoundingBoxVertex[6],
                               textureCoordinates[2], 
                               textureCoordinates[6],
                               volumeViewVertex[2], 
                               volumeViewVertex[6],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);

    // Right top edge 3 7
    numberOfEdges += Intersect(volumeBoundingBoxVertex[3], 
                               volumeBoundingBoxVertex[7],
                               textureCoordinates[3], 
                               textureCoordinates[7],
                               volumeViewVertex[3], 
                               volumeViewVertex[7],
                               slicePlanePoint, 
                               slicePlaneNormal,
                               tmpUVert[numberOfEdges], 
                               tmpUText[numberOfEdges], 
                               tmpTVert[numberOfEdges]);


    // b. Compute the center of the proxy polygon by averaging the intersection
    // points. Sort the polygon vertices clockwise or counterclockwise by
    // projecting them onto the x-y plane and computing their angle around the
    // center, with the first vertex or the x axis as the reference. Note that 
    // to avoid trigonometric computations, the tangent of the angle and the 
    // sign of the coordinates, combined into a single scalar value called the
    // pseudo-angle, can be used for sorting the vertices (Moret and 
    // Shapiro 1991).

    // Note: This is right out of Simian.

    int i, j, k;
    float dx, dy, tt, theta, cen[2];
    cen[0] = cen[1] = 0.0;
    int next;
    int order[6] = {0, 1, 2, 3, 4, 5};

    for (j = 0 ; j < numberOfEdges ; j++) {
      cen[0] += tmpTVert[j][0];
      cen[1] += tmpTVert[j][1];
    }
    cen[0] /= numberOfEdges;
    cen[1] /= numberOfEdges;

    for (j = 0 ; j < numberOfEdges ; j++) {
    
      theta = -10;
      next = j;
      for (k = j ; k < numberOfEdges ; k++) {

        dx = tmpTVert[order[k]][0] - cen[0];
        dy = tmpTVert[order[k]][1] - cen[1];
        if ((dx == 0) && (dy == 0)) {
          next = k;
          break;
        }

        tt = dy / (fabs(dx) + fabs(dy));
        if (dx < 0.0) tt = (float) (2.0 - tt);
        else if (dy < 0.0) tt = (float) (4.0 + tt);
        if (theta <= tt) {
          next = k;
          theta = tt;
        }

      }

      int tmp = order[j];
      order[j] = order[next];
      order[next] = tmp;

    }


    // c. Tessellate the proxy polygon into triangles and add the resulting
    // vertices to the output vertex array. The slice polygon can be 
    // tessellated into a triangle strip or triangle fan using the center. 
    // Depending on the rendering algorithm, the vertices may need to be 
    // transformed back to object space during this step.

    // Note: Tessellation into triangles is not implemented. Each slice is
    // rendered as a GL_POLYGON.


    // Render a slice
    RenderSlice(numberOfEdges, tmpUText, tmpUVert, order);

  }

  // Disable GL for textures
  DisableTextureGL();

  // Disable Cg
  DisableCg();


  // Render brick box
  if (m_brickBoxFlag == true) {
    RenderBoundingBox(node);
  }


  // Pop all transformations
  glPopMatrix();
  
  // Message
  sprintf(message, "Render node %d end.", node -> GetID());
  Assert(message);

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::RenderSlice(int numberOfEdges, double t[6][3], 
                                 double v[6][3], int order[6]) {

  // Render a slice as a polygon
  glBegin(GL_POLYGON);
  for (int i = 0 ; i < numberOfEdges ; i++) {
    glTexCoord3dv(t[order[i]]);
    glVertex3dv(v[order[i]]);
  }
  glEnd();

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::ScaleAlpha(double newSliceFrequency, 
                                double originalSliceFrequency,
                                unsigned char* newTexture, 
                                unsigned char* originalTexture,
                                int width, int height) {

  // Go through each entry
  for (int i = 0 ; i < height ; i++) {
    for (int j = 0 ; j < width ; j++) {

      // Copy color components
      newTexture[(i * width * 4) + (j * 4) + 0] = 
        originalTexture[(i * width * 4) + (j * 4) + 0];
      newTexture[(i * width * 4) + (j * 4) + 1] = 
        originalTexture[(i * width * 4) + (j * 4) + 1];
      newTexture[(i * width * 4) + (j * 4) + 2] = 
        originalTexture[(i * width * 4) + (j * 4) + 2];

      /*
      double originalR =
        (double) originalTexture[(i * width * 4) + (j * 4) + 0] / 255.0;
      double originalG =
        (double) originalTexture[(i * width * 4) + (j * 4) + 1] / 255.0;
      double originalB =
        (double) originalTexture[(i * width * 4) + (j * 4) + 2] / 255.0;
      */

      /*
      double newR = (1.0 - pow(1.0 - originalR, 
                               originalSliceFrequency / newSliceFrequency));
      double newG = (1.0 - pow(1.0 - originalG, 
                               originalSliceFrequency / newSliceFrequency));
      double newB = (1.0 - pow(1.0 - originalB, 
                               originalSliceFrequency / newSliceFrequency));
      double newR = originalR * (originalSliceFrequency / newSliceFrequency);
      double newG = originalG * (originalSliceFrequency / newSliceFrequency);
      double newB = originalB * (originalSliceFrequency / newSliceFrequency);
      newTexture[(i * width * 4) + (j * 4) + 0] = 
        (unsigned char) (newR * 255.0);
      newTexture[(i * width * 4) + (j * 4) + 1] = 
        (unsigned char) (newG * 255.0);
      newTexture[(i * width * 4) + (j * 4) + 2] = 
        (unsigned char) (newB * 255.0);
      */
           
      // Get original alpha and normalize it
      double originalA = 
        (double) originalTexture[(i * width * 4) + (j * 4) + 3] / 255.0;
      
      // Calculate new alpha
      //double newA = originalA*newSliceFrequency;

      // Calculate new alpha
      double newA = (1.0 - pow(1.0 - originalA, 
                               originalSliceFrequency / newSliceFrequency));

      // Assign new alpha
      newTexture[(i * width * 4) + (j * 4) + 3] = 
        (unsigned char) (newA * 255.0);

    }
  }

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetAbortRenderObserver(VolumeRendererCommand* observer) {

  // Set observer
  m_abortRenderObserver = observer;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetAxisOff() {

  // Set flag
  m_axisFlag = false;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetAxisOn() {

  // Set flag
  m_axisFlag = true;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetAxisPosition(float x, float y, float z) {

  // Set position
  m_axisPosition[0] = x;
  m_axisPosition[1] = y;
  m_axisPosition[2] = z;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetBoundingBoxOff() {

  // Set flag
  m_boundingBoxFlag = false;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetBoundingBoxOn() {

  // Set flag
  m_boundingBoxFlag = true;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetBrickBoxOff() {

  // Set flag
  m_brickBoxFlag = false;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetBrickBoxOn() {

  // Set flag
  m_brickBoxFlag = true;

}

/*--------------------------------------------------------------------------*/

bool VolumeRenderer::SetData(Octree* data) {

  // Set data with default cache sizes
  return SetData(data, 50, 10);

}

/*--------------------------------------------------------------------------*/

bool VolumeRenderer::SetData(Octree* data, int ramSize, int vramSize) {

  // Message
  Assert("SetData begin.");

  // Delete old RAM cache
  if (m_cacheRAM != NULL) {
    delete m_cacheRAM;
  }

  // Delete old VRAM cache
  if (m_cacheVRAM != NULL) {
    delete m_cacheVRAM;
  }

  // Delete old queue
  if (m_queue != NULL) {
    delete m_queue;
  }

  // Delete old traversal queue
  if (m_queueTraversal != NULL) {
    delete m_queueTraversal;
  }

  // Delete old visible node queue
  if (m_queueVisible != NULL) {
    delete m_queueVisible;
  }

  // Delete old data
  if (m_data != NULL) {
    delete m_data;
  }

  // Delete old render list
  if (m_renderList != NULL) {
    delete m_renderList;
  }

  // Delete old depth list
  if (m_depthList != NULL) {
    delete m_depthList;
  }

  // Set new data
  if (data != NULL) {
    m_data = data;
  }
  else {
    return false;
  }

  // Stop current log
  Log::Stop();

  // Start new log
  char sufix[256];
  memset(sufix, 0, 256);
  sprintf(sufix, 
          "%d-%d", vramSize, ramSize);
  Log::Start(sufix);

  // Allocate new CacheRAM
  if ((m_cacheRAM = new CacheRAM(ramSize)) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Allocate new CacheVRAM
  if ((m_cacheVRAM = new CacheVRAM(vramSize)) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Allocate new queue
  if ((m_queue = new OctreeNodeQueue(m_data -> GetNumberOfNodes())) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Allocate new traversal queue
  if ((m_queueTraversal = 
       new OctreeNodeQueue(m_data -> GetNumberOfNodes())) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Allocate new visible node queue
  if ((m_queueVisible = 
       new OctreeNodeQueue(m_data -> GetNumberOfNodes())) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Allocate new render list
  if ((m_renderList = new OctreeNode*[m_data -> GetNumberOfNodes()]) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Allocate new depth list
  if ((m_depthList = new float[m_data -> GetNumberOfNodes()]) == NULL) {
    fprintf(stderr, "VolumeRenderer: Memory allocation error.\n");
    return false;
  }

  // Message
  Assert("SetData end.");

  // Return true
  return true;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetFrustum(float left, float right, float bottom,
                                float top, float near, float far) {

  // Message
  Assert("SetFrustum begin.");

  // Check if user called Init
  IsInitialized();

  // Set frustum
  m_frustum[0] = left;
  m_frustum[1] = right;
  m_frustum[2] = bottom;
  m_frustum[3] = top;
  m_frustum[4] = near;
  m_frustum[5] = far;

  // Message
  Assert("SetFrustum end.");
  
}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetFrustum(float frustum[6]) {

  // Message
  Assert("SetFrustum begin.");

  // Check if user called Init
  IsInitialized();

  // Set frustum
  m_frustum[0] = frustum[0];
  m_frustum[1] = frustum[1];
  m_frustum[2] = frustum[2];
  m_frustum[3] = frustum[3];
  m_frustum[4] = frustum[4];
  m_frustum[5] = frustum[5];

  // Message
  Assert("SetFrustum end.");

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetMap(unsigned char* map) {

  // Message
  Assert("SetMap begin.");

  // Check if user called Init
  IsInitialized();
  
  // Set map
  SetMapUnsigned8Int(map);

  // Message
  Assert("SetMap end.");
  
}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetMapUnsigned8Int(unsigned char* map) {

  // Delete original map
  if (m_originalMap != NULL) {

    // Delete map
    delete [] m_originalMap;

  }

  // Delete map
  if (m_map != NULL) {

    // Unload texture
    UnloadMapTexture();

    // Delete map
    delete [] m_map;

  }

  // Set original map
  m_originalMap = map;

  // Set map dimensions
  m_mapDimensions[0] = 256;
  m_mapDimensions[1] = 1;

  // Allocate memory for a new map
  m_map = new unsigned char[m_mapDimensions[0] * m_mapDimensions[1] * 4];

  // Scale alpha values according to the slice frequency
  ScaleAlpha(m_sliceFrequency, m_originalSliceFrequency, m_map, m_originalMap, 
             m_mapDimensions[0], m_mapDimensions[1]);

  // Load map texture
  LoadMapTexture(m_mapDimensions[0], m_mapDimensions[1]);

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetMapUnsigned16Int(unsigned char* map) {

  // Delete original map
  if (m_originalMap != NULL) {

    // Delete map
    delete [] m_originalMap;

  }

  // Delete map
  if (m_map != NULL) {

    // Unload texture
    UnloadMapTexture();

    // Delete map
    delete [] m_map;

  }

  // Set original map
  m_originalMap = map;

  // Set map dimensions
  m_mapDimensions[0] = 256;
  m_mapDimensions[1] = 256;

  // Allocate memory for a new map
  m_map = new unsigned char[m_mapDimensions[0] * m_mapDimensions[1] * 4];

  // Scale alpha values according to the slice frequency
  ScaleAlpha(m_sliceFrequency, m_originalSliceFrequency, m_map, m_originalMap, 
             m_mapDimensions[0], m_mapDimensions[1]);

  // Load map texture
  LoadMapTexture(m_mapDimensions[0], m_mapDimensions[1]);

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetProgressObserver(VolumeRendererCommand* observer){

  // Set observer
  m_progressObserver = observer;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetR(float m[16]) {

  // Message
  Assert("SetRotationMatrix begin.");
  
  // Check if user called Init
  IsInitialized();
  
  // Set rotation matrix
  m_R[0] = m[0]; m_R[4] = m[4]; m_R[8] = m[8]; m_R[12] = m[12];
  m_R[1] = m[1]; m_R[5] = m[5]; m_R[9] = m[9]; m_R[13] = m[13];
  m_R[2] = m[2]; m_R[6] = m[6]; m_R[10] = m[10]; m_R[14] = m[14];
  m_R[3] = m[3]; m_R[7] = m[7]; m_R[11] = m[11]; m_R[15] = m[15];
  
  // Message
  Assert("SetRotationMatrix end.");

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetS(float m[16]) {
  
  // Message
  Assert("SetScaleMatrix begin.");

  // Check if user called Init
  IsInitialized();

  // Set scale matrix
  m_S[0] = m[0]; m_S[4] = m[4]; m_S[8] = m[8]; m_S[12] = m[12];
  m_S[1] = m[1]; m_S[5] = m[5]; m_S[9] = m[9]; m_S[13] = m[13];
  m_S[2] = m[2]; m_S[6] = m[6]; m_S[10] = m[10]; m_S[14] = m[14];
  m_S[3] = m[3]; m_S[7] = m[7]; m_S[11] = m[11]; m_S[15] = m[15];

  // Message
  Assert("SetScaleMatrix end.");
  
}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetSliceFrequency(double frequency) {

  // Set slice frequency
  m_sliceFrequency = frequency;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetSwapBuffersObserver(VolumeRendererCommand* observer){

  // Set observer
  m_swapBuffersObserver = observer;

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetT(float m[16]) {

  // Message
  Assert("SetTranslationMatrix begin.");

  // Check if user called Init
  IsInitialized();
  
  // Set translation matrix
  m_T[0] = m[0]; m_T[4] = m[4]; m_T[8] = m[8]; m_T[12] = m[12];
  m_T[1] = m[1]; m_T[5] = m[5]; m_T[9] = m[9]; m_T[13] = m[13];
  m_T[2] = m[2]; m_T[6] = m[6]; m_T[10] = m[10]; m_T[14] = m[14];
  m_T[3] = m[3]; m_T[7] = m[7]; m_T[11] = m[11]; m_T[15] = m[15];

  // Message
  Assert("SetTranslationMatrix end.");
  
}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::SetViewport(int x, int y, int w, int h) {
  
  // Message
  Assert("SetViewport begin.");

  // Check if user called Init
  IsInitialized();
  
  // Set viewport
  m_viewport[0] = x;
  m_viewport[1] = y;
  m_viewport[2] = w;
  m_viewport[3] = h;

  // Message
  Assert("SetViewport end.");
  
}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::SetViewportAndFrustum() {

  // Set viewport
  glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

  // Switch to projection mode
  glMatrixMode(GL_PROJECTION);

  // Set frustum
  glLoadIdentity();
  glOrtho(m_frustum[0], m_frustum[1], m_frustum[2],
          m_frustum[3], m_frustum[4], m_frustum[5]);

  // Switch to modelview mode
  glMatrixMode(GL_MODELVIEW);

}

/*--------------------------------------------------------------------------*/

void VolumeRenderer::UnInitCg() {

  // Destroy fragment programs
  cgDestroyProgram(m_cgFProgram);

  // Destroy context
  cgDestroyContext(m_cgContext);

  // Set initialized flag
  m_isInitialized = false;

}

/*---------------------------------------------------------------------------*/

void VolumeRenderer::UnloadMapTexture() {

  // Delete map texture
  glDeleteTextures(1, &m_mapTexture);

  // Set flag
  m_isMapLoaded = false;

}

/*---------------------------------------------------------------------------*/
