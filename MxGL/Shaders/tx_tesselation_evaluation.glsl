//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Tesselation Evaluation Shader for terrain surface representation.
//========================================================================

// Generate fractional even subdivisions in counterclockwise ordering
layout(quads, equal_spacing/*fractional_odd_spacing*/) in;

in VSO
{
  vec2 HeightTextureUV;
  vec4 VertexColor;
} tesi[];


patch in float TesselationFactor;

out TESO
{
  vec2  HeightTextureUV;
  vec4  VertexColor;
  float Height;
  uint  OnBorder;
} teso;


void main()
{
	// Interpolation of the 4 vertices from parameters given by tessaltion engine
  float alpha = gl_TessCoord.x;
  float beta  = gl_TessCoord.y;

  vec4 iv1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, alpha);
  vec4 iv2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, alpha);
  vec4 interpolatedVertex = mix(iv1, iv2, beta);

  vec2 iuv1 = mix(tesi[0].HeightTextureUV, tesi[1].HeightTextureUV, alpha);
  vec2 iuv2 = mix(tesi[3].HeightTextureUV, tesi[2].HeightTextureUV, alpha);
  teso.HeightTextureUV = mix(iuv1, iuv2, beta);

  float height = getHeight(teso.HeightTextureUV);
  teso.Height = interpolatedVertex.z = height;
  
  vec4 ic1 = mix(tesi[0].VertexColor, tesi[1].VertexColor, alpha);
  vec4 ic2 = mix(tesi[3].VertexColor, tesi[2].VertexColor, alpha);
  teso.VertexColor = mix(ic1, ic2, beta);

  teso.OnBorder = 0;
  if (gl_TessCoord.x == 0)
    teso.OnBorder |= 0x0001;
  else if (gl_TessCoord.x == 1)
    teso.OnBorder |= 0x0010;
  if (gl_TessCoord.y == 0)
    teso.OnBorder |= 0x0100;
  else if (gl_TessCoord.y == 1)
    teso.OnBorder |= 0x1000;

  gl_Position = interpolatedVertex;
}
