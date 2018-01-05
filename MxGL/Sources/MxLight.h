//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "MxSceneObject.h"
#include "vmath.h"

//========================================================================
//  Ligth (scene object):
//    Single light illuminating the scene.
//========================================================================

class MxLight: public MxSceneObject
{
private:
  static void Startup();
  static bool _Startup;

public:

  struct u_Lighting
  {
    Vector4f  position;
    Vector4f  ambiantColor;
    Vector4f  diffuseColor;
    Vector3f  specularColor;
    float     specularPower;
  };

private:

  Vector4f  _Position;
  Vector4f  _AmbiantColor;
  Vector4f  _DiffuseColor;
  Vector3f  _SpecularColor;
  float     _SpecularPower;

public:
  
  MxLight();
  ~MxLight();

  void init(const Vector4f& iPosition, const Vector4f& iAmbiantColor, const Vector4f& iDiffuseColor, const Vector3f& iSpecularColor, float iSpecularPower);
  
  void setPosition(const Vector4f& iPosition) { _Position = iPosition; }
  void setAmbiantColor(const Vector4f& iAmbiantColor) { _AmbiantColor = iAmbiantColor; }
  void setDiffuseColor(const Vector4f& iDiffuseColor) { _DiffuseColor = iDiffuseColor; }
  void setSpecularColor(const Vector3f& iSpecularColor) { _SpecularColor = iSpecularColor; }
  void setSpecularPower(float iSpecularPower) { _SpecularPower = iSpecularPower; }

protected:

  void render(int iTime, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle, uint32_t& oPatchNb, uint32_t& oDrawnPatchNb, uint32_t& oTriangleNb, uint32_t& oDiscardedTriangleNb);
};
