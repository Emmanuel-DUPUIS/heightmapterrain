//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

in vec4 vso_color;
out vec4 fso_color;

void main(void)
{
  gl_FragDepth = gl_FragCoord.z - 0.001;
  fso_color = vso_color;
}