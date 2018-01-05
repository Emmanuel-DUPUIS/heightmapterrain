//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

layout(points) in;
layout(points, max_vertices = 2) out;

out vec4 vso_color;

void main(void)
{
  gl_Position = u_Viewing.projection * u_Viewing.view * gl_in[0].gl_Position;
  vso_color = vec4(1.0, 0.0, 0.0, 1.0);
  gl_PointSize = 4.0 * clamp(500.0/gl_Position.z-0.3, 0.0, 1.0); 
  EmitVertex();
  EndPrimitive();
  
  gl_Position = u_Viewing.projection * u_Viewing.view * vec4(gl_in[0].gl_Position.xy, 0.0, 1.0);
  vso_color = vec4(0.0, 1.0, 0.0, 1.0);
  EmitVertex();
  EndPrimitive();
}
