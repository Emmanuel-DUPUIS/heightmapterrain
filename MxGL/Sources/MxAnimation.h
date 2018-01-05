//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include <string>
#include <memory>

class MxScene;

class MxAnimation
{
protected:

  bool      _Enabled;
  bool      _Completed;
  int       _FrozenUntil;
  float     _Step;
  float     _DeltaStep;
  int       _StartPause;
  int       _EndPause;
  int       _StartTime;
  int       _LastTime;

public:

  MxAnimation(int iStartPause, int iEndPause);
  virtual ~MxAnimation() {}

  void enable() { _Enabled = true; }
  void disable() { _Enabled = false; }
  void toggleAbility() { _Enabled = !_Enabled; }

  bool isEnable(int iTime) { return _Enabled; }
  bool isFrozen(int iTime);
  int  frozenUntil() const { return _FrozenUntil; }
  void freezeUntil(int iTime) { if (iTime > _FrozenUntil) _FrozenUntil = iTime; }
  void upadeStage(int iTime, float iStep, float iDeltaStep);

  virtual void init(int iTime);
  virtual std::string getStage() const;
  virtual void execute(MxScene* iScene, int iTime, uint64_t iFrame) = 0;
};
