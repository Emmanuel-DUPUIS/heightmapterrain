//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include <set>
#include <vmath.h>

struct GLFWwindow;

class MxViewer
{
private:

  GLFWwindow* _Window;
  Matrix4f    _ViewMatrix;
  Matrix4f    _ProjectionMatrix;

  struct camera
  {
    Vector3f position;
    Vector3f aimed;
    Vector3f dir;
    Vector3f right;
    Vector3f up;
  } _camera;

  static MxViewer* _TheViewer;

  static std::set<int> PressedKeys;
  static double LastOnMoveW;
  static double LastOnMoveH;
  static double LastonKeyPressedW;
  static double LastonKeyPressedH;
  static double LastOnMouseW;
  static double LastOnMouseH;
  static int    LeftButton;

  static void onResize(GLFWwindow* window, int w, int h);
  static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void onMouseMove(GLFWwindow* window, double x, double y);
  
  void translateCamera(float dw, float dh);
  void modifyShotCamera(float ratio);
  void rotateCamera(float angle, Vector3f direction);

  void buildViewMatrix();


public:
  MxViewer(GLFWwindow* iWindow);
  ~MxViewer() {}

  const Matrix4f& getViewMatrix() const { return _ViewMatrix; }
  const Matrix4f& getProjectionMatrix() const { return _ProjectionMatrix; }
  const Vector2i  getViewport() const;
};
