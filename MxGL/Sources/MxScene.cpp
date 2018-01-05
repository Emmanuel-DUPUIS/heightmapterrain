//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxScene.h"
#include "UxUniformBlockDataAccessor.h"

#include <algorithm>


bool MxScene::_Startup = false;
void MxScene::Startup()
{
  if (!_Startup)
  {
    MxGLObjects::addUniformBlock<u_Viewing>("SceneViewing");
  }
}

MxScene::MxScene()
{
  Startup();
}

MxScene::~MxScene()
{
}

void MxScene::addObject(std::shared_ptr<MxSceneObject> iObject, bool iFirstPosition)
{
  if (iFirstPosition)
    _Objects.insert(_Objects.begin(), iObject);
  else
    _Objects.push_back(iObject);
}

void MxScene::addAnimation(std::shared_ptr<MxAnimation> iAnimation, bool iFirstPosition)
{
  if (iFirstPosition)
    _Animations.insert(_Animations.begin(), iAnimation);
  else
    _Animations.push_back(iAnimation);
}

void MxScene::disableAllAnimations()
{
  for (auto anim : _Animations)
    anim->disable();
}

void MxScene::render(int iTime, Matrix4f iViewMatrix, Matrix4f iProjectionMatrix, Vector2i iViewport)
{
  static uint64_t frame = 0;

  _ViewMatrix       = iViewMatrix;
  _ProjectionMatrix = iProjectionMatrix;
  _Viewport         = iViewport;

  for (auto anim : _Animations)
  {
    if (anim->isEnable(iTime))
      anim->execute(this, iTime, frame);
  }

  {
    std::shared_ptr<UxUniformBlock<u_Viewing>> uniform = UxGLObjects::getUniformBlock<u_Viewing>("SceneViewing");
    UxUniformBlockDataAccessor<u_Viewing> accessor(uniform);

    accessor->projection  = _ProjectionMatrix;
    accessor->view        = _ViewMatrix;
    accessor->viewport    = _Viewport;
  }

  glClearColor(0.0f, 0.2f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _PatchNb = _DrawnPatchNb = _TriangleNb = _DiscardedTriangleNb = 0;

  Matrix4f mat = _ViewMatrix.inverse();
  Vector3f eyeView(mat.at(3, 0), mat.at(3, 1), mat.at(3, 2));
  Vector3f eyeDirection(-mat.at(2, 0), -mat.at(2, 1), -mat.at(2, 2));
  // Solid angle corresponding to the diagonal
  float    angle = atanf(1.0f/(_ProjectionMatrix[0]*_ProjectionMatrix[0]) + 1.0f/(_ProjectionMatrix[5]* _ProjectionMatrix[5]));

  for (auto obj : _Objects)
  {
    uint32_t nb1 = 0, nb2 = 0, nb3 = 0, nb4 = 0;
    obj->render(iTime, eyeView, eyeDirection, angle, nb1, nb2, nb3, nb4);
    _PatchNb += nb1;
    _DrawnPatchNb += nb2; 
    _TriangleNb += nb3;
    _DiscardedTriangleNb += nb4;
    
    __CheckGLErrors;
  }

  frame++;
}
