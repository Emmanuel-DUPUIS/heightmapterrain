//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Vertex Shader for terrain surface representation.
//========================================================================

// 3D coord of the unitary patch vertex 
in vec4 i_VertexPos;
// Associated position (u,v) on the height map of the vertex 
in vec2 i_HeightTextureUV;
// Base color of the vertex (used mainly for debug)
in vec4 i_VertexColor;

out VSO
{
  vec2 HeightTextureUV;
  vec4 VertexColor;
} vso;


void main(void)
{
  // Data passed to tesselation control shader (TCS)
  gl_Position         = u_Positionning.model * i_VertexPos;
  vso.HeightTextureUV = i_HeightTextureUV;
  vso.VertexColor     = i_VertexColor;
}
