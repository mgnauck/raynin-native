#ifndef TRACE_H
#define TRACE_H

#include "vec3.h"

typedef struct ray ray;
typedef struct mesh mesh;

vec3 trace_bvh(ray *r, const mesh *m);
vec3 trace_mesh(ray *r, const mesh *m);

#endif
