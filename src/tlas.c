#include "tlas.h"
#include <float.h>
#include "sutil.h"
#include "bvh.h"
#include "aabb.h"

void tlas_init(tlas *t, bvh_inst *instances, size_t inst_cnt)
{
  // Would be 2 * inst_cnt - 1 but we skip one node
  t->nodes = aligned_alloc(64, 2 * inst_cnt * sizeof(*t->nodes)); 
  t->instances = instances;
  t->inst_cnt = inst_cnt;
}

size_t find_best_node(tlas_node *nodes, size_t curr_idx, size_t *indices, size_t indices_cnt)
{
  float best_area = FLT_MAX;
  size_t best_idx;

  vec3 curr_min = nodes[indices[curr_idx]].min;
  vec3 curr_max = nodes[indices[curr_idx]].max;

  for(size_t i=0; i<indices_cnt; i++) {
    if(curr_idx != i) {
      vec3 mi = vec3_min(curr_min, nodes[indices[i]].min);
      vec3 ma = vec3_max(curr_max, nodes[indices[i]].min);
      float area = aabb_calc_area((aabb){ mi, ma });
      if(area < best_area) {
        best_area = area;
        best_idx = i;
      }
    }
  }

  return best_idx;
}

// Walter et al: Fast Agglomerative Clustering for Rendering
void tlas_build(tlas *t)
{
  size_t node_indices_cnt = t->inst_cnt;
  size_t node_indices[node_indices_cnt];

  // Construct leaf node for each bvh instance
  for(size_t i=0; i<t->inst_cnt; i++) {
    tlas_node *n = &t->nodes[1 + i];
    bvh_inst *bi = &t->instances[i];
    n->min = bi->min;
    n->max = bi->max;
    n->children = 0;
    n->bvh_inst = i;
    node_indices[i] = 1 + i;
  }

  // Account for leaf nodes so far and reserve a spot for the root
  t->node_cnt = 1 + t->inst_cnt;

  // Bottom up combining of tlas nodes
  size_t a = 0;
  size_t b = find_best_node(t->nodes, a, node_indices, node_indices_cnt);
  while(node_indices_cnt > 1) {
    size_t c = find_best_node(t->nodes, b, node_indices, node_indices_cnt);
    if(a == c) {
      size_t idx_a = node_indices[a];
      size_t idx_b = node_indices[b];
      tlas_node *node_a = &t->nodes[idx_a];
      tlas_node *node_b = &t->nodes[idx_b];
      tlas_node *new_node = &t->nodes[t->node_cnt];
      // Encode child node indices in 16 bits each
      new_node->children = idx_b << 16 | idx_a;
      new_node->min = vec3_min(node_a->min, node_b->min);
      new_node->max = vec3_max(node_a->max, node_b->max);
      // Replace node A with newly created combined node
      node_indices[a] = t->node_cnt++;
      // Remove node B by replacing its slot with last node
      node_indices[b] = node_indices[--node_indices_cnt];
      // Restart the loop for remaining nodes
      b = find_best_node(t->nodes, a, node_indices, node_indices_cnt);
    } else {
      // The best match B we found for A had itself a better match in C, thus
      // A and B are not best matches and we continue with B and C.
      a = b;
      b = c;
    }
  }

  // Root node was formed last, move it to index 0
  t->nodes[0] = t->nodes[t->node_cnt--];
}

void tlas_release(tlas *t)
{
  free(t->nodes);
}
