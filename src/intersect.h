#ifndef INTERSECT_H
#define INTERSECT_H

#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include "vec3.h"

#define MAX_DISTANCE  FLT_MAX

typedef struct ray ray;
typedef struct tri tri;
typedef struct bvh bvh;
typedef struct bvh_inst bvh_inst;
typedef struct tlas tlas;

typedef struct hit {
  float   t;
  float   u;
  float   v;
  size_t  prim_id;
} hit;

float intersect_aabb(const ray *r, float curr_dist, vec3 min_ext, vec3 max_ext);
void  intersect_tri(const ray *r, const tri *t, size_t prim_id, hit *h);

void  intersect_bvh(const ray *r, const bvh *b, hit *h);
void  intersect_bvh_inst(const ray *r, const bvh_inst *b, hit *h);
void  intersect_tlas(const ray *r, const tlas *t, hit *h);

#endif
