#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <SDL.h>
#include "mutil.h"
#include "vec3.h"

#define SCR_WIDTH   640
#define SCR_HEIGHT  480

uint32_t *scr_buf;

void set_pix(uint32_t x, uint32_t y, vec3 v)
{
  uint32_t r = min(255, (uint32_t)(255 * v.x));
  uint32_t g = min(255, (uint32_t)(255 * v.y));
  uint32_t b = min(255, (uint32_t)(255 * v.z));
  scr_buf[SCR_WIDTH * y + x] = 0xff << 24 | r << 16 | g << 8 | b;
}

void init()
{
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

  init();

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
