//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxUtils.h"

#include "UxError.h"

#include <regex>
#include <cassert>
#include <IL/il.h>
//#include <IL/ilut.h>

Matrix3f UxUtils::cofactor(Matrix3f m)
{
  Matrix3f result;

  result.at(0, 0) = m.at(1, 1) * m.at(2, 2) - m.at(2, 1) * m.at(1, 2);
  result.at(0, 1) = m.at(2, 1) * m.at(0, 2) - m.at(0, 1) * m.at(2, 2);
  result.at(0, 2) = m.at(0, 1) * m.at(1, 2) - m.at(1, 1) * m.at(0, 2);
  result.at(1, 0) = m.at(2, 0) * m.at(1, 2) - m.at(1, 0) * m.at(2, 2);
  result.at(1, 1) = m.at(0, 0) * m.at(2, 2) - m.at(2, 0) * m.at(0, 2);
  result.at(1, 2) = m.at(1, 0) * m.at(0, 2) - m.at(0, 0) * m.at(1, 2);
  result.at(2, 0) = m.at(1, 0) * m.at(2, 1) - m.at(2, 0) * m.at(1, 1);
  result.at(2, 1) = m.at(2, 0) * m.at(0, 1) - m.at(0, 0) * m.at(2, 1);
  result.at(2, 2) = m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
  return result;
}

Matrix4f UxUtils::cofactor(Matrix4f m)
{
  Matrix4f result;

  result.at(0, 0) = m.at(1, 1) * m.at(2, 2) - m.at(2, 1) * m.at(1, 2);
  result.at(0, 1) = m.at(2, 1) * m.at(0, 2) - m.at(0, 1) * m.at(2, 2);
  result.at(0, 2) = m.at(0, 1) * m.at(1, 2) - m.at(1, 1) * m.at(0, 2);
  result.at(1, 0) = m.at(2, 0) * m.at(1, 2) - m.at(1, 0) * m.at(2, 2);
  result.at(1, 1) = m.at(0, 0) * m.at(2, 2) - m.at(2, 0) * m.at(0, 2);
  result.at(1, 2) = m.at(1, 0) * m.at(0, 2) - m.at(0, 0) * m.at(1, 2);
  result.at(2, 0) = m.at(1, 0) * m.at(2, 1) - m.at(2, 0) * m.at(1, 1);
  result.at(2, 1) = m.at(2, 0) * m.at(0, 1) - m.at(0, 0) * m.at(2, 1);
  result.at(2, 2) = m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
  result.at(0, 3) = m.at(0, 3);
  result.at(1, 3) = m.at(1, 3); 
  result.at(2, 3) = m.at(2, 3); 
  result.at(3, 0) = m.at(3, 0);
  result.at(3, 1) = m.at(3, 1);
  result.at(3, 2) = m.at(3, 2);
  result.at(3, 3) = 1.f;
  return result;
}


Vector3f UxUtils::rotatePoint(const Vector3f& iPointToRotate, const Vector3f& iPointOnAxis, const Vector3f& iAxisDirection, float iRadAngle)
{
  Vector3f dir = iAxisDirection;
  dir.normalize();
  // Vector from pointOnAxis to pointToRotate (link)
  Vector3f l = iPointToRotate - iPointOnAxis;
  // Vector from pointOnAxis to the projection of pointToRotate onto the axis (projection)
  Vector3f s = dir * l.dotProduct(dir);
  // Vector from the projection of pointToRotate onto the axis to the pointToRotate (distance)
  Vector3f d = l - s;
  return iPointOnAxis + s + d*cos(iRadAngle) + dir.crossProduct(d)*sin(iRadAngle);
}

Vector3f UxUtils::rotateVector(const Vector3f& iVectorToRotate, const Vector3f& iAxisDirection, float iRadAngle)
{
  Vector3f dir = iAxisDirection;
  dir.normalize();
  // Vector from pointOnAxis to the projection of pointToRotate onto the axis (projection)
  Vector3f s = dir * iVectorToRotate.dotProduct(dir);
  // Vector from the projection of pointToRotate onto the axis to the pointToRotate (distance)
  Vector3f d = iVectorToRotate - s;
  return s + d*cos(iRadAngle) + dir.crossProduct(d)*sin(iRadAngle);
}

void UxUtils::createBindlessTexture(const std::string& iFilePath, GLuint& oTextureName, GLuint64& oTextureHandle, bool iMakeResident)
{
  ILubyte *buffer       = nullptr;
  ILuint   bufferLength = 0;
  FILE*    fp           = nullptr;

  /*assert(!fopen_s(&fp, filePath, "rb"));

  fseek(fp, 0, SEEK_END);
  bufferLength = ftell(fp);

  buffer = (ILubyte*)malloc(bufferLength);
  fseek(fp, 0, SEEK_SET);
  fread(buffer, 1, bufferLength, fp);
  fclose(fp);

  ilLoadL(IL_PNG, buffer, bufferLength);
  free(buffer);
  buffer = nullptr;
  */

  ilLoadImage((const wchar_t*)iFilePath.c_str());

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  __CheckGLErrors;

  glGenTextures(1, &oTextureName);
  glBindTexture(GL_TEXTURE_2D, oTextureName);
  
  uint32_t width  = ilGetInteger(IL_IMAGE_WIDTH);
  uint32_t height = ilGetInteger(IL_IMAGE_HEIGHT);
  uint32_t fmt    = ilGetInteger(IL_IMAGE_FORMAT);
  uint32_t type   = ilGetInteger(IL_IMAGE_TYPE);
  ILubyte* data   = ilGetData();

  //glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16, width, height);
  //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, fmt, type, data);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, type, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
  __CheckGLErrors;

  oTextureHandle = glGetTextureHandleARB(oTextureName);
  __CheckGLErrors;
  if (iMakeResident)
  {
    glMakeTextureHandleResidentARB(oTextureHandle);
    __CheckGLErrors;
  }
}


std::string UxUtils::GLSLTypeToCPlusPlus(const char* iDeclaration)
{
  static const char *table[][2] = { {"Vector([234])f", "vec$1"}, { "Vector([234])([diu])", "$2vec$1" }, { "Matrix([34])f", "mat$1"}, { "Matrix([34])d", "dmat$1" }, { "uint32_t", "uint" }, { "int32_t", "int" } };

  for (auto couple : table)
  {
    if (std::regex_search(iDeclaration, std::regex(couple[0])))
      return std::regex_replace(iDeclaration, std::regex(couple[0]), couple[1]);
  }

  return iDeclaration;
}


bool UxUtils::deviation(float iRefValue, float iComputedValue, float iRatio)
{
  return fabs(iComputedValue - iRefValue) < iRatio;
}
