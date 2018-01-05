//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include <GL/glew.h>
#include <vmath.h>

#define SizeOfTable(table)  (sizeof(table)/sizeof(table[0]))

class UxUtils
{
public:

  //===============================================================================
  //   Cofactor matrix (or matrix of the minors) of a 3x3 matrix
  //     Used especially to determine inverse matrix (cf. Matrix3<T>::inverse),
  //     but also in case of non-isometric matrix transformation (where cross
  //     product is not distribute over this matrix transformation, i.e.
  //     (M.V1)x(M.V2) = Cofactor(M).(V1xV2) <> M.(V1xV2)).
  //     For Matrix4f, the cofactor are only computed on 3x3 sub-matrix 
  //     (translation is left unchnaged).
  //===============================================================================

  static Matrix3f cofactor(Matrix3f m);
  static Matrix4f cofactor(Matrix4f m);

  static Vector3f rotatePoint(const Vector3f& iPointToRotate, const Vector3f& iPointOnAxis, const Vector3f& iAxisDirection, float iRadAngle);
  static Vector3f rotateVector(const Vector3f& iVectorToRotate, const Vector3f& iAxisDirection, float iRadAngle);

  static void createBindlessTexture(const std::string& iFilePath, GLuint& oTextureName, GLuint64& oTextureHandle, bool iMakeResident = true);

  static std::string GLSLTypeToCPlusPlus(const char* iDeclaration);

  static bool deviation(float iRefValue, float iComputedValue, float iRatio);
};