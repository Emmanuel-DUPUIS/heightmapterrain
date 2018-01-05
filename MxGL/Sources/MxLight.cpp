//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxLight.h"
#include "UxUniformBlockDataAccessor.h"
#include "MxScene.h"
#include "UxGLObjects.h"


bool MxLight::_Startup = false;
void MxLight::Startup()
{
  if (!_Startup)
  {
    // Uniform block registration
    UxGLObjects::addUniformBlock<u_Lighting>("SceneLighting");
  }
}

MxLight::MxLight()
{
  Startup();
}

MxLight::~MxLight()
{

}

void MxLight::init(const Vector4f& iPosition, const Vector4f& iAmbiantColor, const Vector4f& iDiffuseColor, const Vector3f& iSpecularColor, float iSpecularPower)
{
  setPosition(iPosition);
  setAmbiantColor(iAmbiantColor);
  setDiffuseColor(iDiffuseColor);
  setSpecularColor(iSpecularColor);
  setSpecularPower(iSpecularPower);
}

void MxLight::render(int iTime, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle, uint32_t& oPatchNb, uint32_t& oDrawnPatchNb, uint32_t& oTriangleNb, uint32_t& oDiscardedTriangleNb)
{
  std::shared_ptr<UxUniformBlock<u_Lighting>> uniformL = UxGLObjects::getUniformBlock<u_Lighting>("SceneLighting");
  UxUniformBlockDataAccessor<u_Lighting> accessorL(uniformL);

  accessorL->position      = _Position;
  accessorL->ambiantColor  = _AmbiantColor;
  accessorL->diffuseColor  = _DiffuseColor;
  accessorL->specularColor = _SpecularColor;
  accessorL->specularPower = _SpecularPower;

  oPatchNb = oDrawnPatchNb = oTriangleNb = oDiscardedTriangleNb = 0;
}
