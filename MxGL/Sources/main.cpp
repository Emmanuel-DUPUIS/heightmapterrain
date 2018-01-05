//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glfw/glfw3.h>

#include <IL/il.h>
//#include <IL/ilut.h>

#include "UxUtils.h"
#include "MxViewer.h"
#include "MxGLObjects.h"
#include "MxScene.h"
#include "MxTerrain.h"
#include "MxLight.h"
#include "MxFlyAnimation.h"
#include "MxLightAnimation.h"
#include "MxHeightAnimation.h"

#include <sstream>
#include <iomanip>

static uint32_t gWireframeMode = 0;
static uint32_t gColorMode = 0;
static uint32_t gIsolineMode = 0;
static uint32_t gFunctionalMode = 0;
static uint32_t gMapMode = 0;
static uint32_t gSmoothMode = 0;
static uint32_t gAnimationMode = 0;
static uint32_t gShadowMode = 0;
static uint32_t gDisplayHelp = 0;
static float    gDistortionFactor = 4.0f;

void onCharKeyPressed(GLFWwindow* window, unsigned int key);
float getIsolineStep(uint32_t iMode);
std::string formatLongInt(uint32_t iNumber);
void displayText(const std::string& i2DText, void* iFont);
void displayInfo(const MxViewer& iViewer, const MxScene& iScene, uint32_t iBeforeTime, uint32_t iAfterTime, const std::vector<std::shared_ptr<MxAnimation>>& iAnimations);
void displayHelp(const MxViewer& iViewer);

void main(int argc, char **argv)
{
   if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW\n";
    return;
  }

  GLFWwindow* window = glfwCreateWindow(1800, 1000, "Heightmap Terrain", /*glfwGetPrimaryMonitor()*/ NULL, NULL);
  if (!window)
  {
    std::cerr << "Failed to open window\n";
    return;
  }

  glfwMakeContextCurrent(window);
  glfwSetCharCallback(window, onCharKeyPressed);

  glewInit();
  glutInit(&argc, argv);

  std::cerr << "============================================\n OpenGL configuration\n";
  std::cerr << "    Vendor   : " << (char *)glGetString(GL_VENDOR) << "\n";
  std::cerr << "    Version  : " << (char *)glGetString(GL_VERSION) << "\n";
  std::cerr << "    Renderer : " << (char *)glGetString(GL_RENDERER) << "\n";
  std::cerr << "============================================\n";

  // Initialize IL (file decoding for texture definition)
  ilInit();
 

  // Creaates a 3D viewer to display the scene
  MxViewer viewer(window);

  // Creates a scene of 3D objects
  MxGLObjects::init();
  MxScene scene;

  // Creates a light (fixed and animated) and adds it to the scene
  auto spLight = std::make_shared<MxLight>();
  spLight->init({ 500.0f, 5000.0f, 1000.0f, 1.0f }, { 0.15f, 0.15f, 0.15f, 1.0f }, { 0.15f, 0.15f, 0.15f, 1.0f }, { 0.4f, 0.4f, 0.4f }, 1.0f);
  scene.addObject(spLight, true);

  // Creates a terrain from a jpeg file and adds it to the scene
  auto spTerrain = std::make_shared<MxTerrain>();
  spTerrain->init({ 2000.0f, 1000.0f }, {32, 16}, 350.0f, 64.0f, 100.0f, "Data/terrain1_128x64.jpg", "Data/reliefs.jpg", { 10.0f, 160.0f });
  scene.addObject(spTerrain);
  
  // Creates 4 animations (fly, sun light move, morphing)  
  auto spFlyAnimation       = std::make_shared<MxFlyAnimation>(1900.0f, 80.0f);
  auto spLightAnimation     = std::make_shared<MxLightAnimation>(spLight, 12.0f);
  auto spHeightAnimationMax = std::make_shared<MxHeightAnimation>(spTerrain, false, true, 12.0f);
  auto spHeightAnimationMin = std::make_shared<MxHeightAnimation>(spTerrain, true, false, 12.0f);

  std::vector<std::shared_ptr<MxAnimation>> animations;
  animations.push_back(spFlyAnimation);
  animations.push_back(spLightAnimation);
  animations.push_back(spHeightAnimationMax);
  animations.push_back(spHeightAnimationMin);

  scene.addAnimation(spFlyAnimation);
  scene.addAnimation(spLightAnimation);
  scene.addAnimation(spHeightAnimationMax);
  scene.addAnimation(spHeightAnimationMin);

  uint32_t frame = 0;
  bool running = true;
  do
  {
    // Applies user's parameter modifications to the terrain
    spTerrain->setWireframeMode(gWireframeMode);
    spTerrain->setColorMode(gColorMode);
    spTerrain->setIsolineStep(gIsolineMode == 0 ? 0.0f : getIsolineStep(gIsolineMode));
    spTerrain->setFunctionalMode(gFunctionalMode == 0 ? 0.0f : pow(20.0f,(float)gFunctionalMode));
    spTerrain->setMapMode(gMapMode);
    spTerrain->setSmoothMode(gSmoothMode);
    spTerrain->setShadowMode(gShadowMode);
    spTerrain->setDistortionFactor(gDistortionFactor);
    spTerrain->setMinHeight(-1.0f);
    spTerrain->setMaxHeight(-1.0f);

    // Reinits light to override animation modifications
    spLight->init({ 500.0f, 5000.0f, 1000.0f, 1.0f }, { 0.15f, 0.15f, 0.15f, 1.0f }, { 0.25f, 0.25f, 0.25f, 1.0f }, { 0.4f, 0.4f, 0.4f }, 1.0f);

    int timeBefore = glutGet(GLUT_ELAPSED_TIME);

    static uint32_t anim = 0;
    if (gAnimationMode != anim)
    {
      if (anim != 0)
        animations[anim-1]->disable();
      if (gAnimationMode != 0)
      {
        animations[gAnimationMode-1]->enable();
        animations[gAnimationMode-1]->init(timeBefore);
      }
    }

    anim = gAnimationMode;

    scene.render(timeBefore, viewer.getViewMatrix(), viewer.getProjectionMatrix(), viewer.getViewport());
    int timeAfter = glutGet(GLUT_ELAPSED_TIME);
    
    // Displays rednering info (duration, quantity of geo displayed/discared, active modes/parameters...)
    displayInfo(viewer, scene, timeBefore, timeAfter, animations);

    // Displays the help window if "h" key has been pressed
    if (gDisplayHelp)
      displayHelp(viewer);

    glfwSwapBuffers(window);
    glfwPollEvents();

    running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
    running &= (glfwWindowShouldClose(window) != GL_TRUE);

    frame++;
  } while (running);

  glfwDestroyWindow(window);
  glfwTerminate();
}

void displayInfo(const MxViewer& iViewer, const MxScene& iScene, uint32_t iBeforeTime, uint32_t iAfterTime, const std::vector<std::shared_ptr<MxAnimation>>& iAnimations)
{
  float dx = 2.0f / iViewer.getViewport()[0];
  float dy = 2.0f / iViewer.getViewport()[1];

  // Display information about rendering

  glUseProgram(0);

  glColor3f(0.0f, 0.0f, 0.0f);

  glRasterPos2f(30*dx-1.0f, 45*dy-1.0f);
  std::stringstream ss1;
  if (gFunctionalMode != 0)
    ss1 << "Function f(u,v)=" << gFunctionalMode << "*u(1-u).v(1-v)";
  else
  {
    switch (gSmoothMode)
    {
    case 0:
      ss1 << "Linear Height Interpolation, constant Normals";
      break;
    case 1:
      ss1 << "Linear Interpolation (Height, Normals)";
      break;
    case 2:
      ss1 << "Bicubic Interpolation (Height, Normals)";
      break;
    }
  }

  if (gAnimationMode != 0)
  {
    switch (gAnimationMode)
    {
    case 1:
      ss1 << " | Animation #1: fly over the terrain";
      break;
    case 2:
      ss1 << " | Animation #2: sun light from sunrise to sunset";
      break;
    case 3:
      ss1 << " | Animation #3: terrain generation from ground to altitude";
      break;
    case 4:
      ss1 << " | Animation #4: terrain generation from altitude to ground";
      break;
    }

    ss1 << " [" << iAnimations[gAnimationMode-1]->getStage() << "]";
    int fr = iAnimations[gAnimationMode - 1]->frozenUntil();
    if (fr)
    {
      float duration = (fr - iBeforeTime) / 1000.0f;
      int   nPart = (int)duration;
      int   dPart = (int)(duration*10.0f) - nPart*10;
      ss1 << " Frozen " << std::setw(2) << nPart << "." << dPart << " s";
    }
  }

  switch (gColorMode)
  {
  case 1:
    ss1 << " | Color Map";
    break;
  }

  if (gIsolineMode > 0)
  {
    ss1 << " | isoline every " << getIsolineStep(gIsolineMode) << " units (orange)";
  }
    
  switch (gMapMode)
  {
  case 1:
    ss1 << " | Height Map vertices (red, green: projection on z=0)";
    break;
  case 2:
    ss1 << " | Height Map network (red)";
    break;
  }

  ss1 << " | Distortion factor=" << std::setprecision(3) << gDistortionFactor;
  
  switch (gWireframeMode)
  {
    case 1:
      ss1 << " | Patch border (green) and subdivisions (blue)";
      break;
    case 2:
      ss1 << " | Normals (white: triangle, pink: vertex from map)";
      break;
    case 3:
      ss1 << " | Patch border (green) and subdivisions (blue), Normals (white: triangle, pink: vertex from map)";
      break;
  }

  displayText(ss1.str(), GLUT_BITMAP_9_BY_15);

  glRasterPos2f(30*dx-1.0f, 20*dy-1.0f);
  std::stringstream ss2;
  ss2 << "Duration=" << std::setw(4) << (iAfterTime-iBeforeTime) << "ms Patches sent=" << std::setw(7) << formatLongInt(iScene.getDrawnPatchNb()) << "/" << std::setw(7) << formatLongInt(iScene.getPatchNb()) << " Triangles=" << std::setw(10) << formatLongInt(iScene.getTriangleNb()) << " (discarded=" << std::setw(9) << formatLongInt(iScene.getDiscardedTriangleNb()) << ")";
  displayText(ss2.str(), GLUT_BITMAP_9_BY_15);

  float height = 70*dy;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
  glBegin(GL_QUADS);
  glVertex2f(-1.0f, -1.0f);
  glVertex2f( 1.0f, -1.0f);
  glVertex2f( 1.0f, -1.0f + height);
  glVertex2f(-1.0f, -1.0f + height);
  glEnd();
  __CheckGLErrors; 
  glDisable(GL_BLEND);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex2f(1.0f, -1.0f + height);
  glVertex2f(-1.0f, -1.0f + height);
  glEnd();
  __CheckGLErrors;
}

void displayHelp(const MxViewer& iViewer)
{
  float dx = 2.0f / iViewer.getViewport()[0];
  float dy = 2.0f / iViewer.getViewport()[1];

  // Display information about rendering

  glUseProgram(0);

  glColor3f(1.0f, 1.0f, 1.0f);
  glRasterPos2f(365*dx-1.0f, -250*dy+1.0f);
  displayText("KEY*", GLUT_BITMAP_TIMES_ROMAN_24);

  glRasterPos2f(750*dx-1.0f, -250*dy+1.0f);
  displayText("COMMAND", GLUT_BITMAP_TIMES_ROMAN_24);

  const std::string texts1[] = { "H", "Q", "+/-", "C", "A", "I", "F", "S", "W" };
  const std::string texts2[] = { "Show | Hide this help menu", "Quality of interpolation for position (linear, bicubic) and tangent (constant, linear, bicubic)", "Increase | Decrease tesselation factor based on height distortion",
                                 "Color Map (coloring terrain according a texture map)", "Animations: fly over, sunlight simulation, building terrain from bottom to top and vice versa",
                                 "Isoline display (different pre-defined values of heights)", "Replace Height Map with functional height (for debug)", "Shadow, alternative method to shadow mapping", "Show | Hide Wireframe representation: borders of patch (green) or/and normals" };
  for (uint32_t iLine = 0; iLine < SizeOfTable(texts1); iLine++)
  {
    glRasterPos2f(380 * dx - 1.0f, -40.0f*iLine*dy-330*dy+1.0f);
    displayText(texts1[iLine], GLUT_BITMAP_HELVETICA_18);

    glRasterPos2f(480 * dx - 1.0f, -40.0f*iLine*dy-330*dy+1.0f);
    displayText(texts2[iLine], GLUT_BITMAP_HELVETICA_18);
  }

  glRasterPos2f(360*dx-1.0f, -720*dy+1.0f);
  displayText("* repeat key stroke to go to the next circular choice", GLUT_BITMAP_HELVETICA_18);
  
  float deltaX = 180 * dx;
  float deltaY = 180 * dy;

  for (int i = 0; i < 2; i++)
  {
    if (i == 0)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(1.0f, 0.0f, 0.0f, 0.35f);
      glBegin(GL_QUADS);
    }
    else
    {
      glDisable(GL_BLEND);
      glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
    }

    glVertex2f(deltaX - 1.0f, deltaY +70*dy - 1.0f);
    glVertex2f(-deltaX + 1.0f, deltaY + 70 * dy - 1.0f);
    glVertex2f(-deltaX + 1.0f, -deltaY + 1.0f);
    glVertex2f(deltaX - 1.0f, -deltaY + 1.0f);
    glEnd();
    __CheckGLErrors;
  }
}

std::string formatLongInt(uint32_t iNumber)
{
  std::string nbstr = std::to_string(iNumber);
  std::string nbstrSep;
  uint32_t nb = 0;
  for (int32_t n = nbstr.length() - 1; n >= 0; n--)
  {
    nbstrSep.insert(nbstrSep.begin(), nbstr[n]);
    nb++;
    if (nb % 3 == 0 && n != 0)
      nbstrSep.insert(nbstrSep.begin(), ',');
  }

  return nbstrSep;
}

void displayText(const std::string& i2DText, void* iFont)
{
  int len = i2DText.length();
  for (int i = 0; i < len; i++)
  {
    glutBitmapCharacter(iFont, i2DText[i]);
  }
  __CheckGLErrors;
}

void onCharKeyPressed(GLFWwindow* window, unsigned int key)
{
  if (key == 'a' || key == 'A')
    ++gAnimationMode %= 5;
  else if (key == 'c' || key == 'C')
    ++gColorMode %= 2;
  else if (key == 'f' || key == 'F')
    ++gFunctionalMode %= 4;
  else if (key == 'h' || key == 'H')
    ++gDisplayHelp %= 2;
  else if (key == 'i' || key == 'I')
    ++gIsolineMode %= 6;
  else if (key == 'm' || key == 'M')
    ++gMapMode %= 3;
  else if (key == 'q' || key == 'Q')
    ++gSmoothMode %= 3;
  else if (key == 's' || key == 'S')
    ++gShadowMode %= 2;
  else if (key == 'w' || key == 'W')
    ++gWireframeMode %= 4;
  else if (key == '+')
    gDistortionFactor *= 1.1f;
  else if (key == '-' && gDistortionFactor > 1.0f)
    gDistortionFactor /= 1.1f;
}

float getIsolineStep(uint32_t iMode)
{
  float steps[] = { 5.0f, 10.0f, 50.0f };
  return steps[(gIsolineMode - 1) % SizeOfTable(steps)] * pow(10.0f, (float)((gIsolineMode - 1) / SizeOfTable(steps)));
}
