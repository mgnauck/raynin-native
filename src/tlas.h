#ifndef TLAS_H
#define TLAS_H

#include <stddef.h>
#include "vec3.h"

typedef struct bvh_inst bvh_inst;

typedef struct tlas_node {
  vec3      min;
  size_t    children; // 2x 16 bit
  vec3      max;
  size_t    bvh_inst; // Leaf nodes only
} tlas_node;

typedef struct tlas {
  size_t    node_cnt;
  tlas_node *nodes;
  size_t    inst_cnt;
  bvh_inst  *instances;
} tlas;

void tlas_init(tlas *t, size_t inst_cnt);
void tlas_build(tlas *t);

#endif
