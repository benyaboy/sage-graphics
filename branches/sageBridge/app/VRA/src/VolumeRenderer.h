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

#ifndef VOLUME_RENDERER_H
#define VOLUME_RENDERER_H

/*--------------------------------------------------------------------------*/

#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <GL/gl.h>

#include "CacheRAM.h"
#include "CacheVRAM.h"
#include "FrustumPlanes.h"
#include "Log.h"
#include "Matrix.h"
#include "Octree.h"
#include "OctreeNodeQueue.h"
#include "Vertex.h"
#include "VolumeRendererCommand.h"

/*--------------------------------------------------------------------------*/

extern "C" {
  extern void glTexImage3D(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei,
                           GLint, GLenum, GLenum, const GLvoid*);
}

/*--------------------------------------------------------------------------*/

class VolumeRenderer {
  
public:
  
  // Default constructor
  VolumeRenderer();
  
  // Default destructor
  ~VolumeRenderer();
  
  // Get data
  Octree* GetData();

  // Get frustum
  void GetFrustum(float* left, float* right, float* bottom,
                  float* top, float* near, float* far);

  // Get frustum
  void GetFrustum(float frustum[6]);

  // Get color and opacity map
  unsigned char* GetMap();

  // Get rotation matrix
  void GetR(double m[16]);

  // Get scale matrix
  void GetS(double m[16]);

  // Get slice frequency/sample frequency
  double GetSliceFrequency();

  // Get translation matrix
  void GetT(double m[16]);

  // Get viewport
  void GetViewport(int* x, int* y, int* w, int* h);

  // Get viewport
  void GetViewport(int viewport[4]);

  // Initialize - Call this after the OpenGL rendering context has been created
  bool Init();
  
  // User must call Init before any other method
  bool IsInitialized();
  
  // Render
  bool Render();

  // Render axis
  void RenderAxis();

  // Render bounding box
  void RenderBoundingBox(OctreeNode* node);

  // Render bounding box
  void RenderBoundingBox(Vertex v0, Vertex v1, Vertex v2, Vertex v3,
                         Vertex v4, Vertex v5, Vertex v6, Vertex v7);

  // Scale alpha values based on slice frequency
  // newA = 
  //       1.0 - ((1.0 - originalA) ^ originalSliceFrequency/newSliceFrequency)
  void ScaleAlpha(double newSliceFrequency, double originalSliceFrequency,
                  unsigned char* newTexture, unsigned char* originalTexture,
                  int width, int height);

  // Set abort render observer
  void SetAbortRenderObserver(VolumeRendererCommand* observer);

  // Set axis off
  void SetAxisOff();

  // Set axis on
  void SetAxisOn();

  // Set axis position
  void SetAxisPosition(float x, float y, float z);

  // Turn bounding box off
  void SetBoundingBoxOff();

  // Turn bounding box on
  void SetBoundingBoxOn();

  // Turn bounding box off
  void SetBrickBoxOff();

  // Turn bounding box on
  void SetBrickBoxOn();

  // Set data - Returns true if data is loaded, false otherwise. The data 
  // should be allocated on the heap using the new operator. This class will 
  // attempt to delete the data with the delete operator when new data is set 
  // or when the destructor is called.
  bool SetData(Octree* data);

  // Set data - Returns true if data is loaded, false otherwise. The data 
  // should be allocated on the heap using the new operator. This class will 
  // attempt to delete the data with the delete operator when new data is set 
  // or when the destructor is called.
  bool SetData(Octree* data, int ramSize, int vramSize);
  
  // Set frustum
  void SetFrustum(float left, float right, float bottom,
                  float top, float near, float far);
  
  // Set frustum
  void SetFrustum(float frustum[6]);

  // Set 8-bit color and opacity map - This map should be allocated on the heap
  // using the new operator. This class will attempt to delete the map with the
  // delete operator when a new map is set, when the slice frequency is changed
  // or when the destructor is called.
  void SetMap(unsigned char* map);

  // Set 8-bit color and opacity map - This map should be allocated on the heap
  // using the new operator. This class will attempt to delete the map with the
  // delete operator when a new map is set, when the slice frequency is changed
  // or when the destructor is called.
  void SetMapUnsigned8Int(unsigned char* map);

  // Set 16-bit color and opacity map - This map should be allocated on the 
  // heap using the new operator. This class will attempt to delete the map 
  // with the delete operator when a new map is set, when the slice frequency 
  // is changed and when the destructor is called.
  void SetMapUnsigned16Int(unsigned char* map);

  // Set progress observer
  void SetProgressObserver(VolumeRendererCommand* observer);

  // Set rotation matrix
  void SetR(double m[16]);

  // Set rotation matrix
  void SetR(float m[16]);

  // Set scale matrix
  void SetS(double m[16]);

  // Set scale matrix
  void SetS(float m[16]);

  // Set slice frquency/sample frequency
  void SetSliceFrequency(double frequency);

  // Set swap buffers observer
  void SetSwapBuffersObserver(VolumeRendererCommand* observer);

  // Set translation matrix
  void SetT(double m[16]);

  // Set translation matrix
  void SetT(float m[16]);
  
  // Set viewport
  void SetViewport(int x, int y, int w, int h);

  // Set viewport
  void SetViewport(int viewport[4]);
  
private:

  // Print a message to stdout
  void Assert(const char* message);

  // Build queue of visible nodes. Uses a BFS and two queues to construct a
  // queue of all visible nodes level by level.
  void BuildVisibleNodeQueue();
  
  // Disable Cg
  void DisableCg();

  // Disable texture state in OpenGL
  void DisableTextureGL();

  // Enable Cg
  void EnableCg();

  // Enable texture state in OpenGL
  void EnableTextureGL();

  // Initialize Cg
  bool InitCg();

  // Compute intersection with bounding box - Returns one if there's an
  // intersection and computes new values, returns zero otherwise.
  int Intersect(double p0[3], double p1[3],       // bounding box coordinates
                double t0[3], double t1[3],       // texture coordinates
                double v0[3], double v1[3],       // view coordinates
                double sp[3], double sn[3],       // slice plane point & normal
                double pnew[3], double tnew[3], double vnew[3]);  // new values

  // Load map texture
  void LoadMapTexture(int w, int h);

  // Render node
  void RenderNode(OctreeNode* node);

  // Render a single slice given the number of edges, texture coordinates,
  // vertices, and the vertex order.
  void RenderSlice(int numberOfEdges, double t[6][3], 
                   double v[6][3], int order[6]);
  
  // Set the viewport and frustum
  void SetViewportAndFrustum();

  // Uninitialize Cg
  void UnInitCg();

  // Unload data texture
  void UnloadDataTexture();

  // Unload map texture
  void UnloadMapTexture();

  // Abort render observer
  VolumeRendererCommand* m_abortRenderObserver;

  // Render axis flag
  bool m_axisFlag;

  // Axis position
  float m_axisPosition[3];

  // Render bounding box flag
  bool m_boundingBoxFlag;

  // Render brick box flag
  bool m_brickBoxFlag;

  // RAM cache
  CacheRAM* m_cacheRAM;

  // VRAM cache
  CacheVRAM* m_cacheVRAM;

  // Cg context
  CGcontext m_cgContext;

  // Cg data texture
  CGparameter m_cgFDataTexture;

  // Cg map texture
  CGparameter m_cgFMapTexture;

  // Cg fragment profile
  CGprofile m_cgFProfile;

  // Cg fragment program
  CGprogram m_cgFProgram;

  // Data
  Octree* m_data;

  // Depth list: used by Render()
  float* m_depthList;
  
  // Frustum and viewport
  float m_frustum[6];
  
  // Frustum planes utility class
  FrustumPlanes m_frustumPlanes;
  
  // User must call Init before any other method
  bool m_isInitialized;

  // Map loaded flag
  bool m_isMapLoaded;

  // Color and opacity map
  unsigned char* m_map;

  // Color and opacity map dimensions
  int m_mapDimensions[2];

  // Texture for map
  GLuint m_mapTexture;

  // Number of nodes on render list
  int m_numberOfNodesOnRenderList;

  // Original color and opacity map
  unsigned char* m_originalMap;

  // Original slice frequency
  double m_originalSliceFrequency;

  // Progress observer
  VolumeRendererCommand* m_progressObserver;

  // Queue
  OctreeNodeQueue* m_queue;

  // Traversal queue
  OctreeNodeQueue* m_queueTraversal;

  // Visible node queue
  OctreeNodeQueue* m_queueVisible;
  
  // Rotation matrix
  float m_R[16];

  // Render list: used by Render()
  OctreeNode** m_renderList;

  // Scale matrix
  float m_S[16];

  // Slice frequency
  double m_sliceFrequency;

  // Swap buffer observer
  VolumeRendererCommand* m_swapBuffersObserver;

  // Translation matrix
  float m_T[16];
  
  // Viewport
  int m_viewport[4];

};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
