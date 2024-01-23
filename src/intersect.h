#ifndef INTERSECT_H
#define INTERSECT_H

#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include "vec3.h"

#define MAX_DISTANCE  FLT_MAX

typedef struct ray ray;
typedef struct tri tri;

typedef struct hit {
  float   t;
  float   u;
  float   v;
  size_t  prim_id;
} hit;

float intersect_aabb(const ray *r, float curr_dist, vec3 min_ext, vec3 max_ext);
void  intersect_tri(const ray *r, const tri *t, size_t prim_id, hit *h);

#endif
