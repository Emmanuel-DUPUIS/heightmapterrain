//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "MxAnimation.h"
#include "MxLight.h"

//========================================================================
//  Light animation:
//    Simulates a sun light (sunrise/sunset, color modification) during
//    a whole daylight.
//========================================================================

class MxLightAnimation: public MxAnimation
{
private:

  std::shared_ptr<MxLight> _Light;
  uint32_t                 _Duration;

public:

  MxLightAnimation(std::shared_ptr<MxLight> iLight, float iSecDuration);
  ~MxLightAnimation() {};

  virtual std::string getStage() const;
  virtual void init(int iTime);
  virtual void upadeStage(int iTime);
  virtual void execute(MxScene* iScene, int iTime, uint64_t iFrame);
};
