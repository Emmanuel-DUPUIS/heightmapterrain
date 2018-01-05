//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Geometry Shader for terrain wireframe representation (patch/triangle
//  borders or/and triangle/height map normals).
//========================================================================

layout(triangles) in;
layout(line_strip, max_vertices = 14) out; 

in TESO
{
  vec2  HeightTextureUV;
  vec4  VertexColor;
  float Height;
  uint  OnBorder;
} gsi[];

out GSO
{
  vec2  HeightTextureUV;
  vec4  VertexColor;
  float Height;
  vec2  DistanceToViewer;
} gso;


void main(void)
{
  vec4 pos[3];
  pos[0] = u_Viewing.view*gl_in[0].gl_Position;
  pos[1] = u_Viewing.view*gl_in[1].gl_Position;
  pos[2] = u_Viewing.view*gl_in[2].gl_Position;

  if (u_HeightMap.wireframe == 1 || u_HeightMap.wireframe == 3)
  { 
    for (uint index = 0; index < 3; index++)
    {
      if ((gsi[index].OnBorder & gsi[(index+1)%3].OnBorder) != 0)
        gso.VertexColor = vec4(.0, 1., .0, 1.);
      else
        gso.VertexColor = vec4(.0, .0, 1., 1.);

      gl_Position = u_Viewing.projection*pos[index];
      gl_Position.z -= 0.1;
      EmitVertex();
      gl_Position = u_Viewing.projection*pos[(index+1)%3];
      gl_Position.z -= 0.1;
      EmitVertex();
      EndPrimitive();
    }
  }

  if (u_HeightMap.wireframe == 2 || u_HeightMap.wireframe == 3)
  {
    // Triangle normal (white)
    vec3  normal = normalize(cross(pos[1].xyz - pos[0].xyz, pos[2].xyz - pos[0].xyz));
    vec4  anchor = (pos[0] + pos[1] + pos[2]) / 3; // isobarycenter of the triangle
    float height = .4*max(distance(pos[0].xyz, pos[1].xyz), max(distance(pos[1].xyz, pos[2].xyz), distance(pos[0].xyz, pos[2].xyz)));

    gso.VertexColor = vec4(1, 1, 1, 1);
    gl_Position = u_Viewing.projection * anchor;
    EmitVertex();
    gl_Position = u_Viewing.projection * (anchor + height*vec4(normal, 0));
    EmitVertex();
    EndPrimitive();

    // Computed normal at vertices (pink)
    for (uint index = 0; index < 3; index++)
    {
      vec3 normalV = normalize((u_Viewing.view*vec4(getModelNormalFromTexture(gsi[index].HeightTextureUV), 0)).xyz);

      gso.VertexColor = vec4(1, 0, 1, 1);
      gl_Position = u_Viewing.projection*pos[index];
      EmitVertex();
      gl_Position = gl_Position + height*u_Viewing.projection*vec4(normalV, 0);
      EmitVertex();
      EndPrimitive();
    }
  }
}
