#include "trace.h"
#include "mutil.h"
#include "ray.h"
#include "tri.h"
#include "mesh.h"
#include "bvh.h"
#include "intersect.h"

vec3 trace_bvh(ray *r, const mesh *m)
{
#define NODE_STACK_SIZE 64
  uint32_t  stack_pos = 0;
  bvh_node  *node_stack[NODE_STACK_SIZE];
  bvh_node  *node = &m->bvh->nodes[0];
  hit       h = (hit){ .t = MAX_DISTANCE };

  while(true) {
    if(node->obj_cnt > 0) {
      // Leaf node, check triangles
      for(size_t i=0; i<node->obj_cnt; i++) {
        size_t tri_idx = m->bvh->indices[node->start_idx + i];
        intersect_tri(r, &m->tris[tri_idx], tri_idx, &h);
      }
      if(stack_pos > 0)
        node = node_stack[--stack_pos];
      else
        break;
    } else {
      // Interior node, check aabbs of children
      bvh_node *c1 = &m->bvh->nodes[node->start_idx];
      bvh_node *c2 = &m->bvh->nodes[node->start_idx + 1];
      float     d1 = intersect_aabb(r, h.t, c1->min, c1->max);
      float     d2 = intersect_aabb(r, h.t, c2->min, c2->max);
      if(d1 > d2) {
        // Swap for nearer child
        float td = d1;
        d1 = d2;
        d2 = td;
        bvh_node *tc = c1;
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

  if(h.t < MAX_DISTANCE)
    return (vec3){ h.u, h.v, 1.0f - h.u - h.v };

  return (vec3){ 0.0f, 0.0f, 0.0f };
}

vec3 trace_mesh(ray *r, const mesh *m)
{
  hit h = (hit){ .t = MAX_DISTANCE };
  for(size_t i=0; i<m->tri_cnt; i++)
    intersect_tri(r, &m->tris[i], i, &h);

  if(h.t < MAX_DISTANCE)
    return (vec3){ h.u, h.v, 1.0f - h.u - h.v };

  return (vec3){ 0.0f, 0.0f, 0.0f };
}
