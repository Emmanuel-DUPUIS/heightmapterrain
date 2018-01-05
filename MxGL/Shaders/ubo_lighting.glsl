//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Uniform Buffer Oject definition for mono-source lighting.
//========================================================================

layout(std140) uniform u_LightingBlock
{
  // Light configuration to compute Gouraud shading
  vec4  position;          // Position of the light source given in WC
  vec4  ambiantColor;
  vec4  diffuseColor;
  vec3  specularColor;
  float specularPower;
} u_Lighting;
