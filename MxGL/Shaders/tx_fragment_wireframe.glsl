//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

// Inputs resulting from GS computation (except HeightTextureUV, passed)
in GSO
{
  vec2  HeightTextureUV;
  vec4  VertexColor;
  float Height;
  vec2  DistanceToViewer;
} fsi;

// Resulting fragment's color
out vec4 fso_color;

void main(void)
{
  fso_color = fsi.VertexColor;
}