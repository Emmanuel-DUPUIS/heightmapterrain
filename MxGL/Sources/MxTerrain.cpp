//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxTerrain.h"
#include "MxHeightComputation.h"
#include "MxScene.h"
#include "MxGLObjects.h"
#include "UxProgram.h"
#include "UxUniformBlockBase.h"
#include "UxUniformBlockDataAccessor.h"
#include "UxUtils.h"
#include "UxReport.h"

// Declare the "vertex" report to dump GPU data in a CPU debugging session
#define __UxReportPath ../MxGL
#define __UxReportName vertex
#define __UxReportSize 64
#define __UxReportInstance reportVertex
#include "UxIncludeReport.h"

// Startup management

UxProgram* MxTerrain::_TriangleDraw     = nullptr;
UxProgram* MxTerrain::_WireframeDraw    = nullptr;
UxProgram* MxTerrain::_PointMapDraw     = nullptr;
UxProgram* MxTerrain::_WireframeMapDraw = nullptr;

UxAtomicCounter* MxTerrain::_TriangleCounter  = nullptr;
UxAtomicCounter* MxTerrain::_DiscardedTriangleCounter = nullptr;

bool MxTerrain::_Startup = false;
void MxTerrain::Startup()
{
  if (!_Startup)
  {
    std::vector<UxShader> shaders;
    shaders.emplace_back(GL_GEOMETRY_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/ubo_heightmap.glsl", "Shaders/ubo_lighting.glsl", "Shaders/tx_mapcomputing.glsl", "Shaders/tx_geometry.glsl" }));
    shaders.emplace_back(GL_FRAGMENT_SHADER, std::vector<std::string>({ "Shaders/ubo_heightmap.glsl", "Shaders/tx_fragment.glsl" }));
    shaders.emplace_back(GL_VERTEX_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/ubo_heightmap.glsl", "Shaders/tx_mapcomputing.glsl", "Shaders/tx_vertex.glsl" }));
    shaders.emplace_back(GL_TESS_CONTROL_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/ubo_heightmap.glsl", "Shaders/tx_mapcomputing.glsl", "Shaders/tx_tesselation_control.glsl" }));
    shaders.emplace_back(GL_TESS_EVALUATION_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/ubo_heightmap.glsl", "Shaders/tx_mapcomputing.glsl", "Shaders/tx_tesselation_evaluation.glsl" }));
    shaders.emplace_back(GL_GEOMETRY_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/ubo_heightmap.glsl", "Shaders/ubo_lighting.glsl", "Shaders/tx_mapcomputing.glsl", "Shaders/tx_geometry_wireframe.glsl" }));
    shaders.emplace_back(GL_FRAGMENT_SHADER, std::vector<std::string>({ "Shaders/tx_fragment_wireframe.glsl" }));
    shaders.emplace_back(GL_GEOMETRY_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/map_pointgeometry.glsl" }));
    shaders.emplace_back(GL_VERTEX_SHADER, std::vector<std::string>({ "Shaders/ubo_heightmap.glsl", "Shaders/ubo_positionning.glsl", "Shaders/map_vertex.glsl" }));
    shaders.emplace_back(GL_FRAGMENT_SHADER, std::vector<std::string>({ "Shaders/map_fragment.glsl" }));
    shaders.emplace_back(GL_GEOMETRY_SHADER, std::vector<std::string>({ "Shaders/ubo_viewing.glsl", "Shaders/ubo_positionning.glsl", "Shaders/map_wiregeometry.glsl" }));
    
    const char* patchAttributeBindings[][2] = { { "PositionCoordinates4f", "i_VertexPos" },{ "HeightTextureCoordinates2f", "i_HeightTextureUV" },{ "ColorComponents4f", "i_VertexColor" } };

    _TriangleDraw = new UxProgram("Fill Terrain");
    _TriangleDraw->attachShaders(shaders, 0, 4);
    _TriangleDraw->bindVertexAttributes(patchAttributeBindings, SizeOfTable(patchAttributeBindings));
    
    _WireframeDraw = new UxProgram("Wireframe Terrain");
    _WireframeDraw->attachShaders(shaders, 2, 6);
    _WireframeDraw->bindVertexAttributes(patchAttributeBindings, SizeOfTable(patchAttributeBindings));

    const char* mapAttributeBindings[][2] = { { "PositionOnPlaneCoordinates2f", "i_VertexPos" },{ "PixelCoordinates2i", "i_Pixel" } };

    _PointMapDraw = new UxProgram("Map Cloud of Points");
    _PointMapDraw->attachShaders(shaders, 7, 9);
    _PointMapDraw->bindVertexAttributes(mapAttributeBindings, SizeOfTable(mapAttributeBindings));

    _WireframeMapDraw = new UxProgram("Map Wire");
    _WireframeMapDraw->attachShaders(shaders, 8, 10);
    _WireframeMapDraw->bindVertexAttributes(mapAttributeBindings, SizeOfTable(mapAttributeBindings));

    for (auto it = shaders.begin(); it != shaders.end(); it++)
      glDeleteShader(it->getGLName());

    // Uniform blocks registration and bindings
    UxGLObjects::addUniformBlock<u_HeightMap>("HeightMapTerrain");
    UxGLObjects::addUniformBlock<u_Positionning>("TerrainPositionning");

    UxGLObjects::getUniformBlock("SceneLighting")->bindToProgram(_TriangleDraw, "u_Lighting");
    UxGLObjects::getUniformBlock("SceneLighting")->bindToProgram(_WireframeDraw, "u_Lighting");

    for (auto program : { _TriangleDraw, _WireframeDraw, _PointMapDraw, _WireframeMapDraw })
    {
      UxGLObjects::getUniformBlock("HeightMapTerrain")->bindToProgram(program, "u_HeightMap");
      UxGLObjects::getUniformBlock("TerrainPositionning")->bindToProgram(program, "u_Positionning");
      UxGLObjects::getUniformBlock("SceneViewing")->bindToProgram(program, "u_Viewing");
      program->bindReports();
      program->introspect();
    }
  
    // TO REVIEW: program link / attribute / uniform block /ssbo of report

    _TriangleCounter = new UxAtomicCounter(0, 0);
    _DiscardedTriangleCounter = new UxAtomicCounter(1, 0);

    _Startup = true;
  }
}

MxTerrain::MxTerrain()
{
  Startup();
} 

MxTerrain::~MxTerrain()
{
}

void MxTerrain::init(const Vector2f& iTerrainDimension, const Vector2i& iTerrainSubdivision, float iHeightFactor, float iMaxSubdivison, float iMaxPixelSubdivisionRatio, const std::string& iHeightMapTexturePath, const std::string& iHeightColorMapTexturePath, const Vector2f& iHeightColorMapBounds)
{
  _TerrainDimension          = iTerrainDimension; __AssertIfNot(iTerrainDimension[0] > 0.0f && iTerrainDimension[1] > 0.0f, "Invalid Terrain Dimension");
  _TerrainSubdivision        = iTerrainSubdivision; __AssertIfNot(iTerrainSubdivision[0] > 0 && iTerrainSubdivision[1] > 0, "Invalid Terrain Subdivision");
  _HeightFactor              = iHeightFactor; __AssertIfNot(iHeightFactor > 0.0f, "Invalid HeightFactor");
  _MaxSubdivison             = iMaxSubdivison; __AssertIfNot(iMaxSubdivison > 0, "Invalid Max Subdivison");
  _MaxPixelSubdivisionRatio  = iMaxPixelSubdivisionRatio; __AssertIfNot(iMaxPixelSubdivisionRatio > 0, "Invalid Max Pixel Subdivision Ratio");
  _HeightMapTexturePath      = iHeightMapTexturePath;
  _HeightColorMapTexturePath = iHeightColorMapTexturePath;
  _DistortionFactor          = 4.0f;
  _FunctionalMode            = 0.0f;

  setHeightColorMapBounds(iHeightColorMapBounds);

  UxUtils::createBindlessTexture(_HeightMapTexturePath, _HeightMapTextureName, _HeightTextureHandle);
  UxUtils::createBindlessTexture(_HeightColorMapTexturePath, _HeightColorMapTextureName, _HeightColorMapHandle);
  glGetTextureLevelParameteriv(_HeightMapTextureName, 0, GL_TEXTURE_WIDTH, &_Width);
  glGetTextureLevelParameteriv(_HeightMapTextureName, 0, GL_TEXTURE_HEIGHT, &_Height);

  //UxUtils::createTangents(_HeightMapTextureName);

  _ColorMode     = 0;
  _IsolineStep   = 0.0f;
  _SmoothMode    = 0;
  _WireframeMode = 0;
  _MapMode       = 0;
  _ShadowMode    = 0;
  _MinHeight     = -1.0f;
  _MaxHeight     = -1.0f;

  generateTerrainData();
  generateMapData();
}

void MxTerrain::generateTerrainData()
{
  GLubyte* pPixels = new GLubyte[3*_Width*_Height];
  glGetTextureImage(_HeightMapTextureName, 0, GL_RGB, GL_UNSIGNED_BYTE, 3*_Width*_Height, pPixels);

  // Fills vertex buffer with patch coordinates
  float du = 1.0f / _TerrainSubdivision[0];
  float dv = 1.0f / _TerrainSubdivision[1];
  for (uint16_t i = 0; i <= _TerrainSubdivision[0]; i++)
  {
    for (uint16_t j = 0; j <= _TerrainSubdivision[1]; j++)
    {
      float u = (float)i*du;
      float v = 1.0f - (float)j*dv;
      float z = MxHeightComputation::getHeight(pPixels, _Width, _Height, _FunctionalMode, _SmoothMode, u, v, _HeightFactor, _MinHeight, _MaxHeight);
      _Vertices.push_back({ (float)i, (float)j, z, 1.0f, u, v, 1.0, 1.0, 1.0, 1.0 });
    }
  }

  _Indices = new uint32_t[_Vertices.size()];

  // Links input attributes to vertex array segments
  _PatchVertexArray.linkAttribute(MxGLObjects::getInputAttribute("PositionCoordinates4f"), &TerrainVertexData::position, GL_FLOAT, GL_FALSE);
  _PatchVertexArray.linkAttribute(MxGLObjects::getInputAttribute("HeightTextureCoordinates2f"), &TerrainVertexData::uv, GL_FLOAT, GL_FALSE);
  _PatchVertexArray.linkAttribute(MxGLObjects::getInputAttribute("ColorComponents4f"), &TerrainVertexData::color, GL_FLOAT, GL_FALSE);
}

void MxTerrain::generateMapData()
{
  // Generates position for each pixel
  float dx = (float)_TerrainSubdivision[0] / (float)(_Width - 1);
  float dy = (float)_TerrainSubdivision[1] / (float)(_Height - 1);
  for (uint32_t iX = 0; iX < (uint32_t)_Width; iX++)
  {
    for (uint32_t iY = 0; iY < (uint32_t)_Height; iY++)
      _MapVertexArray.addVertex({ { iX*dx, iY*dy },{ (int32_t)iX, (int32_t)_Height - (int32_t)iY - 1 } });
  }

  _MapVertexArray.store(GL_STATIC_DRAW);
  
  // Fills index buffer (matrix for points and line strips for wireframe)
  _PointMapIndexBuffer.addLinearMatrix(0, _Width, _Height);
  _PointMapIndexBuffer.store(GL_STATIC_DRAW);
  _WireframeMapIndexBuffer.addColumnLineStripsFromMatrix(0, true, _Width, _Height, 0xFFFFFFFF);
  _WireframeMapIndexBuffer.addRowLineStripsFromMatrix(0, true, _Width, _Height, 0xFFFFFFFF);
  _WireframeMapIndexBuffer.store(GL_STATIC_DRAW);

  // Links input attributes to vertex array segments
  _MapVertexArray.linkAttribute(MxGLObjects::getInputAttribute("PositionOnPlaneCoordinates2f"), &MapVertexData::position, GL_FLOAT, GL_FALSE);
  // If type for glVertexAttribPointer set to GL_UNSIGNED_INT, the data in the shader is coded as a float!
  _MapVertexArray.linkAttribute(MxGLObjects::getInputAttribute("PixelCoordinates2i"), &MapVertexData::uv, GL_FLOAT, GL_FALSE);
}

void MxTerrain::sendData(const Matrix4f& iModelMatrix, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle)
{
  // Alternative to static grid sent once to GPU: send a relimited part of the grid according to visibility criterion
  // To be evaluated (ie: performance/resource consuption advantages to use CPU or GPU
  memset(_Indices, 0xFF, (_TerrainSubdivision[0]+1)*(_TerrainSubdivision[1]+1)*sizeof(uint32_t));
 
  // Browses the grid of vertices and checks visibility criterium for every one (property stored in _Indices,
  // 0xFF: not visible and 0xFE: visible)
  uint32_t vIndex = 0;
  for (uint16_t i = 0; i <= _TerrainSubdivision[0]; i++)
  {
    for (uint16_t j = 0; j <= _TerrainSubdivision[1]; j++)
    {
      // Needs to transform every vertex from model to wc coordinates (is this a good trade-off CPU/GPU model matrix transformation?)
      float*   position = _Vertices[vIndex++].position;
      Vector4f vertex   = iModelMatrix*Vector4f(position[0], position[1], position[2], position[3]);

      // Potential need of height recomputation in case of dynamic min/max height constraints
      if (_MinHeight != -1 || _MaxHeight != -1)
      {
        if (_MinHeight != -1 && (vertex[2] < _MinHeight * _HeightFactor))
          vertex[2] = _MinHeight * _HeightFactor;
        if (_MaxHeight != -1 && (vertex[2] > _MaxHeight * _HeightFactor))
          vertex[2] = _MaxHeight * _HeightFactor;
      }

      // Vector from the eye point (wc) to the vertex (wc)
      Vector3f ev = Vector3f(vertex[0] - iEyeView[0], vertex[1] - iEyeView[1], vertex[2] - iEyeView[2]);

      // 1. Position of the vertex with respect to the screen plane
      float side = ev.dotProduct(iEyeDirection);

      // 2. Angle eye/vertex to eye direction: inaccurate (all the 4 vertices might be outside the solid angle and the "center"
      // part of the patch still visible)
      //ev.normalize();
      //float angle = acosf(ev.dotProduct(iEyeDirection));
      //if (angle < iAngle)
      
      if (side > 0)
        _Indices[i*(_TerrainSubdivision[1]+1)+j] = 0xFFFFFFFE;
    }
  }

  // Clears the VAO and the Index Buffer prior new feeding
  _PatchVertexArray.clearVector();
  _PatchIndexBuffer.clearVector();
  
  // Browses the grid of patches and fills the vertex and index buffers from every patch to be draw
  uint32_t indIndex = 0;
  for (uint16_t i = 0; i < _TerrainSubdivision[0]; i++)
  {
    for (uint16_t j = 0; j < _TerrainSubdivision[1]; j++)
    {
      uint32_t ind       = i*(_TerrainSubdivision[1]+1)+j;
      uint32_t shifts[4] = { ind, ind+_TerrainSubdivision[1]+1, ind+_TerrainSubdivision[1]+2, ind+1 };

      if (_Indices[shifts[0]] != 0xFFFFFFFF || _Indices[shifts[1]] != 0xFFFFFFFF || _Indices[shifts[2]] != 0xFFFFFFFF || _Indices[shifts[3]] != 0xFFFFFFFF)
      {
        for (auto vi : shifts)
        {
          if (_Indices[vi] >= 0xFFFFFFFE)
          {
            auto vertex = _Vertices[vi];
            if (_MinHeight != -1 || _MaxHeight != -1)
            {
              if (_MinHeight != -1 && (vertex.position[2] < _MinHeight * _HeightFactor))
                vertex.position[2] = _MinHeight * _HeightFactor;
              if (_MaxHeight != -1 && (vertex.position[2] > _MaxHeight * _HeightFactor))
                vertex.position[2] = _MaxHeight * _HeightFactor;
            }

            _PatchVertexArray.addVertex(vertex);
            _Indices[vi] = indIndex;
            _PatchIndexBuffer.addElement(0, indIndex);
            indIndex++;
          }
          else
            _PatchIndexBuffer.addElement(0, _Indices[vi]);
        }
      }
    }
  }

  _PatchIndexBuffer.store(GL_STREAM_DRAW);
  _PatchVertexArray.store(GL_STREAM_DRAW);
}

void MxTerrain::render(int iTime, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle, uint32_t& oPatchNb, uint32_t& oDrawnPatchNb, uint32_t& oTriangleNb, uint32_t& oDiscardedTriangleNb)
{
  // Initializes report
  reportVertex.init();

  // Initializes atomic counters
  _TriangleCounter->set(0);
  _DiscardedTriangleCounter->set(0);

  // Update uniform blocks (positionning and height map parameters)
  Matrix4f modelMatrix = Matrix4f::createScale(_TerrainDimension[0] / _TerrainSubdivision[0], _TerrainDimension[1] / _TerrainSubdivision[1], 1.f);
  {
    std::shared_ptr<UxUniformBlock<u_Positionning>> uniformP = UxGLObjects::getUniformBlock<u_Positionning>("TerrainPositionning");
    UxUniformBlockDataAccessor<u_Positionning> accessorP(uniformP);
    accessorP->model = modelMatrix;

    std::shared_ptr<UxUniformBlock<u_HeightMap>> uniformM = UxGLObjects::getUniformBlock<u_HeightMap>("HeightMapTerrain");
    UxUniformBlockDataAccessor<u_HeightMap> accessorM(uniformM);
    accessorM->heightTextureHandle      = _HeightTextureHandle;
    accessorM->terrainDimension         = _TerrainDimension;
    accessorM->terrainSubdivision       = _TerrainSubdivision;
    accessorM->heightFactor             = _HeightFactor;
    accessorM->distortionFactor         = _DistortionFactor;
    accessorM->maxSubdivison            = _MaxSubdivison;
    accessorM->maxPixelSubdivisionRatio = _MaxPixelSubdivisionRatio;
    accessorM->colorMode                = _ColorMode;
    accessorM->heightColorMapHandle     = _HeightColorMapHandle;
    accessorM->heightColorMapBounds     = _HeightColorMapBounds;
    accessorM->isolineStep              = _IsolineStep;
    accessorM->wireframeMode            = _WireframeMode;
    accessorM->functionalMode           = _FunctionalMode;
    accessorM->smoothMode               = _SmoothMode;
    accessorM->shadowMode               = _ShadowMode;
    accessorM->minHeight                = _MinHeight;
    accessorM->maxHeight                = _MaxHeight;
  }

  // Back-face culling managed (almost completely) by tesselation control and geometry shaders
  //glEnable(GL_CULL_FACE);
  
  glEnable(GL_DEPTH_TEST);
  glPatchParameteri(GL_PATCH_VERTICES, 4);

  // Builds vertex/index data to send to the pipeline
  sendData(modelMatrix, Vector3f(iEyeView[0], iEyeView[1], iEyeView[2]), iEyeDirection, iAngle);

  // Stores the patch numbers (total and sent to draw) 
  oPatchNb      = _TerrainSubdivision[0] * _TerrainSubdivision[1];
  oDrawnPatchNb = _PatchIndexBuffer.getBufferSize() / 4;

  // Draw terrain patches
  _TriangleDraw->draw(GL_PATCHES, _PatchVertexArray, _PatchIndexBuffer);

  if (_WireframeMode > 0)
  {
    // Draws wireframe patch borders and/or normals
    glLineWidth(1.0f);
    _WireframeDraw->draw(GL_PATCHES, _PatchVertexArray, _PatchIndexBuffer);
  }

  if (_MapMode == 1)
  {
    // Draws grid of points of the height map
    glEnable(GL_PROGRAM_POINT_SIZE);
    _PointMapDraw->draw(GL_POINTS, _MapVertexArray, _PointMapIndexBuffer);
  }
  else if (_MapMode == 2)
  {
    // Draws wireframe grid of the height map
    glLineWidth(1.0f);
    _WireframeMapDraw->draw(GL_LINE_STRIP, _MapVertexArray, _WireframeMapIndexBuffer);
  }

  //glMemoryBarrier(GL_ALL_BARRIER_BITS);
    
  // Stores triangle numbers (displayed and discared by the pipeline)
  oTriangleNb          = _TriangleCounter->get();
  oDiscardedTriangleNb = _DiscardedTriangleCounter->get();

  // Map the report to be able to dump the content during debug session
  reportVertex.map();
  uint32_t vertexCounter = reportVertex.getRecordNumber();
  auto table = reportVertex.getRecords();
  reportVertex.unmap();
}
