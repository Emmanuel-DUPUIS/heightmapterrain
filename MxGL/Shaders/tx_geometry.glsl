//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Geometry Shader for terrain ssurface representation.
//========================================================================

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;

layout(binding = 0) uniform atomic_uint u_GeometryCounter1;
layout(binding = 1) uniform atomic_uint u_GeometryCounter2;

// Inputs resulting from TES interpolation (HeightTextureUV, VertexColor) and  
in TESO
{
  vec2  HeightTextureUV;
  vec4  VertexColor;
  float Height;
  uint  OnBorder;
} gsi[];

out GSO
{
  vec4  VertexColor;
  float Height;
  float ScreenHeightGradient;
} gso;

vec2 ndc(vec4 viewPoint)
{
  vec4 proj = u_Viewing.projection * viewPoint;
  return proj.xy / proj.w;
}

void main(void)
{
  const vec4 viewSpaceLightPosition = u_Viewing.view * u_Lighting.position;

  // Discard non visible triangle (out of frustum and back face)
  vec4 viewPoints[3];
  viewPoints[0] = u_Viewing.view * gl_in[0].gl_Position;
  viewPoints[1] = u_Viewing.view * gl_in[1].gl_Position;
  viewPoints[2] = u_Viewing.view * gl_in[2].gl_Position;

  // Patch unvisible if the 3 vertices are out of the frustum
  vec2 ndcPos[3];
  ndcPos[0] = ndc(viewPoints[0]);
  ndcPos[1] = ndc(viewPoints[1]);
  ndcPos[2] = ndc(viewPoints[2]);

  float patchVisibility0 = sign(1 + ndcPos[0].x)*sign(1 - ndcPos[0].x)*sign(1 + ndcPos[0].y)*sign(1 - ndcPos[0].y);
  float patchVisibility1 = patchVisibility0 > 0 ? 1 : sign(1 + ndcPos[1].x)*sign(1 - ndcPos[1].x)*sign(1 + ndcPos[1].y)*sign(1 - ndcPos[1].y);
  float patchVisibility2 = patchVisibility1 > 0 ? 1 : sign(1 + ndcPos[2].x)*sign(1 - ndcPos[2].x)*sign(1 + ndcPos[2].y)*sign(1 - ndcPos[2].y);

  if (patchVisibility2 < 0)
  {
    atomicCounterAddARB(u_GeometryCounter2, 2);
    return;
  }

  // Back-face culling
  if (dot(cross(viewPoints[1].xyz, viewPoints[2].xyz), viewPoints[0].xyz) > 0) // simplification of dot(cross(p1-p0,p2-p0),p0-eye)>0 with eye=origin
  {
    atomicCounterAddARB(u_GeometryCounter2, 1);
    return;
  }

  for (uint index = 0; index < gl_in.length(); index++)
  {
    vec3 modelNormal = getModelNormalFromTexture(gsi[index].HeightTextureUV);

    // Position and normal in view coordinates
    vec4 viewPoint = viewPoints[index]; // u_Viewing.view * gl_in[index].gl_Position;
    vec4 viewNormal = u_Viewing.view * vec4(modelNormal, .0);

    // Point set in clipping space (projection)
    gl_Position = u_Viewing.projection * viewPoint;

    float shadow = 1;
    //====================================================================
    // Shadow: Alternate computation algo to shadow mapping
    //         project line joining point to light onto the height map
    //         and dertemine potential point of greater height masking
    //         the light. Unsatisafctory: the shadow's edge is not accurate
    //         and the performance is highly impaired. Quadtree of the height 
    //         map representing max height might solve performance issue
    //         (intersect the projection with the quadtree). Quality
    //         issue on the shadow's border.
    //
    if (u_HeightMap.shadow > 0)
    {
      vec4 pt   = gl_in[index].gl_Position;
      vec3 dir  = normalize(u_Lighting.position.xyz - pt.xyz);
      vec3 xDir = (u_Positionning.model * vec4(1.0, 0.0, 0.0, 0.0)).xyz;
      vec3 yDir = (u_Positionning.model * vec4(0.0, -1.0, 0.0, 0.0)).xyz;

      float gradient  = dir.z / sqrt(dir.x*dir.x + dir.y*dir.y);
      vec2 textureDir = normalize(vec2(dot(xDir, dir), dot(yDir, dir)));
      ivec2 mapSize   = textureSize(u_HeightMap.heightTexture, 0);
      vec2 dStep      = textureDir / float(mapSize - 1);
      vec2 uv = gsi[index].HeightTextureUV;
      while (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
      {
        uv += dStep;
        vec4  ptT = u_Positionning.model * vec4(vec2(uv.x, 1 - uv.y) * u_HeightMap.terrainSubdivision, getHeight(uv), 1.0);
        vec4  jt  = ptT - pt;
        float lg  = sqrt(jt.x*jt.x + jt.y*jt.y);
        float h   = lg * gradient;
        float limit = 2;
        if (jt.z > h)
        {
          //shadow = 0; break;
          float s = clamp(1-(jt.z-h+2)/10, 0, 1);
          if (s < shadow)
            shadow *= s;
          if (shadow == 0)
            break;
        }
      }
    }
    //================================================================
    
    // Compute light ray incidence and diffuse and specular factors
    // and eventually color at vertex (transparencey inchanged)
    vec3 pointToLightDir = normalize(viewSpaceLightPosition.xyz - viewPoint.xyz);
    vec3 pointToEyeDir   = normalize(-viewPoint.xyz);
    vec3 modelReflection = reflect(-pointToLightDir, viewNormal.xyz);

    float diffuse  = dot(pointToLightDir, viewNormal.xyz); 
    float specular = pow(max(.0, dot(pointToEyeDir.xyz, modelReflection)), u_Lighting.specularPower);
    gso.VertexColor.rgb = min((0.1+shadow/2)*(u_Lighting.ambiantColor.xyz + diffuse*u_Lighting.diffuseColor.xyz + specular*u_Lighting.specularColor) * gsi[index].VertexColor.rgb, 1.);
    gso.VertexColor.w = gsi[index].VertexColor.w;
    
    // Optional isoline display
    gso.ScreenHeightGradient = 0;
    if (u_HeightMap.isolineStep > 0)
    {
      vec2 gradient2D = getGradient(gsi[index].HeightTextureUV);
      vec4 gradient3D = vec4(normalize(vec3(gradient2D.x, gradient2D.y, gradient2D.x*gradient2D.x + gradient2D.y*gradient2D.y)), 0.0);
      vec4 p1 = u_Viewing.projection * u_Viewing.view * (gl_in[index].gl_Position + gradient3D);
      vec4 p2 = u_Viewing.projection * u_Viewing.view * (gl_in[index].gl_Position - gradient3D);
      gso.ScreenHeightGradient = distance(u_Viewing.viewport * p1.xy / p1.w, u_Viewing.viewport * p2.xy / p2.w) / (2 * gradient3D.z);
    }

    // Passes data inchanged
    gso.Height = gsi[index].Height;

    EmitVertex();
  }

  EndPrimitive();
  atomicCounterIncrement(u_GeometryCounter1);
}
