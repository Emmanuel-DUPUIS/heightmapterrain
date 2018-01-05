//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Tesselation Eveluation Shader for terrain surface representation.
//    To avoid cracks on shared edge (between two adjacent patches), 
//    the tesselation factor must be identical for both instances. 
//    Since screenCovering is intrinsically reflexive, it's only 
//    necessary to enforce heightDistortion reflixivity. So, the latter
//    function must consider the maximum height distortion considering
//    both side of the shared edge.
//========================================================================

// Normalized uv coordinates within the patch (output of vertex shader)
in VSO
{
  vec2 HeightTextureUV; 
  vec4 VertexColor;
} tcsi[];

// Per-patch output
//patch out float gl_TessLevelOuter[4];
//patch out float gl_TessLevelInner[2];

layout(binding = 1) uniform atomic_uint u_GeometryCounter2;

// Quad patch
layout(vertices = 4) out;

out VSO
{
  vec2 HeightTextureUV;
  vec4 VertexColor;
} tcso[];


vec2 ndc(vec4 modelPoint)
{
  const mat4 pv = u_Viewing.projection * u_Viewing.view;
  vec4 proj = pv * modelPoint;
  return proj.xy / proj.w;
}

// Computes the subdivision factor according to the size of the projected
// edge on the viewport
float screenCovering(vec2 projVertex1, vec2 projVertex2)
{
  float pixelDistance = 0.5 * distance(u_Viewing.viewport * projVertex1, u_Viewing.viewport * projVertex2);
  return pixelDistance / u_HeightMap.maxPixelSubdivisionRatio;
}

float heightDistortion(vec4 vertex0, vec4 vertex1, vec2 uv0, vec2 uv1)
{
  // Point in the middle of the edge
  vec2 uvCenter = 0.5*(uv0 + uv1);
  vec4 center   = vec4(0.5*(vertex0.xy + vertex1.xy), getHeight(uvCenter), 0);

  // Orthogonal delta from center of edge
  const vec2 deltaUV = 0.5*vec2(float(1) / u_HeightMap.terrainSubdivision.x, float(1) / u_HeightMap.terrainSubdivision.y);

  vec2 orthoUV  = normalize(vec2(uv0.t - uv1.t, uv1.s - uv0.s));
  vec2 orthoVec = normalize(vec2(vertex0.y - vertex1.y, vertex1.x - vertex0.x));

  // Computes vertex2 and vertex3 , midldes of both adjacent patches to the edge (clamp if edge belongs to the terrain border)
  // from resp. uv2 and uv3 since clamp function may have relimited inside terrain limits
  vec2 delta   = orthoUV*deltaUV;
  vec2 uv2     = clamp(uvCenter + delta, 0, 1);
  vec2 duv     = uv2 - uvCenter;
  vec4 vertex2 = vec4(center.xy + (u_HeightMap.terrainDimension.x*duv.x+u_HeightMap.terrainDimension.y*duv.y)*orthoVec, getHeight(uv2), 0);
  vec2 uv3     = clamp(uvCenter - delta, 0, 1);
  duv          = uv3 - uvCenter;
  vec4 vertex3 = vec4(center.xy + (u_HeightMap.terrainDimension.x*duv.x+u_HeightMap.terrainDimension.y*duv.y)*orthoVec, getHeight(uv3), 0);

  float mean = 0.2 * (vertex0.z + vertex1.z + vertex2.z + vertex3.z + center.z);
  float distortion = 0.0;
  float t = vertex0.z - mean;
  distortion += t*t;
  t = vertex1.z - mean;
  distortion += t*t;
  t = vertex2.z - mean;
  distortion += t*t;
  t = vertex3.z - mean;
  distortion += t*t;
  t = center.z - mean;
  distortion += t*t;

  float minT = 0;
  // Impose greater subdivision when isoline to be displayed inside the patch
  // TODO: validate that min better than multiplication factor or both or other integration
  //       at computeTesselationOneSide level
  if (u_HeightMap.isolineStep > 0)
  {
    float min = min(min(min(min(center.z, vertex0.z), vertex1.z), vertex2.z), vertex3.z);
    float max = max(max(max(max(center.z, vertex0.z), vertex1.z), vertex2.z), vertex3.z);
    int minU = int(min / u_HeightMap.isolineStep);
    int maxU = int(max / u_HeightMap.isolineStep);
    if (minU != maxU)
      minT = 2.0;
  }

  t = max(minT, u_HeightMap.distortionFactor * sqrt(distortion / (distance(vertex0, vertex1) * distance(vertex2, vertex3))));

  // Report Data to CPU for debugging session
  UxReport::addRecord("vertex");
  UxReport::setValue("vertex", uv0, uv0);
  UxReport::setValue("vertex", uv1, uv1);
  UxReport::setValue("vertex", uv2, uv2);
  UxReport::setValue("vertex", uv3, uv3);
  UxReport::setValue("vertex", p0, vertex0);
  UxReport::setValue("vertex", p1, vertex1);
  UxReport::setValue("vertex", p2, vertex2);
  UxReport::setValue("vertex", p3, vertex3);
  UxReport::setValue("vertex", tesselation, t);

  return t;
}

float computeTesselationEdge(float scrCovering, float distortion)
{
  // Combining screen covering (viewport's size of the element) and height distortion
  return clamp(floor(clamp(scrCovering, 0, 4) * distortion), 1, u_HeightMap.maxSubdivison);
}

void main(void)
{
  if (gl_InvocationID == 0)
  {
    vec2 ndcPos[4];

    // Discard non visible (out of viewing frustum) patches
    ndcPos[0] = ndc(gl_in[0].gl_Position);
    ndcPos[1] = ndc(gl_in[1].gl_Position);
    ndcPos[2] = ndc(gl_in[2].gl_Position);
    ndcPos[3] = ndc(gl_in[3].gl_Position);

    // Beware v-axis and y-axis opposite (last paramter value, dPatch.y)
    float distortion = heightDistortion(gl_in[3].gl_Position, gl_in[0].gl_Position, tcsi[3].HeightTextureUV, tcsi[0].HeightTextureUV);
    gl_TessLevelOuter[0] = computeTesselationEdge(screenCovering(ndcPos[3], ndcPos[0]), distortion);
      
    distortion = heightDistortion(gl_in[0].gl_Position, gl_in[1].gl_Position, tcsi[0].HeightTextureUV, tcsi[1].HeightTextureUV);
    gl_TessLevelOuter[1] = computeTesselationEdge(screenCovering(ndcPos[0], ndcPos[1]), distortion);

    distortion = heightDistortion(gl_in[1].gl_Position, gl_in[2].gl_Position, tcsi[1].HeightTextureUV, tcsi[2].HeightTextureUV);
    gl_TessLevelOuter[2] = computeTesselationEdge(screenCovering(ndcPos[1], ndcPos[2]), distortion);

    distortion = heightDistortion(gl_in[2].gl_Position, gl_in[3].gl_Position, tcsi[2].HeightTextureUV, tcsi[3].HeightTextureUV);
    gl_TessLevelOuter[3] = computeTesselationEdge(screenCovering(ndcPos[2], ndcPos[3]), distortion);

    // Inner tessellation level
    gl_TessLevelInner[0] = 0.5 * (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]);
    gl_TessLevelInner[1] = 0.5 * (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]);
  }
  
	// Passes inchnaged vertex's position
	gl_out[gl_InvocationID].gl_Position   = gl_in[gl_InvocationID].gl_Position;

  tcso[gl_InvocationID].HeightTextureUV = tcsi[gl_InvocationID].HeightTextureUV; 
  tcso[gl_InvocationID].VertexColor     = tcsi[gl_InvocationID].VertexColor;
}
