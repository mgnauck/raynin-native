#include "mesh.h"
#include <stdio.h>
#include "sutil.h"
#include "tri.h"
#include "bvh.h"

mesh *mesh_init(size_t tri_cnt)
{
  mesh *m = malloc(sizeof(*m));
  m->tri_cnt = tri_cnt;
  m->tris = malloc(m->tri_cnt * sizeof(*m->tris));
  m->centers = malloc(m->tri_cnt * sizeof(*m->centers));
  return m;
}

void mesh_release(mesh *m)
{
  bvh_release(m->bvh);
  
  free(m->tris);
  free(m->centers);
  free(m);
}

void mesh_create_bvh(mesh *m)
{
  m->bvh = bvh_init(m);
  bvh_create(m->bvh);
}

mesh *mesh_create_file(const char *path, size_t tri_cnt)
{
  mesh *m = mesh_init(tri_cnt);

  FILE *f = fopen(path, "r");
  float v[9];
  for(size_t i=0; i<tri_cnt; i++) {
    fscanf(f, "%f %f %f %f %f %f %f %f %f\n",
        &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8]);
    tri *tri = &m->tris[i];
    tri->v[0] = (vec3){ v[0], v[1], v[2] };
    tri->v[1] = (vec3){ v[3], v[4], v[5] };
    tri->v[2] = (vec3){ v[6], v[7], v[8] };
    m->centers[i] = tri_calc_center(tri);
  }
  fclose(f);

  mesh_create_bvh(m);

  return m;
}

mesh *mesh_create_rand(size_t tri_cnt)
{
  mesh *m = mesh_init(tri_cnt);

  for(size_t i=0; i<m->tri_cnt; i++) {
    tri *tri = &m->tris[i];
    tri->v[0] = vec3_sub(vec3_scale(vec3_rand(), 16.0f), (vec3){ 8.0f, 8.0f, 8.0f });
    tri->v[1] = vec3_add(tri->v[0], vec3_rand());
    tri->v[2] = vec3_add(tri->v[0], vec3_rand());
    m->centers[i] = tri_calc_center(tri);
  }

  mesh_create_bvh(m);

  return m;
}
