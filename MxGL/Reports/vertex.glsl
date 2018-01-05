
layout(std430) writeonly buffer Struct_UxReport_vertex
{
  struct {
    vec2 uv0;
    vec2 uv1;
    vec4 p0;
    vec4 p1;
    vec2 uv2;
    vec2 uv3;
    vec4 p2;
    vec4 p3;
    vec3 _alignment;
    float    tesselation;

  } data[64];
  uint counter;
} UxReport_vertex;
