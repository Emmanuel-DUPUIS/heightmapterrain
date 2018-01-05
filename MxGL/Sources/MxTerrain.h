//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include <gl/glew.h>
#include <vmath.h>

#include "MxSceneObject.h"
#include "UxVertexArray.h"
#include "UxAtomicCounter.h"

class UxProgram;


//========================================================================
//  Height Map Terrain Model:
//    Manages an instance representation throug a set of subidvided
//    patches. Tunning/debugging capabilities. Animations capabilities.
//========================================================================

class MxTerrain: public MxSceneObject
{
public:
  
  // Height map uniform block structure
  struct u_HeightMap
  {
    GLuint64  heightTextureHandle;      // Bindless texture (assumption: only first component, r, considered for height valuation). Rem: height could be also coded with rb or rbga combination 
    Vector2f  terrainDimension;         // Size (WC) of the terrain
    Vector2i  terrainSubdivision;       // Number of unitary patches in x and y directions
    float     heightFactor;             // Factor applied to heigth texture
    float     distortionFactor;         // Factor applied to patch height distortion to determine the subdivision level
    float     maxSubdivison;            // Max number of subdivision of each side of patch (tesselation process)
    float     maxPixelSubdivisionRatio; // Subdivision factor based on edge dimension on screen
    GLuint64  heightColorMapHandle;     // Bindless texture for color map (color=fct(height))
    Vector2f  heightColorMapBounds;     // Parameters to apply color map
    uint32_t  colorMode;                // Color mode (luminance, color map)
    float     isolineStep;              // Height between isolines
    uint32_t  wireframeMode;            // Optional wireframe border and normals display (0: none, 1: patch borders, 2: patch and tesselated triangles borders, 3: triangle and normals computed from map, 4: 2+3)
    float     functionalMode;           // Polynomial function to replace the map height texture to allow neutralize bugs in height and associated normalcomputation
    uint32_t  smoothMode;               // Mode of interpolation (Linear+Constant, Linear+Linear or Bicubic)
    uint32_t  shadowMode;               // Shadow mode, alternate method to shadow mapping, unsatisfactory 
    float     minHeight;                // Trim height trough minimum value
    float     maxHeight;                // Trim height trough maximum value
    Vector2f  _alignment;
  };

private:

  // Model positionning uniform block structure
  struct u_Positionning
  {
    Matrix4f  model;
  };

  // Vertex data structure for terrain draw
  struct TerrainVertexData
  {
    float position[4];
    float uv[2];
    float color[4];
  };

  // Vertex data structure for map draw
  struct MapVertexData
  {
    float   position[2];
    int32_t uv[2];
  };

private:

  // Static initialisations (programs, shaders, counters...)
  static void Startup();
  static bool               _Startup;
  static UxProgram*         _TriangleDraw;
  static UxProgram*         _WireframeDraw;
  static UxProgram*         _PointMapDraw;
  static UxProgram*         _WireframeMapDraw;
  static UxAtomicCounter*   _TriangleCounter;
  static UxAtomicCounter*   _DiscardedTriangleCounter;

protected:

  // Parameters defining the instance geometry
  Vector2f     _TerrainDimension;         // Size (WC) of the terrain
  Vector2i     _TerrainSubdivision;       // Number of unitary patches in x and y directions
  float        _HeightFactor;             // Factor applied to heigth texture
  float        _DistortionFactor;         // Factor of subdivions based upon height distortion within patch
  float        _MaxSubdivison;            // Max number of subdivision of each side of a patch (tesselation process)
  float        _MaxPixelSubdivisionRatio; // Ratio to determine tesselation factor according number of pixel on the viewport
  float        _FunctionalMode;

  // Parameters defining the instance representation (corresponding to u_HeightMap uniform structure)
  std::string  _HeightMapTexturePath;
  std::string  _HeightColorMapTexturePath;
  uint32_t     _ColorMode;
  Vector2f     _HeightColorMapBounds;
  float        _IsolineStep;
  uint32_t     _SmoothMode;
  uint32_t     _ShadowMode;
  float        _MinHeight;
  float        _MaxHeight;

  // Parameters for additionnal representations
  uint32_t     _WireframeMode;             // Display patch and triangle borders and normals
  uint32_t     _MapMode;                   // Optional map display (0: none, 1: points for each pixel with corresponding height, 2: wireframe grid)

  // Textures data
  GLuint       _HeightMapTextureName;
  GLuint64     _HeightTextureHandle;
  GLuint       _HeightColorMapTextureName;
  GLuint64     _HeightColorMapHandle;
  
  // Terrain data
  int32_t                           _Width;
  int32_t                           _Height;
  std::vector<TerrainVertexData>    _Vertices;
  uint32_t*                         _Indices;

  // Data sent to Vertex shader for patch draw (triangles and wireframe)
  UxVertexArray<TerrainVertexData>  _PatchVertexArray;
  UxIndexBuffer                     _PatchIndexBuffer;

  // Data sent to Vertex Shader for map draw (points and wireframe)
  UxVertexArray<MapVertexData>      _MapVertexArray; 
  UxIndexBuffer                     _PointMapIndexBuffer;
  UxIndexBuffer                     _WireframeMapIndexBuffer;

public:

  MxTerrain();
  ~MxTerrain();
  __DeclareDeletedCtorsAndAssignments(MxTerrain)

  float getHeightFactor() const { return _HeightFactor; }

  void init(const Vector2f& iTerrainDimension, const Vector2i& iTerrainSubdivision, float iHeightFactor, float iMaxSubdivison, float iMaxPixelSubdivisionRatio, const std::string& iHeightMapTexturePath, const std::string& iHeightColorMapTexturePath, const Vector2f& iHeightColorMapBounds);

  void setFunctionalMode(float iFunctionalMode) { _FunctionalMode = iFunctionalMode;  }
  void setColorMode(uint32_t iColorMode) { __AssertIfNot(iColorMode >=0 && iColorMode <= 2, "Invalid Color Mode");  _ColorMode = iColorMode; }
  void setHeightColorMapBounds(Vector2f iHeightColorMapBounds) { __AssertIfNot(iHeightColorMapBounds[0] >= 0 && iHeightColorMapBounds[0] < iHeightColorMapBounds[1], "Invalid Color Map Bounds");  _HeightColorMapBounds = iHeightColorMapBounds; }
  void setIsolineStep(float iIsolineStep) { __AssertIfNot(iIsolineStep >= 0.0f, "Invalid Isoline step");  _IsolineStep = iIsolineStep; }
  void setSmoothMode(uint32_t iSmoothMode) { __AssertIfNot(iSmoothMode >= 0 && iSmoothMode <=3, "Invalid Smooth Mode"); _SmoothMode = iSmoothMode; }
  void setWireframeMode(uint32_t iWireframeMode) { __AssertIfNot(iWireframeMode >= 0 && iWireframeMode <= 3, "Invalid Wireframe Mode"); _WireframeMode = iWireframeMode; }
  void setMapMode(uint32_t iMapMode) { __AssertIfNot(iMapMode >= 0 && iMapMode <= 2, "Invalid Map Mode"); _MapMode = iMapMode; }
  void setShadowMode(uint32_t iShadowMode) { __AssertIfNot(iShadowMode >= 0 && iShadowMode <= 4, "Invalid Shadow Mode"); _ShadowMode = iShadowMode; }
  void setMinHeight(float iMinHeight) { _MinHeight = iMinHeight; }
  void setMaxHeight(float iMaxHeight) { _MaxHeight = iMaxHeight; }
  void setDistortionFactor(float iDistortionFactor) { _DistortionFactor = iDistortionFactor; }

protected:

  void sendData(const Matrix4f& iModelMatrix, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle);
  void render(int iTime, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle, uint32_t& oPatchNb, uint32_t& oDrawnPatchNb, uint32_t& oTriangleNb, uint32_t& oDiscardedTriangleNb);

  void generateTerrainData();
  void generateMapData();
};