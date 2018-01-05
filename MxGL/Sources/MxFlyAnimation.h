//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "MxAnimation.h"
#include <vmath.h>

class MxFlyAnimation: public MxAnimation
{
private:

  Vector3f      _StartPosition;
  Vector3f      _StartDir;
  Vector3f      _Position; 
  Vector3f      _Dir;
  float         _Length;
  float         _Speed;

public:

  MxFlyAnimation(float iLength, float iSpeed);
  ~MxFlyAnimation() {};

  virtual void init(int iTime);
  virtual void upadeStage(int iTime);
  virtual std::string getStage() const;
  virtual void execute(MxScene* iScene, int iTime, uint64_t iFrame);
};
