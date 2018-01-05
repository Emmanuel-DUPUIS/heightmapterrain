//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxHeightComputation.h"

#include "UxUtils.h"
#include "UxError.h"

void MxHeightComputation::getPixelNeighbour(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iU, float iV, GLsizei iSize, float iMin, float iMax, float* oPixels, float& oDu, float& oDv, int32_t& oUDirection, int32_t& oVDirection, bool& oUBorder, bool& oVBorder)
{
  float s = iU * (iWidth - 1);
  float t = iV * (iHeight - 1);

  // U and V may be reversed if uvOnPatch in last row or column of the patch matrix
  oUDirection = s < iWidth  - 2 ? 1 : -1; // u-direction of neighbour matrix
  oVDirection = t < iHeight - 2 ? 1 : -1; // v-direction of neighbour matrix

  oDu = s - (int)s;
  oDv = t - (int)t;

  // if iSize=2  oPixels[0][0]:=iPixels[iX][iY] , if iSize=4  oPixels[1][1]:=iPixels[iX][iY]
  int32_t pixelU = (uint32_t)s - oUDirection * iSize / 4;
  int32_t pixelV = (uint32_t)t - oVDirection * iSize / 4;

  // If direction reversed and if not nul u|v, parameter must be complemented and base pixel shifted
  if (oUDirection == -1 && oDu != 0.0f)
  {
    oDu = 1 - oDu;
    s++;
  }

  if (oVDirection == -1 && oDv != 0.0f)
  {
    oDv = 1 - oDv;
    t++;
  }

  oUBorder = s < 1 || s == iWidth - 1;
  oVBorder = t < 1 || t == iHeight - 1;

  // oPixels: Column-major order
  uint32_t index = 0;
  for (int32_t uIndex = 0; uIndex < iSize; uIndex++)
  {
    for (int32_t vIndex = 0; vIndex < iSize; vIndex++)
    {
      int32_t nU = pixelU + oUDirection*uIndex;
      int32_t nV = pixelV + oVDirection*vIndex;
      float t = 0.0f;
      if (nU >= 0 && nU < (int)iWidth && nV >= 0 && nV < (int)iHeight)
      {
        t = iPixels[3 * (nV*iWidth + nU)] / 255.0f;
        if (iMin >= 0 && t < iMin)
          t = iMin;
        if (iMax >= 0 && t > iMax)
          t = iMax;
      }
      oPixels[index++] = t;
    }
  }
}

void MxHeightComputation::getDerivatives(const float iPixels[4][4], bool iUBorder, bool iVBorder, float oXDerivatives[2][2], float oYDerivatives[2][2], float oXYDerivatives[2][2])
{
  if (iUBorder)
  {
    oXDerivatives[0][0] = iPixels[2][1] - iPixels[1][1];
    oXDerivatives[0][1] = iPixels[2][2] - iPixels[1][2];
  }
  else
  {
    oXDerivatives[0][0] = 0.5f * (iPixels[2][1] - iPixels[0][1]);
    oXDerivatives[0][1] = 0.5f * (iPixels[2][2] - iPixels[0][2]);
  }

  oXDerivatives[1][0] = 0.5f * (iPixels[3][1] - iPixels[1][1]);
  oXDerivatives[1][1] = 0.5f * (iPixels[3][2] - iPixels[1][2]);

  if (iVBorder)
  {
    oYDerivatives[0][0] = iPixels[1][2] - iPixels[1][1];
    oYDerivatives[1][0] = iPixels[2][2] - iPixels[2][1];
  }
  else
  {
    oYDerivatives[0][0] = 0.5f * (iPixels[1][2] - iPixels[1][0]);
    oYDerivatives[1][0] = 0.5f * (iPixels[2][2] - iPixels[2][0]);
  }

  oYDerivatives[0][1] = 0.5f * (iPixels[1][3] - iPixels[1][1]);
  oYDerivatives[1][1] = 0.5f * (iPixels[2][3] - iPixels[2][1]);

  if (iUBorder && iVBorder)
    oXYDerivatives[0][0] = oXDerivatives[0][1] - iPixels[2][1] + iPixels[1][1];
  else if (iUBorder)
    oXYDerivatives[0][0] = 0.5f * (oXDerivatives[0][1] - iPixels[2][0] - iPixels[1][0]);
  else if (iVBorder)
    oXYDerivatives[0][0] = oXDerivatives[0][1] - 0.5f * (iPixels[2][1] - iPixels[0][1]);
  else
    oXYDerivatives[0][0] = 0.5f * oXDerivatives[0][1] - 0.25f * (iPixels[2][0] - iPixels[0][0]);

  if (iUBorder)
    oXYDerivatives[0][1] = oYDerivatives[1][1] - 0.5f * (iPixels[1][3] + iPixels[1][1]);
  else
    oXYDerivatives[0][1] = 0.5f * oYDerivatives[1][1] - 0.25f * (iPixels[0][3] + iPixels[0][1]);

  if (iVBorder)
    oXYDerivatives[1][0] = oXDerivatives[1][1] - 0.5f * (iPixels[3][1] - iPixels[1][1]);
  else
    oXYDerivatives[1][0] = 0.5f * oXDerivatives[1][1] - 0.25f * (iPixels[3][0] - iPixels[1][0]);

  oXYDerivatives[1][1] = 0.25f * (iPixels[3][3] - iPixels[1][3]) - 0.5f * oXDerivatives[1][0];
}

float MxHeightComputation::getLinearHeight(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iU, float iV, float iMin, float iMax)
{
  float du, dv;
  bool uBorder, vBorder;
  int32_t uDirection, vDirection;
  float pixels[2][2];
  getPixelNeighbour(iPixels, iWidth, iHeight, iU, iV, 2, iMin, iMax, (float*)pixels, du, dv, uDirection, vDirection, uBorder, vBorder);

  return (1 - dv)*((1 - du)*pixels[0][0] + du*pixels[1][0]) + dv*((1 - du)*pixels[0][1] + du*pixels[1][1]);
}

float MxHeightComputation::getBicubicHeight(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iU, float iV, float iMin, float iMax)
{
  float du, dv;
  int32_t uDirection, vDirection;
  bool uBorder, vBorder;
  float pixels[4][4];
  getPixelNeighbour(iPixels, iWidth, iHeight, iU, iV, 4, iMin, iMax, (float*)pixels, du, dv, uDirection, vDirection, uBorder, vBorder);

  float xDerivatives[2][2], yDerivatives[2][2], xyDerivatives[2][2];
  getDerivatives(pixels, uBorder, vBorder, xDerivatives, yDerivatives, xyDerivatives);

  float heights[2][2] = { { pixels[1][1], pixels[1][2] },{ pixels[2][1], pixels[2][2] } };
  float coefficients[4][4];
  bicubicInterpolation(heights, xDerivatives, yDerivatives, xyDerivatives, coefficients);
    
  return bicubicEvaluation(coefficients, du, dv);
}

float MxHeightComputation::getHeight(const GLubyte* iPixels, uint32_t iWidth, uint32_t iHeight, float iFunctional, uint16_t iSmoothInterpolation, float iU, float iV, float iHeightFactor, float iMin, float iMax)
{
  if (iFunctional == 0.0)
  {
    float height;
    if (iSmoothInterpolation < 2)
      height = getLinearHeight(iPixels, iWidth, iHeight, iU, iV, iMin, iMax);
    else
      height = getBicubicHeight(iPixels, iWidth, iHeight, iU, iV, iMin, iMax);
      
    return iHeightFactor * height;
  }

  // functional height (f(u,v)={u,v,u(1-u)*v(1-v)}
  return iFunctional * iU * (1 - iU) * iV * (1 - iV);
}

float MxHeightComputation::bicubicEvaluation(const float iCoefficients[4][4], float iX, float iY)
{
  // Evaluation of the polynomial function
  float x2 = iX*iX;
  float x[4] = { 1.0f, iX, x2, iX*x2 };
  float y2 = iY*iY;
  float y[4] = { 1.0f, iY, y2, iY*y2 };

  float t = 0.0f;
  for (uint32_t i = 0; i < 4; i++)
    for (uint32_t j = 0; j < 4; j++)
      t += iCoefficients[i][j] * x[i] * y[j];
  return t;
}

void MxHeightComputation::bicubicDerivateEvaluation(const float iCoefficients[4][4], float iX, float iY, float& oXDerivate, float& oYDerivate, float& oXYDerivate)
{
  // Evaluation of the 3 derivatives of the polynomial function (i.e. polynomial functions)
  
  float x2 = iX*iX;
  float x[4] = { 1.0f, iX, x2, iX*x2 };
  float y2 = iY*iY;
  float y[4] = { 1.0f, iY, y2, iY*y2 };

  oXDerivate = 0.0f;
  oYDerivate = 0.0f;
  oXYDerivate = 0.0f;

  for (uint32_t i = 0; i < 4; i++)
    for (uint32_t j = 0; j < 4; j++)
    {
      if (i != 0)
        oXDerivate += i*iCoefficients[i][j] * x[i - 1] * y[j];
      if (j != 0)
        oYDerivate += j*iCoefficients[i][j] * x[i] * y[j - 1];
      if (i != 0 && j != 0)
        oXYDerivate += i*j*iCoefficients[i][j] * x[i - 1] * y[j - 1];
    }
}

void MxHeightComputation::bicubicInterpolation(const float iHeights[2][2], const float iXDerivatives[2][2], const float iYDerivatives[2][2], const float iXYDerivatives[2][2], float oCoefficients[4][4])
{
  // 16 coefficients calculation (solution of the linear system, cf. bicubic_interpolation.pdf to the formal matter
  oCoefficients[0][0] = iHeights[0][0];
  oCoefficients[1][0] = iXDerivatives[0][0];
  oCoefficients[0][1] = iYDerivatives[0][0];
  oCoefficients[1][1] = iXYDerivatives[0][0];

  float dt = iHeights[0][1] - iHeights[0][0];
  oCoefficients[0][2] = 3 * dt - 2 * iYDerivatives[0][0] - iYDerivatives[0][1];
  oCoefficients[0][3] = -2 * dt + iYDerivatives[0][0] + iYDerivatives[0][1];

  dt = iHeights[1][0] - iHeights[0][0];
  oCoefficients[2][0] = 3 * dt - 2 * iXDerivatives[0][0] - iXDerivatives[1][0];
  oCoefficients[3][0] = -2 * dt + iXDerivatives[0][0] + iXDerivatives[1][0];

  dt = iXDerivatives[0][1] - iXDerivatives[0][0];
  oCoefficients[1][2] = 3 * dt - 2 * iXYDerivatives[0][0] - iXYDerivatives[0][1];
  oCoefficients[1][3] = -2 * dt + iXYDerivatives[0][0] + iXYDerivatives[0][1];
  
  dt = iYDerivatives[1][0] - iYDerivatives[0][0];
  oCoefficients[2][1] = 3 * dt - 2 * iXYDerivatives[0][0] - iXYDerivatives[1][0];
  oCoefficients[3][1] = -2 * dt + iXYDerivatives[0][0] + iXYDerivatives[1][0];

  float c0 = iHeights[1][1];
  for (uint32_t i = 0; i < 4; i++)
    for (uint32_t j = 0; j < 4 - 2 * (i / 2); j++)
      c0 -= oCoefficients[i][j];

  float c1 = iXDerivatives[1][1];
  for (uint32_t i = 1; i < 4; i++)
    for (uint32_t j = 0; j < 4 - 2 * (i / 2); j++)
      c1 -= i*oCoefficients[i][j];

  float c2 = iYDerivatives[1][1];
  for (uint32_t i = 0; i < 4; i++)
    for (uint32_t j = 1; j < 4 - 2 * (i / 2); j++)
      c2 -= j*oCoefficients[i][j];

  float c3 = iXYDerivatives[1][1];
  for (uint32_t i = 1; i < 4; i++)
    for (uint32_t j = 1; j < 4 - 2 * (i / 2); j++)
      c3 -= i*j*oCoefficients[i][j];

  oCoefficients[2][2] =  9 * c0 - 3 * c1 - 3 * c2 + c3;
  oCoefficients[2][3] = -6 * c0 + 2 * c1 + 3 * c2 - c3;
  oCoefficients[3][2] = -6 * c0 + 3 * c1 + 2 * c2 - c3;
  oCoefficients[3][3] =  4 * c0 - 2 * c1 - 2 * c2 + c3;

  #ifdef __CheckCodeValidity
    // Verifies coeeficients validity according corner constraints
    float corners[4][2] = { {0.0f,0.0f},{ 1.0f,0.0f },{ 0.0f,1.0f },{ 1.0f,1.0f } };
    for (uint32_t iCorner = 0; iCorner < 4; iCorner++)
    {
      float h = bicubicEvaluation(oCoefficients, corners[iCorner][0], corners[iCorner][1]);
      __AssertIfNot(UxUtils::deviation(h, iHeights[(int)corners[iCorner][0]][(int)corners[iCorner][1]], 0.0001f), "Invalid coefficients for bicubic interpolation");
      float dx, dy, dxdy;
      bicubicDerivateEvaluation(oCoefficients, corners[iCorner][0], corners[iCorner][1], dx, dy, dxdy);
      __AssertIfNot(UxUtils::deviation(iXDerivatives[(int)corners[iCorner][0]][(int)corners[iCorner][1]], dx, 0.001f), "Invalid coefficients for bicubic interpolation");
      __AssertIfNot(UxUtils::deviation(iYDerivatives[(int)corners[iCorner][0]][(int)corners[iCorner][1]], dy, 0.001f), "Invalid coefficients for bicubic interpolation");
      __AssertIfNot(UxUtils::deviation(iXYDerivatives[(int)corners[iCorner][0]][(int)corners[iCorner][1]], dxdy, 0.001f), "Invalid coefficients for bicubic interpolation");
    }
  #endif
}
