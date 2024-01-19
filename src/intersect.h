#ifndef INTERSECT_H
#define INTERSECT_H

#include <stdbool.h>
#include "ray.h"

typedef struct tri tri;

bool  intersect_slabs(ray r, vec3 min_ext, vec3 max_ext);
float intersect_aabb(ray r, vec3 min_ext, vec3 max_ext, float tmin, float tmax);

float intersect_tri(ray r, tri *t, float *u, float *v);

#endif
