#ifndef BVH_H
#define BVH_H

#include <stddef.h>
#include <stdint.h>
#include "mat4.h"
#include "mat.h"

typedef struct mesh mesh;

typedef struct bvh_node {
  vec3    min;
  size_t  start_idx; // obj start or node index
  vec3    max;
  size_t  obj_cnt;
} bvh_node;

typedef struct bvh {
  size_t    node_cnt;
  bvh_node  *nodes;
  size_t    *indices;
  mesh      *mesh;
} bvh;

typedef struct bvh_inst {
  vec3      min;
  bvh       *bvh;
  vec3      max;
  size_t    inst_idx;
  mat4      inv_transform;
  mat_type  mat_type; // TODO
  size_t    mat_ofs;
} bvh_inst;

void  bvh_init(bvh *b, mesh *m);
void  bvh_build(bvh *b);
void  bvh_update(bvh *b);
void  bvh_release(bvh *b);

void  bvh_create_inst(bvh_inst *bi, bvh *b, size_t idx, const mat4 transform);

#endif
