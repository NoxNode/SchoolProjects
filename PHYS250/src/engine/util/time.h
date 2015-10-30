#ifndef _TIME_
#define _TIME_

#include <SDL2/SDL.h>

// Platform dependent (on SDL)

// requires that sdl be initialized already
unsigned int getTime() {
    return SDL_GetTicks();
}

#endif
