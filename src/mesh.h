#ifndef MESH_H
#define MESH_H

#include <stddef.h>

typedef struct vec3 vec3;
typedef struct tri tri;
typedef struct tri_data tri_data;
typedef struct bvh bvh;

typedef struct mesh {
  size_t    tri_cnt;
  tri       *tris;
  tri_data  *tris_data;
  vec3      *centers;
  bvh       *bvh;
} mesh;

mesh  *mesh_init(size_t tri_cnt);
void  mesh_release(mesh *m);
void  mesh_create_bvh(mesh *m);

mesh  *mesh_create_file(const char *path, size_t tri_cnt);
mesh  *mesh_create_rand(size_t tri_cnt);

#endif
