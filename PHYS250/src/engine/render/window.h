#ifndef _WINDOW_
#define _WINDOW_

#include <SDL2/SDL.h>
#include "surface.h"
#include "../util/booldef.h"

// TODO: add support for hardware rendering
// Platform dependent

typedef struct {
    SDL_Window* sdl_window;
    Surface* surface;
    bool isClosing;
} Window;

extern void Window_init(Window* window, const char* title, int width, int height);
extern void Window_deinit(Window* window);
extern void Window_updateSurface(Window* window);

#endif
