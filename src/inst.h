#ifndef INST_H
#define INST_H

#include <stddef.h>
#include "mat4.h"
#include "mat.h"

typedef struct mesh mesh;
typedef struct bvh bvh;

typedef struct inst {
  mat4      transform;
  mat4      inv_transform;
  vec3      min;
  size_t    id;   // (mat type << 28) | (mat id << 16) | (inst id & 0xffff)
  vec3      max;
  size_t    ofs;  // Offset into tris/trisData/indices/bvh_nodes
} inst;

void inst_create(inst *inst, size_t inst_idx, const mat4 transform,
    const mesh *mesh, const bvh *bvh, size_t mat_type, const mat *mat);

#endif
