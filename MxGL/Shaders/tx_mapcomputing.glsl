//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Numerical algorithms to compute heigth, derivatives, normals based
//  on height map texture (linear and bicup interpolation, see 
//  bicubic_interpolation.pdf to the formal matter.
//========================================================================

const ivec2 mapSize = textureSize(u_HeightMap.heightTexture, 0);

// mapCoordinates: 2D coordinates (s,t) on the texture map (each between 0 and 1)
// u,v : 2D coordinates inside a single patch (each between 0 and 1), the 4 limits corresponding to the 4 corners
//       used for linear/bicubic interoplation

//=================================================================================
//                    Fetch texel, Interpolation, Neighbourg
//=================================================================================

// Textel fetching integrating operation like ratio, limitations...
float getTexel(sampler2D heightMap, ivec2 mapCoordinates, int lod)
{
  float value = texelFetch(heightMap, mapCoordinates, lod).r;

  if (u_HeightMap.minHeight >= 0)
  {
    // From bottom to top
    if (value < u_HeightMap.minHeight)
      return u_HeightMap.minHeight;
  }

  if (u_HeightMap.maxHeight >= 0)
  {
    // From top to bottom
    if (value >  u_HeightMap.maxHeight)
      return u_HeightMap.maxHeight;
  }

  return value;
}

// Size=2, 4 surrounding texel mapCoordinates position (pxiels[0][0] = preceding in u and v textel)
// Size=4, idem as above plus 8 other  texels forming a greek cross (pxiels[1][1] = preceding in u and v textel)
void getNeighbour(vec2 mapCoordinates, uint size, out float u, out float v, out uint sBorder, out uint tBorder, out float pxiels[4][4], out int sDirection, out int tDirection)
{
  vec2  texelPos   = mapCoordinates * (mapSize - 1);
  ivec2 texelIndex = ivec2(int(texelPos.x), int(texelPos.y));

  u = fract(texelPos.x);
  v = fract(texelPos.y);

  // S and T may be reversed if mapCoordinates in last row or column of the patch matrix
  sDirection = int(sign(mapSize.x - 2.5 - texelIndex.x));  // u-direction of neighbour matrix
  tDirection = int(sign(mapSize.y - 2.5 - texelIndex.y));  // v-direction of neighbour matrix

  sBorder = 1 - sign((sDirection + 1)*texelIndex.x); // 1: s border patch, 0: else where
  tBorder = 1 - sign((tDirection + 1)*texelIndex.y); // 1: t border patch, 0: else where

  // If direction reversed, if not nul u|v parameter must be complemented
  u = (sDirection*(2 * u - 1) + 1) / 2;
  v = (tDirection*(2 * v - 1) + 1) / 2;

  // if u or v transformed from 0 to 1
  texelIndex.x -= int(u - fract(u));
  texelIndex.y -= int(v - fract(v));
  u = fract(u);
  v = fract(v);

  int sIndex1 = (1 - sDirection) / 2;
  int sIndex2 = sIndex1 + sDirection;
  
  int tIndex1 = (1 - tDirection) / 2;
  int tIndex2 = tIndex1 + tDirection;

  uint indexBase = size / 4; // 0 for size=2, 1 for size=4 
  pxiels[indexBase][indexBase]     = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex1, tIndex1), 0);
  pxiels[indexBase][indexBase+1]   = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex1, tIndex2), 0);
  pxiels[indexBase+1][indexBase]   = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex2, tIndex1), 0);
  pxiels[indexBase+1][indexBase+1] = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex2, tIndex2), 0);

  if (size == 4)
  {
    int sIndex0 = sIndex1 - sDirection;
    int sIndex3 = sIndex2 + sDirection;

    int tIndex0 = tIndex1 - tDirection;
    int tIndex3 = tIndex2 + tDirection;

    pxiels[0][0] = (1 - sBorder)*(1 - tBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex0, tIndex0), 0);
    pxiels[0][1] = (1 - sBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex0, tIndex1), 0);
    pxiels[1][0] = (1 - tBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex1, tIndex0), 0);
    pxiels[0][2] = (1 - sBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex0, tIndex2), 0);
    pxiels[2][0] = (1 - tBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex2, tIndex0), 0);
    pxiels[0][3] = (1 - sBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex0, sIndex3), 0);
    pxiels[3][0] = (1 - tBorder)*getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex3, sIndex0), 0);

    pxiels[3][1] = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex3, tIndex1), 0);
    pxiels[3][2] = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex3, tIndex2), 0);
    pxiels[1][3] = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex1, tIndex3), 0);
    pxiels[2][3] = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex2, tIndex3), 0);
    pxiels[3][3] = getTexel(u_HeightMap.heightTexture, texelIndex + ivec2(sIndex3, tIndex3), 0);
  }
}

void getNeighbourAndDerivates(vec2 mapCoordinates, out float u, out float v, out float pixels[4][4], out float xDerivatives[2][2], out float yDerivatives[2][2], out float xyDerivatives[2][2], out int sDirection, out int tDirection)
{
  uint sBorder, tBorder;
  getNeighbour(mapCoordinates, 4, u, v, sBorder, tBorder, pixels, sDirection, tDirection);

  // First partial derivative with respect to first parameter at four corners
  xDerivatives[0][0] = (float(sBorder+1)/2)*(pixels[2][1] - mix(pixels[0][1], pixels[1][1], sBorder));
  xDerivatives[0][1] = (float(sBorder+1)/2)*(pixels[2][2] - mix(pixels[0][2], pixels[1][2], sBorder));
  xDerivatives[1][0] = 0.5*(pixels[3][1] - pixels[1][1]);
  xDerivatives[1][1] = 0.5*(pixels[3][2] - pixels[1][2]);

  // First partial derivative with respect to second parameter at four corners
  yDerivatives[0][0] = (float(tBorder+1)/2)*(pixels[1][2] - mix(pixels[1][0], pixels[1][1], tBorder));
  yDerivatives[1][0] = (float(tBorder+1)/2)*(pixels[2][2] - mix(pixels[2][0], pixels[2][1], tBorder));
  yDerivatives[0][1] = 0.5*(pixels[1][3] - pixels[1][1]);
  yDerivatives[1][1] = 0.5*(pixels[2][3] - pixels[2][1]);

  // Mixed second order derivative at four corners
  xyDerivatives[0][0] = (float(tBorder+1)/2)*(xDerivatives[0][1] - (float(sBorder+1)/2)*(mix(pixels[2][0], pixels[2][1], tBorder)-mix(mix(pixels[0][0], pixels[0][1], tBorder), mix(pixels[1][0], pixels[1][1], tBorder), sBorder)));
  xyDerivatives[0][1] = (float(sBorder+1)/2)*(yDerivatives[1][1] - 0.5*(mix(pixels[0][3] - pixels[0][1], pixels[1][3] - pixels[1][1], sBorder)));
  xyDerivatives[1][0] = (float(tBorder+1)/2)*(xDerivatives[1][1] - 0.5*(mix(pixels[3][0] - pixels[1][0], pixels[3][1] - pixels[1][1], tBorder)));
  xyDerivatives[1][1] = 0.25*(pixels[3][3] - pixels[1][3]) - 0.5*xDerivatives[1][0];
}


//=================================================================================
//                              Constant Normals
//=================================================================================

vec2 getConstantFirstDerivatives(vec2 mapCoordinates)
{
  uint sBorder, tBorder;
  int sDirection, tDirection;
  float u, v, pixels[4][4];
  getNeighbour(mapCoordinates, 2, u, v, sBorder, tBorder, pixels, sDirection, tDirection);
  
  float du = sDirection*mix(pixels[1][1] - pixels[0][1], pixels[1][0] - pixels[0][0], 0.5);
  float dv = tDirection*mix(pixels[1][1] - pixels[1][0], pixels[0][1] - pixels[0][0], 0.5);

  vec4 dfu = u_Positionning.model * vec4(float(u_HeightMap.terrainSubdivision.x) / float(mapSize.x - 1), 0, u_HeightMap.heightFactor*du, 0);
  // Minus sign for deltaHeightV since Y (coord) and U (texel) are opposite
  vec4 dfv = u_Positionning.model * vec4(0, float(u_HeightMap.terrainSubdivision.y) / float(mapSize.y - 1), -u_HeightMap.heightFactor*dv, 0);

  return vec2(dfu.z / dfu.x, dfv.z / dfv.y);
}

vec3 getConstantNormal(vec2 mapCoordinates)
{
  vec2 df = getConstantFirstDerivatives(mapCoordinates);
  return normalize(vec3(-df.x, -df.y, 1));
}

vec2 getConstantGradient(vec2 mapCoordinates)
{
  return getConstantFirstDerivatives(mapCoordinates);
}


//=================================================================================
//                              Linear Interpolation
//=================================================================================

// Linear bi-dimentionnal height interpolation
float getLinearInterpolationHeight(vec2 mapCoordinates)
{
  uint sBorder, tBorder;
  int sDirection, tDirection;
  float u, v, pixels[4][4];

  getNeighbour(mapCoordinates, 2, u, v, sBorder, tBorder, pixels, sDirection, tDirection);
  return mix(mix(pixels[0][0], pixels[1][0], u), mix(pixels[0][1], pixels[1][1], u), v);
}

vec2 getLinearFirstDerivatives(vec2 mapCoordinates)
{
  int sDirection, tDirection;
  float u, v, pixels[4][4], xDerivatives[2][2], yDerivatives[2][2], xyDerivatives[2][2];

  getNeighbourAndDerivates(mapCoordinates, u, v, pixels, xDerivatives, yDerivatives, xyDerivatives, sDirection, tDirection);
  
  float du1 = mix(xDerivatives[0][0], xDerivatives[1][0], u);
  float du2 = mix(xDerivatives[0][1], xDerivatives[1][1], u);
  float du  = sDirection*mix(du1, du2, v);

  float dv1 = mix(yDerivatives[0][0], yDerivatives[0][1], v);
  float dv2 = mix(yDerivatives[1][0], yDerivatives[1][1], v);
  float dv  = tDirection*mix(dv1, dv2, u);

  vec4 dfu = u_Positionning.model * vec4(float(u_HeightMap.terrainSubdivision.x) / float(mapSize.x-1), 0, u_HeightMap.heightFactor * du, 0);
  // Minus sign for deltaHeightV since Y (coord) and U (texel) are opposite
  vec4 dfv = u_Positionning.model * vec4(0, float(u_HeightMap.terrainSubdivision.y) / float(mapSize.y-1), -u_HeightMap.heightFactor * dv, 0);

  return vec2(dfu.z / dfu.x, dfv.z / dfv.y);
}

vec3 getLinearInterpolationNormal(vec2 mapCoordinates)
{
  // Normal to f(u,v)=height, estimated by cross(df/du, df/dv)  with df/du={du,0,dfu} and df/dv={0,dv,dfv}
  // thus normal={-dv.dfu,-du.dfv,du.dv} or {-dfu/du,-dfv/dv,1}, since we want a normalized vector.
  // But, since the model transformation is NOT isometric (i.e. not the same scale values in every direction)
  // the cross product must be down after the transformation application (and not the other way round)
  vec2 df = getLinearFirstDerivatives(mapCoordinates);
  return normalize(vec3(-df.x, -df.y, 1));
}

vec2 getLinearInterpolationGradient(vec2 mapCoordinates)
{
  return getLinearFirstDerivatives(mapCoordinates);
}


//=================================================================================
//                                       Bicubic
//=================================================================================

float bicubicEvaluation(float coefficients[4][4], float iX, float iY)
{
  float x2 = iX*iX;
  float x[4] = { 1, iX, x2, iX*x2 };
  float y2 = iY*iY;
  float y[4] = { 1, iY, y2, iY*y2 };

  float t = 0;
  for (uint i = 0; i < 4; i++)
    for (uint j = 0; j < 4; j++)
      t += coefficients[i][j] * x[i] * y[j];
  return t;
}

void bicubicDerivateEvaluation(float coefficients[4][4], float iX, float iY, out float xDerivate, out float yDerivate, out float xyDerivate)
{
  float x2 = iX*iX;
  float x[4] = { 1, iX, x2, iX*x2 };
  float y2 = iY*iY;
  float y[4] = { 1, iY, y2, iY*y2 };

  xDerivate = yDerivate = xyDerivate = 0;

  for (uint i = 0; i < 4; i++)
    for (uint j = 0; j < 4; j++)
    {
      float it = i*coefficients[i][j] * x[i - 1];
      float jt = j*y[j - 1];

      xDerivate  += it * y[j];
      yDerivate  += jt*coefficients[i][j] * x[i];
      xyDerivate += it*jt;
    }
}

void bicubicInterpolation(float pixels[2][2], float xDerivate[2][2], float yDerivate[2][2], float xyDerivate[2][2], out float coefficients[4][4])
{
  coefficients[0][0] = pixels[0][0];
  coefficients[1][0] = xDerivate[0][0];
  coefficients[0][1] = yDerivate[0][0];
  coefficients[1][1] = xyDerivate[0][0];

  float dt = pixels[0][1] - pixels[0][0];
  coefficients[0][2] = 3 * dt - 2 * yDerivate[0][0] - yDerivate[0][1];
  coefficients[0][3] = -2 * dt + yDerivate[0][0] + yDerivate[0][1];

  dt = pixels[1][0] - pixels[0][0];
  coefficients[2][0] = 3 * dt - 2 * xDerivate[0][0] - xDerivate[1][0];
  coefficients[3][0] = -2 * dt + xDerivate[0][0] + xDerivate[1][0];

  dt = xDerivate[0][1] - xDerivate[0][0];
  coefficients[1][2] = 3 * dt - 2 * xyDerivate[0][0] - xyDerivate[0][1];
  coefficients[1][3] = -2 * dt + xyDerivate[0][0] + xyDerivate[0][1];

  dt = yDerivate[1][0] - yDerivate[0][0];
  coefficients[2][1] = 3 * dt - 2 * xyDerivate[0][0] - xyDerivate[1][0];
  coefficients[3][1] = -2 * dt + xyDerivate[0][0] + xyDerivate[1][0];

  float c0 = pixels[1][1];
  float c1 = xDerivate[1][1];
  float c2 = yDerivate[1][1];
  float c3 = xyDerivate[1][1];
  for (uint i = 0; i < 4; i++)
  {
    for (uint j = 0; j < 4 - 2 * (i / 2); j++)
    {
      c0 -= coefficients[i][j];
      float t = i*coefficients[i][j];
      c1 -= t;
      c2 -= j*coefficients[i][j];
      c3 -= j*t;
    }
  }

  float t1 = c1 + c2;
  float t2 = -6 * c0 + 2 * t1 - c3;
  coefficients[2][2] = 9 * c0 - 3 * t1 + c3;
  coefficients[2][3] = t2 + c2;
  coefficients[3][2] = t2 + c1;
  coefficients[3][3] = 4 * c0 - 2 * t1 + c3;
}

// Bicubic interpolation
float getBicubicInterpolationHeight(vec2 mapCoordinates)
{
  int sDirection, tDirection;
  float u, v, pixels[4][4], xDerivatives[2][2], yDerivatives[2][2], xyDerivatives[2][2];
  getNeighbourAndDerivates(mapCoordinates, u, v, pixels, xDerivatives, yDerivatives, xyDerivatives, sDirection, tDirection);

  float heights[2][2] = { { pixels[1][1], pixels[1][2] },{ pixels[2][1], pixels[2][2] } };
  float coefficients[4][4];
  bicubicInterpolation(heights, xDerivatives, yDerivatives, xyDerivatives, coefficients);

  return bicubicEvaluation(coefficients, u, v);
}

vec2 getBicubicFirstDerivatives(vec2 mapCoordinates)
{
  int sDirection, tDirection;
  float u, v, pixels[4][4], xDerivatives[2][2], yDerivatives[2][2], xyDerivatives[2][2];
  getNeighbourAndDerivates(mapCoordinates, u, v, pixels, xDerivatives, yDerivatives, xyDerivatives, sDirection, tDirection);

  float heights[2][2] = { { pixels[1][1], pixels[1][2] },{ pixels[2][1], pixels[2][2] } };
  float coefficients[4][4];
  bicubicInterpolation(heights, xDerivatives, yDerivatives, xyDerivatives, coefficients);

  float xDerivate, yDerivate, xyDerivate;
  bicubicDerivateEvaluation(coefficients, u, v, xDerivate, yDerivate, xyDerivate);

  vec3 dfu = (u_Positionning.model * vec4(float(u_HeightMap.terrainSubdivision.x) / float(mapSize.x - 1), 0, u_HeightMap.heightFactor * sDirection * xDerivate, 0)).xyz;
  // Minus sign for deltaHeightV since Y (coord) and U (texel) are opposite
  vec3 dfv = (u_Positionning.model * vec4(0, float(u_HeightMap.terrainSubdivision.y) / float(mapSize.y - 1), -u_HeightMap.heightFactor * tDirection * yDerivate, 0)).xyz;

  return vec2(dfu.z / dfu.x, dfv.z / dfv.y);
}

vec3 getBicubicInterpolationNormal(vec2 mapCoordinates)
{
  vec2 df = getBicubicFirstDerivatives(mapCoordinates);
  return normalize(vec3(-df.x, -df.y, 1.0));
}

vec2 getBicubicInterpolationGradient(vec2 mapCoordinates)
{
  return getBicubicFirstDerivatives(mapCoordinates);
}


//==================================================================
//                Linear/Bicubic switch functions
//==================================================================

// Retrieve height
float getHeight(vec2 mapCoordinates)
{
  if (u_HeightMap.functional == 0)
  {
    float height;
    if (u_HeightMap.smoothInterpolation < 2)
      height = getLinearInterpolationHeight(mapCoordinates);
    else
      height = getBicubicInterpolationHeight(mapCoordinates);
    return u_HeightMap.heightFactor * height;
  }

  // functional height (f(u,v)={u,v,u(1-u)*v(1-v)}
  return u_HeightMap.functional * mapCoordinates.s * (1 - mapCoordinates.s) * mapCoordinates.t * (1 - mapCoordinates.t);
}

// Retrieves normal (in Model coordinates) to vertex using the height map
vec3 getModelNormalFromTexture(vec2 mapCoordinates)
{
  if (u_HeightMap.functional == 0)
  {
    if (u_HeightMap.smoothInterpolation == 0)
      return getConstantNormal(mapCoordinates);

    if (u_HeightMap.smoothInterpolation == 1)
      return getLinearInterpolationNormal(mapCoordinates);

    return getBicubicInterpolationNormal(mapCoordinates);
  }
  else
  {
    //functional function(f(u, v) = { u,v,u(1 - u) * v(1 - v) }
    float u = mapCoordinates.s;
    float v = mapCoordinates.t;
    vec3 dfu = (u_Positionning.model * vec4(1, 0, u_HeightMap.functional*(1-2*u)*v*(1-v), 0)).xyz;
    vec3 dfv = (u_Positionning.model * vec4(0, 1, u_HeightMap.functional*(1-2*v)*u*(1-u), 0)).xyz;
    return  normalize(cross(dfu, dfv));
  }
}

vec2 getGradient(vec2 mapCoordinates)
{
  if (u_HeightMap.functional == 0)
  {
    if (u_HeightMap.smoothInterpolation == 0)
      return getConstantGradient(mapCoordinates);
    if (u_HeightMap.smoothInterpolation == 1)
      return getLinearInterpolationGradient(mapCoordinates);
    return getBicubicInterpolationGradient(mapCoordinates);
  }
  else
  {
    //functional function(f(u, v) = { u,v,u(1 - u) * v(1 - v) }
    float u = mapCoordinates.s;
    float v = mapCoordinates.t;
    vec3 dfu = (u_Positionning.model * vec4(1, 0, u_HeightMap.functional*(1-2*u)*v*(1-v), 0)).xyz;
    vec3 dfv = (u_Positionning.model * vec4(0, 1, u_HeightMap.functional*(1-2*v)*u*(1-u), 0)).xyz;
    return  vec2(dfu.x/dfu.z, dfv.y/dfv.z);
  }
}
