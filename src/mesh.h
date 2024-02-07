#ifndef MESH_H
#define MESH_H

#include <stdint.h>

typedef struct vec3 vec3;
typedef struct tri tri;
typedef struct tri_data tri_data;

typedef struct mesh {
  uint32_t  tri_cnt;
  tri       *tris;
  tri_data  *tris_data;
} mesh;

void mesh_init(mesh *m, uint32_t tri_cnt);
void mesh_load_obj(mesh *m, const char *path, uint32_t tri_cnt, uint32_t vertex_cnt, uint32_t normal_cnt, uint32_t uv_cnt);

#endif
