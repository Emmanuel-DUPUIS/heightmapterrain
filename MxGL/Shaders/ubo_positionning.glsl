//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Uniform Buffer Oject definition for model positionning.
//========================================================================

layout(std140) uniform u_PositionningBlock
{
  mat4 model;            // Model coordinates to world coordinates
} u_Positionning;
