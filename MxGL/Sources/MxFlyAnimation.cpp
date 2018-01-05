//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxFlyAnimation.h"
#include "MxScene.h"
#include "UxUtils.h"

#include <iomanip>


MxFlyAnimation::MxFlyAnimation(float iLength, float iSpeed): MxAnimation(3000,3000)
{
  _StartPosition = Vector3f(-50.0f, 400.0f, 80.0f);
  _StartDir      = Vector3f(1.0f, 0.f, 0.0f);
  _StartDir.normalize();

  _Length = iLength;
  _Speed  = iSpeed;
}

std::string MxFlyAnimation::getStage() const
{
  if (_StartTime == 0)
    return "Not yet started";

  uint32_t swidth = std::to_string((int)_Length).length();
  std::stringstream ss;
  ss << std::setw(swidth) << (int)(_Step*_Length+0.5) << "/" << std::setw(swidth) << (int)_Length;
  return ss.str();
}

void MxFlyAnimation::init(int iTime)
{
  _Position  = _StartPosition;
  _Dir       = _StartDir;

  MxAnimation::init(iTime);
}

void MxFlyAnimation::upadeStage(int iTime)
{
  float k = 0.001f * _Speed / _Length;

  MxAnimation::upadeStage(iTime, k * (iTime - _StartTime), k * (iTime - _LastTime));
}

void MxFlyAnimation::execute(MxScene* iScene, int iTime, uint64_t iFrame)
{
  if (!isFrozen(iTime))
    upadeStage(iTime);

  float step1 = 0.1f;
  float step2 = 0.4f;
  float step3 = 0.7f;

  // Smooth function 3t2-2t3 inside [0,1]
  float steps[] = { 0.0f, 0.1f, 0.35f, 0.6f, 0.7f, 1.0f };
  float values[] = { 0.0f, 0.0f, 20.0f, -20.0f, 0.0f, 0.0f };
  auto smoothTransition = [&steps, &values, this](float t) { uint32_t index = 0; while (steps[index+1] < _Step) index++; float u = (t - steps[index]) / (steps[index+1] - steps[index]); float u2 = u*u; return values[index] + (values[index+1] - values[index])*u2*(3 - 2 * u); };
   
  float pitch  = smoothTransition(_Step) *(float)M_PI / 180.0f;
  float dPitch = pitch - smoothTransition(_Step - _DeltaStep) *(float)M_PI / 180.0f;
  
  _Dir = UxUtils::rotateVector(_Dir, _Dir.crossProduct(Vector3f(0.0f, 0.0f, 1.0f)), dPitch);

  Vector3d up;
  if (_Step <= step1)
  {
    _Position = _StartPosition + _Dir * _Step * _Length;
    up = Vector3f(.0f, .0f, -1.f).crossProduct(_Dir).crossProduct(_Dir);
  }
  else if (_Step <= step3)
  {
    bool first = _Step < step2;

    float subStep   = first ? (_Step - step1)/(step2-step1) :(step3 - _Step) / (step3 - step2);
    float curvature = subStep * 0.0002f;
    
    float dPhi = _Length*_DeltaStep*curvature;
    float s = sinf(dPhi);
    float c = cosf(dPhi);

    Vector3f centerDir = _Dir.crossProduct(Vector3f(0.0f, 0.0f, 1.0f));
    _Position += _Dir * (s / curvature) + centerDir * ((1.0f-c) / curvature);
    _Dir = Vector3f(c*_Dir[0] + s*centerDir[0], c*_Dir[1] + s*centerDir[1], _Dir[2]);
    _Dir.normalize();

    float roll = subStep*15.0f*(float)M_PI / 180.0f;
    up = UxUtils::rotateVector(Vector3f(0.0f, 0.0f, 1.0f), _Dir, roll);
  }
  else if (_Step < 1.0f)
  {
    _Position += _Dir * _DeltaStep * _Length;
    up = Vector3f(.0f, .0f, -1.f).crossProduct(_Dir).crossProduct(_Dir);
  } 
  
  // Updating scene's view matrix according camera position (camera slightly bent)
  Vector3f camDir = Vector3f(_Dir[0], _Dir[1], -0.15f);
  camDir.normalize();
  iScene->setViewMatrix(Matrix4f::createLookAt(_Position, _Position + camDir, up));
}
