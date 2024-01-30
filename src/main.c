#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "sutil.h"
#include "mutil.h"
#include "vec3.h"
#include "ray.h"
#include "cfg.h"
#include "view.h"
#include "cam.h"
#include "mesh.h"
#include "bvh.h"
#include "tlas.h"
#include "intersect.h"

#define WIDTH         1024
#define HEIGHT        768

#define MOVE_VEL      0.2f
#define LOOK_VEL      0.005f

#define INSTANCE_CNT  256

//#define NO_KEY_OR_MOUSE_HANDLING

bool          quit = false;
SDL_Surface   *screen;

cfg           config;
view          curr_view;
cam           curr_cam;
mesh          *curr_mesh;
bvh_inst      instances[INSTANCE_CNT];
tlas          scene_tlas;

vec3          positions[INSTANCE_CNT];
vec3          directions[INSTANCE_CNT];
vec3          orientations[INSTANCE_CNT];

bool          orbit_cam = false;

void set_pix(uint32_t x, uint32_t y, vec3 v)
{
  uint32_t r = min(255, (uint32_t)(255 * v.x));
  uint32_t g = min(255, (uint32_t)(255 * v.y));
  uint32_t b = min(255, (uint32_t)(255 * v.z));
  ((uint32_t *)screen->pixels)[WIDTH * y + x] = 0xff << 24 | r << 16 | g << 8 | b;
}

void handle_keypress(const SDL_Keysym *key)
{
  switch(key->sym) {
    case SDLK_ESCAPE:
      quit = true;
      break;
    case SDLK_a:
      curr_cam.eye = vec3_add(curr_cam.eye, vec3_scale(curr_cam.right, -MOVE_VEL));
      break;
    case SDLK_d:
      curr_cam.eye = vec3_add(curr_cam.eye, vec3_scale(curr_cam.right, MOVE_VEL));
      break;
    case SDLK_w:
      curr_cam.eye = vec3_add(curr_cam.eye, vec3_scale(curr_cam.fwd, -MOVE_VEL));
      break;
    case SDLK_s:
      curr_cam.eye = vec3_add(curr_cam.eye, vec3_scale(curr_cam.fwd, MOVE_VEL));
      break;
    case SDLK_i:
      curr_cam.foc_dist += 0.1f;
      break;
    case SDLK_k:
      curr_cam.foc_dist = max(curr_cam.foc_dist - 0.1f, 0.1f);
      break;
    case SDLK_j:
      curr_cam.foc_angle = max(curr_cam.foc_angle - 0.1f, 0.1f);
      break;
    case SDLK_l:
      curr_cam.foc_angle += 0.1f;
      break;
    case SDLK_o:
      orbit_cam = !orbit_cam;
      break;
  }

  view_calc(&curr_view, config.width, config.height, &curr_cam);
}

void handle_mouse_motion(const SDL_MouseMotionEvent *event)
{
  float theta = min(max(acosf(-curr_cam.fwd.y) + (float)event->yrel * LOOK_VEL, 0.01f), 0.99f * PI);
  float phi = fmodf(atan2f(-curr_cam.fwd.z, curr_cam.fwd.x) + PI - (float)event->xrel * LOOK_VEL, 2.0f * PI);
  
  cam_set_dir(&curr_cam, vec3_spherical(theta, phi));
  
  view_calc(&curr_view, config.width, config.height, &curr_cam);
}

void init(uint32_t width, uint32_t height)
{
  pcg_srand(42u, 303u);

  config = (cfg){ width, height, 5, 5 };
  
  curr_cam = (cam){ .vert_fov = 60.0f, .foc_dist = 3.0f, .foc_angle = 0.0f };
  cam_set(&curr_cam, (vec3){ 0.0f, 0.0f, -6.5f }, (vec3){ 0.0f, 0.0f, 2.0f });
  
  view_calc(&curr_view, config.width, config.height, &curr_cam);
  
  curr_mesh = mesh_create_file("data/armadillo.tri", 30000);
  //curr_mesh = mesh_create_file("data/unity.tri", 12582);

  tlas_init(&scene_tlas, instances, INSTANCE_CNT);

  for(size_t i=0; i<INSTANCE_CNT; i++) {
	  positions[i] = vec3_scale(vec3_sub(vec3_rand(), (vec3){ 0.5f, 0.5f, 0.5f }), 4.0f);
	  directions[i] = vec3_scale(vec3_unit(positions[i]), 0.05f);
	  orientations[i] = vec3_scale(vec3_rand(), 2.5f);
  }
}

bool update(float time)
{
  if(orbit_cam) {
    float s = 0.3f;
    float r = 8.0f;
    float h = 0.0f;
    vec3 pos = (vec3){ r * sinf(time * s), h * sinf(time * s * 0.7f), r * cosf(time * s) };
    cam_set(&curr_cam, pos, vec3_neg(pos));
    view_calc(&curr_view, config.width, config.height, &curr_cam);
  }

  uint64_t start = SDL_GetTicks64();
  for(size_t i=0; i<INSTANCE_CNT; i++) {
    mat4 transform;
		
    mat4 rotx, roty, rotz;
    mat4_rot_x(rotx, orientations[i].x);
    mat4_rot_y(roty, orientations[i].y);
    mat4_rot_z(rotz, orientations[i].z);
    mat4_mul(transform, rotx, roty);
    mat4_mul(transform, transform, rotz);
     
    mat4 scale;
    mat4_scale(scale, 0.2f);
    mat4_mul(transform, transform, scale);
    
    mat4 translation;
    mat4_trans(translation, positions[i]);
    mat4_mul(transform, translation, transform);

    bvh_create_inst(&instances[i], curr_mesh->bvh, i, transform);
		
    positions[i] = vec3_add(positions[i], directions[i]);
    orientations[i] = vec3_add(orientations[i], directions[i]);

		if(positions[i].x < -3.0f || positions[i].x > 3.0f)
      directions[i].x *= -1.0f;
		if(positions[i].y < -3.0f || positions[i].y > 3.0f)
      directions[i].y *= -1.0f;
		if(positions[i].z < -3.0f || positions[i].z > 3.0f)
      directions[i].z *= -1.0f;
	}
  SDL_Log("[UPDATE] %lu ms", SDL_GetTicks64() - start);
  
  start = SDL_GetTicks64();
  tlas_build(&scene_tlas);
  SDL_Log("[TLAS] %lu ms", SDL_GetTicks64() - start);

  start = SDL_GetTicks64();
#define BLOCK_SIZE 4
  for(size_t j=0; j<HEIGHT; j+=BLOCK_SIZE) {
    for(size_t i=0; i<WIDTH; i+=BLOCK_SIZE) {
      for(size_t y=0; y<BLOCK_SIZE; y++) {
        for(size_t x=0; x<BLOCK_SIZE; x++) {
          ray r;
          ray_create_primary(&r, (float)(i + x), (float)(j + y), &curr_view, &curr_cam);
          hit h = (hit){ .t = MAX_DISTANCE };
          intersect_tlas(&r, &scene_tlas, &h);
          vec3 c = (h.t < MAX_DISTANCE) ?
            (vec3){ h.u, h.v, 1.0f - h.u - h.v } : (vec3){ 0.0f, 0.0f, 0.0f };
          set_pix(i + x, j + y, c);
        }
      }
    }
  }
  SDL_Log("[RENDER] %lu ms", SDL_GetTicks64() - start);
  
  return true;
}

void release()
{
  tlas_release(&scene_tlas);
  mesh_release(curr_mesh);
}

int main(int argc, char *argv[])
{
  int32_t code = EXIT_SUCCESS;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    return EXIT_FAILURE;
  
  SDL_Window *window = SDL_CreateWindow("unik",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  if(!window) {
    code = EXIT_FAILURE;
    goto clean_sdl;
  }

  screen = SDL_GetWindowSurface(window);
  if(!screen) {
    code = EXIT_FAILURE;
    goto clean_window;
  }

#ifndef NO_KEY_OR_MOUSE_HANDLING
  if(SDL_SetRelativeMouseMode(SDL_TRUE) < 0) {
    code = EXIT_FAILURE;
    goto clean_window;
  }
#endif

  init(WIDTH, HEIGHT);

  uint64_t  start = SDL_GetTicks64();
  uint64_t  last = start;

  while(!quit)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT)
        quit = true;
#ifndef NO_KEY_OR_MOUSE_HANDLING
      else if(event.type == SDL_KEYDOWN)
        handle_keypress(&event.key.keysym);
      else if(event.type == SDL_MOUSEMOTION)
        handle_mouse_motion(&event.motion);
#endif
    }

    uint64_t frame = SDL_GetTicks64() - last;
    char title[256];
    snprintf(title, 256, "%ld ms / %6.3f / %4.2fM rays/s", frame, 1000.0f / frame, WIDTH * HEIGHT / (frame * 1000.0f));
    SDL_SetWindowTitle(window, title);
    last = SDL_GetTicks64();

    quit |= !update((last - start) / 1000.0f);

    SDL_UpdateWindowSurface(window);
  }

  release();

clean_window:
  SDL_DestroyWindow(window);
clean_sdl:
  SDL_Quit();
 
  return code;
}
