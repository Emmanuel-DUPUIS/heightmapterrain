
layout(std430) writeonly buffer Struct_TxReport_heights
{
  struct {
    vec3 position;
    float    height;
  } data[1024];
  uint counter;
} TxReport_heights;
