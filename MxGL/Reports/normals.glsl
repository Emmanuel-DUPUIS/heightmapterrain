
layout(std430) writeonly buffer Struct_UxReport_normals
{
  struct {
    vec4 dfu;
    vec4 dfv;

  } data[1024];
  uint counter;
} UxReport_normals;
