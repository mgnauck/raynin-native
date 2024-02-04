#include "bvhinst.h"
#include "buf.h"
#include "mesh.h"
#include "bvh.h"
#include "aabb.h"

void bvh_inst_create(bvh_inst *bi, size_t mesh_idx, size_t inst_idx,
    const mesh *mesh, const bvh *bvh, const mat4 transform)
{
  bi->tri_ofs = buf_idx(TRI, mesh->tris);
  bi->bvh_node_ofs = buf_idx(BVH_NODE, bvh->nodes);
  bi->id = (mesh_idx << 20) | (inst_idx & 0xfffff);
  
  // Store root node bounds transformed into world space
  aabb a = aabb_init();
  vec3 mi = bvh->nodes[0].min;
  vec3 ma = bvh->nodes[0].max;
  
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ mi.x, mi.y, mi.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ ma.x, mi.y, mi.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ mi.x, ma.y, mi.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ ma.x, ma.y, mi.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ mi.x, mi.y, ma.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ ma.x, mi.y, ma.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ mi.x, ma.y, ma.z }));
  aabb_grow(&a, mat4_mul_pos(transform, (vec3){ ma.x, ma.y, ma.z }));

  bi->min = a.min;
  bi->max = a.max;

  // Store transform and precalc inverse
  for(size_t i=0; i<16; i++)
    bi->transform[i] = transform[i];

  mat4_inv(bi->inv_transform, transform);
}
