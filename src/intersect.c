#include "intersect.h"
#include "buf.h"
#include "mutil.h"
#include "ray.h"
#include "tri.h"
#include "mesh.h"
#include "bvh.h"
#include "inst.h"
#include "tlas.h"

// GPU efficient slabs test [Laine et al. 2013; Afra et al. 2016]
// https://www.jcgt.org/published/0007/03/04/paper-lowres.pdf
float intersect_aabb(const ray *r, float curr_t, vec3 min_ext, vec3 max_ext)
{
  vec3 t0 = vec3_mul(vec3_sub(min_ext, r->ori), r->inv_dir);
  vec3 t1 = vec3_mul(vec3_sub(max_ext, r->ori), r->inv_dir);

  float tnear = vec3_max_comp(vec3_min(t0, t1));
  float tfar = vec3_min_comp(vec3_max(t0, t1));

  return tnear <= tfar && tnear < curr_t && tfar > EPSILON ? tnear : MAX_DISTANCE;
}

// MT ray-triangle: https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/raytri/
void intersect_tri(const ray *r, const tri *t, size_t obj, size_t tri, hit *h)
{
  // Vectors of two edges sharing vertex 0
  const vec3 edge1 = vec3_sub(t->v[1], t->v[0]);
  const vec3 edge2 = vec3_sub(t->v[2], t->v[0]);

  // Calculate determinant and u parameter later on
  const vec3 pvec = vec3_cross(r->dir, edge2);
  const float det = vec3_dot(edge1, pvec);

  if(fabsf(det) < EPSILON)
    // Ray in plane of triangle
    return;

  const float inv_det = 1.0f / det;

  // Distance vertex 0 to origin
  const vec3 tvec = vec3_sub(r->ori, t->v[0]);

  // Calculate parameter u and test bounds
  const float u = vec3_dot(tvec, pvec) * inv_det;
  if(u < 0.0 || u > 1.0)
    return;

  // Prepare to test for v
  const vec3 qvec = vec3_cross(tvec, edge1);

  // Calculate parameter u and test bounds
  const float v = vec3_dot(r->dir, qvec) * inv_det;
  if(v < 0.0f || u + v > 1.0f)
    return;

  // Calc distance
  const float dist = vec3_dot(edge2, qvec) * inv_det;
  if(dist > EPSILON && dist < h->t) {
    h->t = dist;
    h->u = u;
    h->v = v;
    h->obj = obj;
    h->tri = tri;
  }
}

void intersect_bvh(const ray *r, const bvh_node *nodes, const size_t *indices, const tri *tris, size_t obj, hit *h)
{
#define NODE_STACK_SIZE 64
  uint32_t        stack_pos = 0;
  const bvh_node  *node_stack[NODE_STACK_SIZE];
  const bvh_node  *node = nodes;

  while(true) {
    if(node->obj_cnt > 0) {
      // Leaf node, check triangles
      for(size_t i=0; i<node->obj_cnt; i++) {
        size_t tri = indices[node->start_idx + i];
        intersect_tri(r, &tris[tri], obj, tri, h);
      }
      if(stack_pos > 0)
        node = node_stack[--stack_pos];
      else
        break;
    } else {
      // Interior node, check aabbs of children
      const bvh_node *c1 = &nodes[node->start_idx];
      const bvh_node *c2 = &nodes[node->start_idx + 1];
      float d1 = intersect_aabb(r, h->t, c1->min, c1->max);
      float d2 = intersect_aabb(r, h->t, c2->min, c2->max);
      if(d1 > d2) {
        // Swap for nearer child
        float td = d1;
        d1 = d2;
        d2 = td;
        const bvh_node *tc = c1;
        c1 = c2;
        c2 = tc;
      }
      if(d1 == MAX_DISTANCE) {
        // Did miss both children, so check stack
        if(stack_pos > 0)
          node = node_stack[--stack_pos];
        else
          break;
      } else {
        // Continue with nearer child node
        node = c1;
        // Push farther child on stack if also a hit
        if(d2 < MAX_DISTANCE)
          node_stack[stack_pos++] = c2;
      }
    }
  }
}

void intersect_inst(const ray *r, const inst *inst, hit *h)
{
  ray r_obj;
  ray_transform(&r_obj, inst->inv_transform, r);

  intersect_bvh(&r_obj, buf_ptr(BVH_NODE, inst->bvh_node_ofs),
      buf_ptr(INDEX, inst->tri_ofs), buf_ptr(TRI, inst->tri_ofs), inst->id, h);
}

void intersect_tlas(const ray *r, const tlas_node *nodes, const inst *instances, hit *h)
{
#define NODE_STACK_SIZE 64
  uint32_t        stack_pos = 0;
  const tlas_node *node_stack[NODE_STACK_SIZE];
  const tlas_node *node = nodes;

  while(true) {
    if(node->children == 0) {
      // Leaf node with a single instance assigned
      intersect_inst(r, &instances[node->inst], h);
      if(stack_pos > 0)
        node = node_stack[--stack_pos];
      else
        break;
    } else {
      // Interior node, check aabbs of children
      const tlas_node *c1 = &nodes[node->children & 0xffff];
      const tlas_node *c2 = &nodes[node->children >> 16];
      float d1 = intersect_aabb(r, h->t, c1->min, c1->max);
      float d2 = intersect_aabb(r, h->t, c2->min, c2->max);
      if(d1 > d2) {
        // Swap for nearer child
        float td = d1;
        d1 = d2;
        d2 = td;
        const tlas_node *tc = c1;
        c1 = c2;
        c2 = tc;
      }
      if(d1 == MAX_DISTANCE) {
        // Did miss both children, so check stack
        if(stack_pos > 0)
          node = node_stack[--stack_pos];
        else
          break;
      } else {
        // Continue with nearer child node
        node = c1;
        // Push farther child on stack if also a hit
        if(d2 < MAX_DISTANCE)
          node_stack[stack_pos++] = c2;
      }
    }
  }
}
