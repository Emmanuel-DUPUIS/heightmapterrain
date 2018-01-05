//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

out vec4 vso_color;

void main(void)
{
  gl_Position = u_Viewing.projection * u_Viewing.view * gl_in[0].gl_Position;
  vso_color = vec4(1.0, 0.0, 0.0, 1.0);
  EmitVertex();

  gl_Position = u_Viewing.projection * u_Viewing.view * gl_in[1].gl_Position;
  vso_color = vec4(1.0, 0.0, 0.0, 1.0);
  EmitVertex();
  
  EndPrimitive();
}
