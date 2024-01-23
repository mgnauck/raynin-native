#ifndef TRI_H
#define TRI_H

#include "vec3.h"

typedef struct tri {
  vec3 v[3];
} tri;

typedef struct tri_data {
  vec3  n[3];
  float u[3];
  float v[3];
  float pad;
} tri_data;

vec3 tri_calc_center(const tri *t);

#endif
