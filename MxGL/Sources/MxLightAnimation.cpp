//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxLightAnimation.h"
#include <iomanip>

MxLightAnimation::MxLightAnimation(std::shared_ptr<MxLight> iLight, float iSecDuration): MxAnimation(3000,3000)
{
  _Light    = iLight;
  _Duration = (uint32_t)(1000*iSecDuration); // in ms
}

std::string MxLightAnimation::getStage() const
{
  if (_StartTime == 0)
    return "Not yet started";

  int hr = (int)(12.0f * (0.5f + _Step) + 0.5f);
  std::stringstream ss;
  ss << std::setw(4) << std::setprecision(0) << (hr < 13 ? hr : hr-12) << (hr < 12 ? " AM" : " PM");
  return ss.str();
}

void MxLightAnimation::init(int iTime)
{
  MxAnimation::init(iTime);
}

void MxLightAnimation::upadeStage(int iTime)
{
  MxAnimation::upadeStage(iTime, (iTime - _StartTime) / (float)_Duration, (iTime - _LastTime) / (float)_Duration);
}

void MxLightAnimation::execute(MxScene* iScene, int iTime, uint64_t iFrame)
{
  if (!isFrozen(iTime))
    upadeStage(iTime);

  // Light azimuth relative to -x axis
  float k       = 1.2f;
  float a0      = (float)M_PI*k*0.5f;
  float azimuth = (float)M_PI*k*(_Step - 0.5f);
  
  // Inclination light trajectory in relation to xy plane
  float inclination = 45.0f * (float)M_PI / 180.0f;
  float radius      = 40000.0f;

  // Light moving along a circle
  float ca = cosf(azimuth);
  float sa = sinf(azimuth);

  Vector4f pos = Vector4f(-radius*ca*sinf(inclination), radius*sa, radius*ca*cosf(inclination), 0.0f);
  _Light->setPosition(pos);

  // Reddening colour when light near the horizon
  float           col   = 0.0f;
  constexpr float limit = 0.2f;
  if (ca < limit)
    col = (limit - ca) / (limit - cosf(a0)) / 255.0f;
  
  _Light->setAmbiantColor(Vector4f(1.0f-col*25, 1.0f-col*140, 1.0f-col*220, 1.0f)*0.15f);
  _Light->setDiffuseColor(Vector4f(1.0f - col * 25, 1.0f - col * 140, 1.0f - col * 220, 1.0f)*0.25f);
}
