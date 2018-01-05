//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxAnimation.h"

#include <sstream>
#include <iomanip>

MxAnimation::MxAnimation(int iStartPause, int iEndPause)
{
  _StartPause  = iStartPause;
  _EndPause    = iEndPause;

  _FrozenUntil = 0;
  _Enabled     = false;
  _Completed   = false;

  _Step        = 0.0f;
  _DeltaStep   = 0.0f;
  _StartTime   = 0;
  _LastTime    = 0;
}

void MxAnimation::init(int iTime)
{
  _Step      = 0;
  _DeltaStep = 0;
  _StartTime = _LastTime = iTime + _StartPause;
  _Completed = false;
  freezeUntil(iTime + _StartPause);
}

std::string MxAnimation::getStage() const
{
  if (_StartTime == 0)
    return "Not yet started";

  std::stringstream ss;
  ss << std::setprecision(3) << _Step  << "/ 1.0";
  return ss.str();
}

bool MxAnimation::isFrozen(int iTime)
{
  if (iTime < _FrozenUntil)
    return true;

  if (_Completed)
  {
    init(iTime);
    return true;
  }

  _FrozenUntil = 0;
  return false;
}

void MxAnimation::upadeStage(int iTime, float iStep, float iDeltaStep)
{
  if (iStep > 1.0f)
  {
    _DeltaStep = 0;
    _Completed = true;
    freezeUntil(iTime + _EndPause);
  }
  else
  {
    _Step      = iStep;
    _DeltaStep = iDeltaStep;
    _LastTime  = iTime;
  }
}
