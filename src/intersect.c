#include "intersect.h"
#include "mutil.h"
#include "tri.h"

// GPU efficient slabs test [Laine et al. 2013; Afra et al. 2016]
// https://www.jcgt.org/published/0007/03/04/paper-lowres.pdf
bool intersect_slabs(ray r, vec3 min_ext, vec3 max_ext)
{
  vec3 t0 = vec3_mul(vec3_sub(min_ext, r.ori), r.inv_dir);
  vec3 t1 = vec3_mul(vec3_sub(max_ext, r.ori), r.inv_dir);

  return vec3_max_comp(vec3_min(t0, t1)) <= vec3_min_comp(vec3_max(t0, t1));
}

// Adapted from above (probably not optimal on CPU)
float intersect_aabb(ray r, vec3 min_ext, vec3 max_ext, float tmin, float tmax)
{
  vec3 t0 = vec3_mul(vec3_sub(min_ext, r.ori), r.inv_dir);
  vec3 t1 = vec3_mul(vec3_sub(max_ext, r.ori), r.inv_dir);

  float tnear = vec3_max_comp(vec3_min(t0, t1));
  float tfar = vec3_min_comp(vec3_max(t0, t1));

  return tnear <= tfar && tnear < tmin && tfar > EPSILON ? tnear : MAX_DISTANCE;
}

// MT ray-triangle: https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/raytri/
float intersect_tri(ray r, tri *t, float *u, float *v)
{
  // Vectors of two edges sharing vertex 0
  const vec3 edge1 = vec3_sub(t->v[1], t->v[0]);
  const vec3 edge2 = vec3_sub(t->v[2], t->v[0]);

  // Calculate determinant and u parameter later on
  const vec3 pvec = vec3_cross(r.dir, edge2);
  const float det = vec3_dot(edge1, pvec);

  if(fabsf(det) < EPSILON)
    // Ray in plane of triangle
    return MAX_DISTANCE;

  const float inv_det = 1.0f / det;

  // Distance vertex 0 to origin
  const vec3 tvec = vec3_sub(r.ori, t->v[0]);

  // Calculate parameter u and test bounds
  *u = vec3_dot(tvec, pvec) * inv_det;
  if(*u < 0.0 || *u > 1.0)
    return MAX_DISTANCE;

  // Prepare to test for v
  const vec3 qvec = vec3_cross(tvec, edge1);

  // Calculate parameter u and test bounds
  *v = vec3_dot(r.dir, qvec) * inv_det;
  if(*v < 0.0f || *u + *v > 1.0f)
    return MAX_DISTANCE;

  // Calc distance
  return vec3_dot(edge2, qvec) * inv_det;
}

