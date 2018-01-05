//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "vmath.h"
#include <cstdint>

class MxSceneObject
{
public:
  virtual void render(int iTime, const Vector3f& iEyeView, const Vector3f& iEyeDirection, float iAngle, uint32_t& oPatchNb, uint32_t& oDrawnPatchNb, uint32_t& oTriangleNb, uint32_t& oDiscardedTriangleNb) = 0;
};
