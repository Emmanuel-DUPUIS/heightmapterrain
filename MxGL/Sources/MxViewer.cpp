//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxViewer.h"
#include "UxUtils.h"

#include <glfw/glfw3.h>

std::set<int> MxViewer::PressedKeys;
double MxViewer::LastOnMoveW = 0;
double MxViewer::LastOnMoveH = 0;
double MxViewer::LastonKeyPressedW = 0;
double MxViewer::LastonKeyPressedH = 0;
double MxViewer::LastOnMouseW = 0;
double MxViewer::LastOnMouseH = 0;
int    MxViewer::LeftButton = 0;

MxViewer* MxViewer::_TheViewer = nullptr;

MxViewer::MxViewer(GLFWwindow* iWindow)
{
  _Window    = iWindow;
  _TheViewer = this;

  _camera.position = { -200.f, 100.f, 300.f };
  _camera.aimed    = { 100.f, 300.f, 100.f };
  _camera.dir      = (_camera.aimed - _camera.position);
  _camera.dir.normalize();

  _camera.up = Vector3f(.0f, .0f, -1.f).crossProduct(_camera.dir).crossProduct(_camera.dir);
  _camera.up.normalize();
  
  _camera.right = _camera.dir.crossProduct(_camera.up);

  int width, height;
  glfwGetWindowSize(_Window, &width, &height);

  float ratio = (float)width / (float)height;
  float size = 0.5f;
  float nearDist = 1.0f;
  float farDist = 4000.f;
  _ProjectionMatrix = Matrix4f::createFrustum(-ratio*size, ratio*size, -size, size, nearDist, farDist);

  // glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetWindowSizeCallback(_Window, MxViewer::onResize);
  glfwSetKeyCallback(_Window, MxViewer::onKeyPressed);
  glfwSetMouseButtonCallback(_Window, MxViewer::onMouseButton);
  glfwSetCursorPosCallback(_Window, MxViewer::onMouseMove);

  buildViewMatrix();
}

const Vector2i MxViewer::getViewport() const
{
  int width, height;
  glfwGetWindowSize(_Window, &width, &height); 
  return Vector2i(width, height);
}

void MxViewer::buildViewMatrix()
{
  _ViewMatrix = Matrix4f::createLookAt(_camera.position, _camera.position + _camera.dir, _camera.up);
}

void MxViewer::translateCamera(float dw, float dh)
{
  float ratio = 5.0f * (_camera.position - _camera.aimed).length();
  dw *= ratio;
  dh *= ratio;
  Vector3f delta = _camera.right*dw + _camera.up*dh;
  _camera.position += delta;
  _camera.aimed    += delta;
  buildViewMatrix();
}

void MxViewer::modifyShotCamera(float ratio)
{
  Vector3f dz = _camera.dir*(_camera.aimed -_camera.position).length()*ratio;
  _camera.position += dz;
  _camera.aimed    += dz*0.5f;
  buildViewMatrix();
}

void MxViewer::rotateCamera(float angle, Vector3f direction)
{
  _camera.position = UxUtils::rotatePoint(_camera.position, _camera.aimed, direction, angle);
  _camera.dir = _camera.aimed - _camera.position;
  _camera.dir.normalize();
  _camera.up = UxUtils::rotateVector(_camera.up, direction, angle);
  _camera.right = _camera.dir.crossProduct(_camera.up);
  buildViewMatrix();
}

void MxViewer::onResize(GLFWwindow* window, int w, int h)
{
  glViewport(0, 0, w, h);
}

void MxViewer::onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    PressedKeys.insert(key);
    LastonKeyPressedW = LastOnMoveW;
    LastonKeyPressedH = LastOnMoveH;
  }
  else if (action == GLFW_RELEASE)
    PressedKeys.erase(key);
}

void MxViewer::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    LeftButton = action;
}

void MxViewer::onMouseMove(GLFWwindow* window, double x, double y)
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  auto end = PressedKeys.end();
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  {
    // Move camera along depth axis
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) //PressedKeys.find(GLFW_KEY_LEFT_CONTROL) != end || PressedKeys.find(GLFW_KEY_RIGHT_CONTROL) != end)
    {
      float ratio = 5.0f * ((float)LastOnMoveH - (float)y) / height;
      //std::cerr << "ratio=" << 5.0f * ((float)y - (float)LastOnMoveH) / height << "," << ratio << " y=" << y << " LastOnMoveH=" << LastOnMoveH << "\n";
      _TheViewer->modifyShotCamera(ratio);
    }
    else
    {
      _TheViewer->translateCamera(((float)LastOnMoveW - (float)x)/width, ((float)y - (float)LastOnMoveH)/height);
      //std::cerr << "dx=%lf dy=%lf\n",  (LastOnMoveW-x) / 10.f, (y - LastOnMoveH) / 10.f);
    }
  }
  else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
      Vector2f v1 = Vector2f((float)LastonKeyPressedW - .5f*width, (float)LastonKeyPressedH - .5f*height);
      v1.normalize(); 
      Vector2f v2 = Vector2f((float)x - .5f*(float)width, (float)y - .5f*(float)height);
      v2.normalize();

      float angle = (signbit(v1[0] * v2[1] - v1[1] * v2[0])?1.f:-1.0f)*acosf(v1[0]*v2[0]+v1[1]*v2[1]);
      float angleW = 15.f*((float)x - (float)LastOnMoveW)/width;
      float angleH =15.f*((float)LastOnMoveH - (float)y)/height;
      _TheViewer->rotateCamera(-angleW, _TheViewer->_camera.up);
      _TheViewer->rotateCamera(angleH, _TheViewer->_camera.right);
  }
    
  LastOnMoveW = x;
  LastOnMoveH = y;
}


