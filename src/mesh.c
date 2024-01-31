#include "mesh.h"
#include <stdio.h>
#include <string.h>
#include "sutil.h"
#include "tri.h"
#include "bvh.h"

mesh *mesh_init(size_t tri_cnt)
{
  mesh *m = malloc(sizeof(*m));
  m->tri_cnt = tri_cnt;
  m->tris = malloc(m->tri_cnt * sizeof(*m->tris));
  m->tris_data = malloc(m->tri_cnt * sizeof(*m->tris_data));
  m->centers = malloc(m->tri_cnt * sizeof(*m->centers));
  return m;
}

void mesh_create_bvh(mesh *m)
{
  m->bvh = malloc(sizeof(*m->bvh));

  bvh_init(m->bvh, m);
  bvh_build(m->bvh);
}

void mesh_release(mesh *m)
{
  bvh_release(m->bvh);

  free(m->bvh);
  free(m->centers);
  free(m->tris_data);
  free(m->tris);
  free(m);
}

mesh *mesh_load_obj(const char *path, size_t tri_cnt, size_t vertex_cnt, size_t normal_cnt, size_t uv_cnt)
{
  mesh *m = mesh_init(tri_cnt);

  float *vertices = malloc(vertex_cnt * 3 * sizeof(float));
  float *pvertices = vertices;

  float *normals = malloc(normal_cnt * 3 * sizeof(float));
  float *pnormals = normals;

  float *uvs = malloc(uv_cnt * 2 * sizeof(float));
  float *puvs = uvs;

  size_t cnt = 0;

  FILE *f = fopen(path, "r");
  if(!f)
    exit(0);

  while(!feof(f)) {
#define LINE_SIZE 256
    char line[LINE_SIZE] = { 0 };
    fgets(line, LINE_SIZE - 1, f);

    if(line == strstr(line, "vt ")) {
			sscanf(line + 3, "%f %f", puvs, puvs + 1);
      puvs += 2;
    } else if(line == strstr(line, "vn ")) {
			sscanf(line + 3, "%f %f %f", pnormals, pnormals + 1, pnormals + 2);
      pnormals += 3;
    } else if(line[0] == 'v') {
			sscanf(line + 2, "%f %f %f", pvertices, pvertices + 1, pvertices + 2);
      pvertices += 3;
    }

    if(line[0] == 'f') {
      int32_t a, b, c, d, e, f, g, h, i;
			sscanf(line + 2, "%i/%i/%i %i/%i/%i %i/%i/%i", &a, &b, &c, &d, &e, &f, &g, &h, &i);

      tri *tri = &m->tris[cnt];
      tri->v[0] = (vec3){ vertices[(a - 1) * 3], vertices[(a - 1) * 3 + 1], vertices[(a - 1) * 3 + 2] };
      tri->v[1] = (vec3){ vertices[(d - 1) * 3], vertices[(d - 1) * 3 + 1], vertices[(d - 1) * 3 + 2] };
      tri->v[2] = (vec3){ vertices[(g - 1) * 3], vertices[(g - 1) * 3 + 1], vertices[(g - 1) * 3 + 2] };
      m->centers[cnt] = tri_calc_center(tri);

      tri_data *tri_data = &m->tris_data[cnt++];
      tri_data->n[0] = (vec3){ normals[(c - 1) * 3], normals[(c - 1) * 3 + 1], normals[(c - 1) * 3 + 2] };
      tri_data->n[1] = (vec3){ normals[(f - 1) * 3], normals[(f - 1) * 3 + 1], normals[(f - 1) * 3 + 2] };
      tri_data->n[2] = (vec3){ normals[(i - 1) * 3], normals[(i - 1) * 3 + 1], normals[(i - 1) * 3 + 2] };
      tri_data->u[0] = uvs[(b - 1) * 2];
      tri_data->v[0] = uvs[(b - 1) * 2 + 1];
      tri_data->u[1] = uvs[(e - 1) * 2];
      tri_data->v[1] = uvs[(e - 1) * 2 + 1];
      tri_data->u[2] = uvs[(h - 1) * 2];
      tri_data->v[2] = uvs[(h - 1) * 2 + 1]; 
    }
  }

  fclose(f);

  free(uvs);
  free(normals);
  free(vertices);

  mesh_create_bvh(m);

  return m;
}
