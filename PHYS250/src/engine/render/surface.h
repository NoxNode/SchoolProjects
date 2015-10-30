#ifndef _SURFACE_
#define _SURFACE_

#include <SDL2/SDL.h>
#include "../util/booldef.h"

// TODO: handle locking
// TODO: handle pixel format and stuff
// Platform dependent

typedef struct {
    SDL_Surface* sdl_surface;
    bool mustLock;
} Surface;

extern void Surface_init(Surface* surface, SDL_Surface* sdl_surface, bool mustLock);
extern void Surface_freeSurface(Surface* surface);
extern void Surface_fill(Surface* surface, int color);
extern void Surface_fillRect(Surface* surface, int color, int x, int y, int width, int height);
extern void Surface_blitSurface(Surface* src_surface, Surface* dest_surface, int x, int y, int width, int height);
// surface->sdl_surface must already exist
extern void Surface_loadSurface(Surface* surface, const char* fileName);

#endif
