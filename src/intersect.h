#ifndef INTERSECT_H
#define INTERSECT_H

#include <stdbool.h>
#include "vec3.h"

typedef struct ray ray;
typedef struct tri tri;

float intersect_aabb(const ray *r, vec3 min_ext, vec3 max_ext);
void  intersect_tri(ray *r, const tri *t);

#endif
