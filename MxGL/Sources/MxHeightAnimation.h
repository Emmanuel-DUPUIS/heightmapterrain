//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "MxAnimation.h"

class MxTerrain;

class MxHeightAnimation: public MxAnimation
{
private:

  std::shared_ptr<MxTerrain> _Terrain;
  bool                       _MinHeight;
  bool                       _MaxHeight;
  uint32_t                   _Duration;

public:

  MxHeightAnimation(std::shared_ptr<MxTerrain> iTerrain, bool iMinHeight, bool iMaxHeight, float iSecDuration);
  ~MxHeightAnimation() {};

  void setModes(bool iMinHeight, bool iMaxHeight) { _MinHeight = iMinHeight; _MaxHeight = iMaxHeight; }

  virtual std::string getStage() const;
  virtual void init(int iTime);
  virtual void upadeStage(int iTime);
  virtual void execute(MxScene* iScene, int iTime, uint64_t iFrame);
};
