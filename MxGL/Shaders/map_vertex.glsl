//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

in vec2  i_VertexPos;
in ivec2 i_Pixel;

void main(void)
{
  gl_Position  = u_Positionning.model * vec4(i_VertexPos, u_HeightMap.heightFactor * texelFetch(u_HeightMap.heightTexture, i_Pixel, 0).r, 1);
}
