//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "MxSceneObject.h"
#include "MxGLObjects.h"
#include "MxAnimation.h"
#include "UxUniformBlockBase.h"
#include "vmath.h"

//========================================================================
//  3D scene gathering different 3D objects:
//    Manages rendering and animating the different objects in the scene.
//========================================================================

class MxScene
{
private:

  // Static initialisations
  static bool _Startup;
  static void Startup();

public:

  struct u_Viewing
  {
    Matrix4f  view;
    Matrix4f  projection;
    Vector2i  viewport;
    uint64_t  __alignment;
  };

private:

  Matrix4f                                     _ViewMatrix;
  Matrix4f                                     _ProjectionMatrix;
  Vector2i                                     _Viewport;

  std::vector<std::shared_ptr<MxAnimation>>    _Animations;
  std::vector<std::shared_ptr<MxSceneObject>>  _Objects;

  uint32_t                                     _PatchNb;
  uint32_t                                     _DrawnPatchNb;
  uint32_t                                     _TriangleNb;
  uint32_t                                     _DiscardedTriangleNb;

public:

  MxScene();
  ~MxScene();

  void setViewMatrix(const Matrix4f& iViewMatrix) { _ViewMatrix = iViewMatrix; }

  uint32_t getTriangleNb() const { return _TriangleNb; }
  uint32_t getDiscardedTriangleNb() const { return _DiscardedTriangleNb; }
  
  uint32_t getPatchNb() const { return _PatchNb; }
  uint32_t getDrawnPatchNb() const { return _DrawnPatchNb; }

  void addObject(std::shared_ptr<MxSceneObject> iObject, bool iFirstPosition = false);

  void addAnimation(std::shared_ptr<MxAnimation> iAnimation, bool iFirstPosition = false);
  void disableAllAnimations();

  void render(int time, Matrix4f iViewMatrix, Matrix4f iProjectionMatrix, Vector2i iViewport);
};
