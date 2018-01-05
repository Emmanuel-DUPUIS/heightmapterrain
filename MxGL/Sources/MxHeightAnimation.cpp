//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxHeightAnimation.h"
#include "UxUniformBlock.h"
#include "UxUniformBlockDataAccessor.h"
#include "MxTerrain.h"
#include "UxGLObjects.h"

#include <iomanip>


MxHeightAnimation::MxHeightAnimation(std::shared_ptr<MxTerrain> iTerrain, bool iMinHeight, bool iMaxHeight, float iSecDuration): MxAnimation(3000,3000)
{
  _Terrain   = iTerrain;
  _MinHeight = iMinHeight;
  _MaxHeight = iMaxHeight;
  _Duration  = (uint32_t)(1000*iSecDuration); // in ms
}


std::string MxHeightAnimation::getStage() const
{
  if (_StartTime == 0)
    return "Not yet started";

  std::stringstream ss;
  float hMax = _Terrain->getHeightFactor();
  float h    = (_MinHeight ? 1-_Step : _Step) * hMax;
  ss << std::setw((int)(logf(hMax)/logf(10.0f))+1) << (int)(h) << "/" << (int)hMax;
  return ss.str();
}

void MxHeightAnimation::init(int iTime)
{
  MxAnimation::init(iTime);
}

void MxHeightAnimation::upadeStage(int iTime)
{
  MxAnimation::upadeStage(iTime, (iTime - _StartTime) / (float)_Duration, (iTime - _LastTime) / (float)_Duration);
}

void MxHeightAnimation::execute(MxScene* iScene, int iTime, uint64_t iFrame)
{
  if (!isFrozen(iTime))
    upadeStage(iTime);

  _Terrain->setMinHeight(_MinHeight ? 1.0f - _Step : -1.0f);
  _Terrain->setMaxHeight(_MaxHeight ? _Step : -1.0f);
}
