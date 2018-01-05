//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include <stdint.h>
#include <gl/glew.h>

//========================================================================
//  Height Map Computation:
//    Numerical algorithms to compute heigth, derivatives, normals based
//    on height map texture.
//    Used on CPU side to make draw pre-filter (filtering elements sent 
//    to the pipeline). Redundant with glsl routines in
//    tx_mapcomputing.glsl and therefore source of discrepancies.
//    See bicubic_interpolation.pdf to the formal matter.
//========================================================================

class MxHeightComputation
{
public:

  static void getPixelNeighbour(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iU, float iV, GLsizei iSize, float iMin, float iMax, float* oPixels, float& oDu, float& oDv, int32_t& oUDirection, int32_t& oVDirection, bool& oUBorder, bool& oVBorder);
  static void getDerivatives(const float iPixels[4][4], bool iUBorder, bool iVBorder, float oXDerivatives[2][2], float oYDerivatives[2][2], float oXYDerivatives[2][2]);

  static float getLinearHeight(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iU, float iV, float iMin, float iMax);
  static float getBicubicHeight(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iU, float iV, float iMin, float iMax);
  static float getHeight(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iFunctional, uint16_t iSmoothInterpolation, float iU, float iV, float iHeightFactor, float iMin, float iMax);

  static float bicubicEvaluation(const float iCoefficients[4][4], float iX, float iY);
  static void  bicubicDerivateEvaluation(const float iCoefficients[4][4], float iX, float iY, float& oXDerivate, float& oYDerivate, float& oXYDerivate);
  static void  bicubicInterpolation(const float iHeights[2][2], const float iXDerivatives[2][2], const float iYDerivatives[2][2], const float iXYDerivatives[2][2], float oCoefficients[4][4]);
};
