#ifndef TRI_H
#define TRI_H

#include "vec3.h"

typedef struct tri {
  vec3 v0;
  vec3 v1;
  vec3 v2;
  vec3 center;
} tri;

typedef struct tri_data {
  vec3  n0;
  vec3  n1;
  vec3  n2;
  float uv0[2];
  float uv1[2];
  float uv2[2];
} tri_data;

void tri_calc_center(tri *t);

#endif
