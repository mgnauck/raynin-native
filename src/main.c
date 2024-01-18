#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "sutil.h"
#include "mutil.h"
#include "vec3.h"
#include "tri.h"
#include "ray.h"
#include "cfg.h"
#include "view.h"
#include "cam.h"
#include "mesh.h"

#define MAX_DISTANCE FLT_MAX

#define SCR_WIDTH   640
#define SCR_HEIGHT  480

uint32_t *scr_buf;

cfg   config;
view  curr_view;
cam   curr_cam;
mesh  *curr_mesh;

mesh *init_scene(size_t tri_cnt, cfg conf, view *v, cam *c)
{
  vec3 vertices[tri_cnt * 3];

  for(size_t i=0; i<tri_cnt * 3; i++)
    vertices[i] = vec3_rand();

  *c = (cam){ .vert_fov = 60.0f, .foc_dist = 3.0f, .foc_angle = 0.0f };
  cam_set(c, (vec3){ 0.0f, 0.0f, -2.0f }, (vec3){ 0.0f, 0.0f, 0.0f });

  view_calc(v, conf.width, conf.height, c);

  return mesh_create(vertices, tri_cnt * 3);
}

// MT ray-triangle: https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/raytri/
float intersect_tri(ray r, tri *t, float *u, float *v)
{
  // Vectors of two edges sharing vertex 0
  vec3 edge1 = vec3_sub(t->v[1], t->v[0]);
  vec3 edge2 = vec3_sub(t->v[2], t->v[0]);

  // Calculate determinant and u parameter later on
  vec3 pvec = vec3_cross(r.dir, edge2);
  float det = vec3_dot(edge1, pvec);

  if(fabsf(det) < EPSILON)
    // Ray in plane of triangle
    return MAX_DISTANCE;

  float inv_det = 1.0f / det;

  // Distance vertex 0 to origin
  vec3 tvec = vec3_sub(r.ori, t->v[0]);

  // Calculate parameter u and test bounds
  *u = vec3_dot(tvec, pvec) * inv_det;
  if(*u < 0.0 || *u > 1.0)
    return MAX_DISTANCE;

  // Prepare to test for v
  vec3 qvec = vec3_cross(tvec, edge1);

  // Calculate parameter u and test bounds
  *v = vec3_dot(r.dir, qvec) * inv_det;
  if(*v < 0.0f || *u + *v > 1.0f)
    return MAX_DISTANCE;

  // Calc distance
  return vec3_dot(edge2, qvec) * inv_det;
}

void set_pix(uint32_t x, uint32_t y, vec3 v)
{
  uint32_t r = min(255, (uint32_t)(255 * v.x));
  uint32_t g = min(255, (uint32_t)(255 * v.y));
  uint32_t b = min(255, (uint32_t)(255 * v.z));
  scr_buf[SCR_WIDTH * y + x] = 0xff << 24 | r << 16 | g << 8 | b;
}

void init(uint32_t width, uint32_t height)
{
  pcg_srand(42u, 303u);

  config = (cfg){ width, height, 5, 5 };

  curr_mesh = init_scene(16, config, &curr_view, &curr_cam);
}

bool update(float time)
{
  for(size_t j=0; j<SCR_HEIGHT; j++)
    for(size_t i=0; i<SCR_WIDTH; i++)
      set_pix(i, j, vec3_rand());

  return true;
}

void release()
{
  mesh_release(curr_mesh);
}

int main(int argc, char *argv[])
{
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    return 1;
  
  SDL_Window *window = SDL_CreateWindow("unik", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, 0);
  if(!window)
    return 1;

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer)
    return 1;

  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, SCR_WIDTH, SCR_HEIGHT);
  if(!texture)
    return 1;

  scr_buf = malloc(SCR_WIDTH * SCR_HEIGHT * sizeof(*scr_buf));

  init(SCR_WIDTH, SCR_HEIGHT);

  bool quit = false;
  uint64_t start = SDL_GetTicks64();
  uint64_t last = start;
  while(!quit)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT)
        quit = true;
      else if(event.type == SDL_KEYDOWN)
        quit = true;
    }

    uint64_t frame = SDL_GetTicks64() - last;
    char title[256];
    snprintf(title, 256, "%ld ms / %6.3f", frame, 1000.0f / frame);
    SDL_SetWindowTitle(window, title);
    last = SDL_GetTicks64();

    quit |= !update((last - start) / 1000.0f);

    SDL_UpdateTexture(texture, NULL, scr_buf, SCR_WIDTH * sizeof(*scr_buf));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  release();

  free(scr_buf);
 
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
 
  return 0;
}
