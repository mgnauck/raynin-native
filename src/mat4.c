#include "mat4.h"

void mat4_identity(mat4 m)
{
  m = (mat4){
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1 };
}

vec3 mat4_transform_pos(mat4 m, vec3 v)
{
  // TODO
  return v;
}

vec3 mat4_transform_dir(mat4 m, vec3 v)
{
  // TODO
  return v;
}

void mat4_inverse(mat4 dest, const mat4 src)
{
  // TODO
}
