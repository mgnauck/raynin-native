#include "trace.h"
#include "mutil.h"
#include "ray.h"
#include "tri.h"
#include "mesh.h"
#include "bvh.h"
#include "intersect.h"

#define NODE_STACK_SIZE 64

vec3 trace_bvh(ray *r, const mesh *m)
{
  uint32_t stack_pos = 0;
  bvh_node *node_stack[NODE_STACK_SIZE];
  bvh_node *node = &m->bvh->nodes[0];

  while(true) {
    if(node->obj_cnt > 0) {
      // Leaf node, check triangles
      for(size_t i=0; i<node->obj_cnt; i++)
        intersect_tri(r, &m->tris[m->bvh->indices[node->start_idx + i]]);
      if(stack_pos > 0)
        node = node_stack[--stack_pos];
      else
        break;
    } else {
      // Interior node, check aabbs of children
      bvh_node *c1 = &m->bvh->nodes[node->start_idx];
      bvh_node *c2 = &m->bvh->nodes[node->start_idx + 1];
      float     d1 = intersect_aabb(r, c1->min, c1->max);
      float     d2 = intersect_aabb(r, c2->min, c2->max);
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

   if(r->t < MAX_DISTANCE) {
    float c = 40.0f / (r->t * 42.0f);
    return (vec3){ c, c, c };
  }

  return (vec3){ 0.0f, 0.0f, 0.0f };
}

vec3 trace_mesh(ray *r, const mesh *m)
{
  for(size_t i=0; i<m->tri_cnt; i++)
    intersect_tri(r, &m->tris[i]);

  if(r->t < MAX_DISTANCE) {
    float c = 400.0f / (r->t * 50.0f);
    return (vec3){ c, 0, 0 };
  }

  return (vec3){ 0.0f, 0.0f, 0.0f };
}
