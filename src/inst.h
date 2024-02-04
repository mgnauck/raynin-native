#ifndef INST_H
#define INST_H

#include <stddef.h>
#include "mat4.h"
#include "mat.h"

typedef struct mesh mesh;
typedef struct bvh bvh;

typedef struct inst {
  vec3      min;
  size_t    tri_ofs;
  vec3      max;
  size_t    bvh_node_ofs;
  mat4      transform;
  mat4      inv_transform;
  size_t    id; // (mesh idx << 20) | (inst idx & 0xfffff)
  mat_type  mat_type;
  size_t    mat_ofs;
} inst;

void inst_create(inst *inst, size_t mesh_idx, size_t inst_idx,
    const mesh *mesh, const bvh *bvh, const mat4 transform);

#endif
