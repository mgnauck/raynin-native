#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

// Standard math (row first) order:
//
// 0 1 2 3       1 0 0 X
// 4 5 6 7  and  0 1 0 Y
// 8 9 0 a       0 0 1 Z
// b c d e       0 0 0 1 

typedef float mat4[16];

void mat4_identity(mat4 m);
vec3 mat4_transform_pos(mat4 m, vec3 v);
vec3 mat4_transform_dir(mat4 m, vec3 v);
void mat4_inverse(mat4 dest, const mat4 src);

#endif
