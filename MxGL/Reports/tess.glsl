
layout(std430) writeonly buffer Struct_UxReport_tess
{
  struct {
    vec2 uv0;
    vec2 uv1;
    vec2 tesselate;

  } data[512];
  uint counter;
} UxReport_tess;
