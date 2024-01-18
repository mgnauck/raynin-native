#ifndef RAY_H
#define RAY_H

#include <float.h>
#include "vec3.h"

#define MAX_DISTANCE  FLT_MAX

typedef struct view view;
typedef struct cam cam;

typedef struct ray {
  vec3  ori;
  vec3  dir;
  vec3  inv_dir;
} ray;

ray ray_create(vec3 ori, vec3 dir);
ray ray_create_primary(float x, float y, view *v, cam *c);

#endif
