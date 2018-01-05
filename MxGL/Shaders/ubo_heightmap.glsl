//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Uniform Buffer Oject definition for Height Map.
//========================================================================

layout(std140) uniform u_HeightMapBlock
{
  sampler2D heightTexture;            // Bindless texture, RGB with same value on each component
  vec2      terrainDimension;         // Dimension (WC) of the patch
  ivec2     terrainSubdivision;       // Number of patches in each direction
  float     heightFactor;             // Scale factor along altitude (z axis)
  float     distortionFactor;         // Factor applied to patch height distortion to determine the subdivision level
  float     maxSubdivison;            // Max number of subdivision of each side of patch (tesselation process)
  float     maxPixelSubdivisionRatio; // Subdivision factor based on edge dimension on screen
  sampler2D heightColorMap;           // Bindless texture, RGB with map along u and constant along v 
  float     minHeightColorMap;        // Height corresponding to heightColorMap(u=0.0)
  float     maxHeightColorMap;        // Height corresponding to heightColorMap(u=1.0)
  uint      colorMode;                // Heigt color map
  float     isolineStep;              // Isoline mode: for points with height (window 2D pixel thickness)
  uint      wireframe;                // Display patch and triangle borders and normals
  float     functional;               // Functional height (replace height map with calibration function f(u,v)=u(1-u)*v(1-v))
  uint      smoothInterpolation;      // Level of interpolation (Linear/Constant,Linear,Bicubic) for height and normal
  uint      shadow;                   // Shadow mode, alternate method to shadow mapping, unsatisfactory
  float     minHeight;                // Minimal absolute value for height (for animation)
  float     maxHeight;                // Maximal absolute value for height (for animation)
} u_HeightMap;


