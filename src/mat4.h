#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include <stdbool.h>

// Standard math (row first) order:
//
// 0 1 2 3       1 0 0 x
// 4 5 6 7  and  0 1 0 y
// 8 9 0 a       0 0 1 z
// b c d e       0 0 0 1

typedef float mat4[16];

void mat4_identity(mat4 m);
void mat4_transpose(mat4 d, const mat4 m);

void mat4_trans(mat4 dest, const vec3 v);

void mat4_rot_x(mat4 dest, float radians);
void mat4_rot_y(mat4 dest, float radians);
void mat4_rot_z(mat4 dest, float radians);

void mat4_mul(mat4 dest, const mat4 a, const mat4 b);

vec3 mat4_mul_pos(const mat4 m, const vec3 v);
vec3 mat4_mul_dir(const mat4 m, const vec3 v);

bool mat4_inv(mat4 dest, const mat4 src);

#endif
