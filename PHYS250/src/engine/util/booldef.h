#ifndef _BOOLDEF_
#define _BOOLDEF_

// if using Microsoft Visual Studio (with default settings), don't need to define bool as a char, sdl takes care of it
// if using GNU GCC (with default settings), have to define bool
#ifndef _MSC_VER

typedef unsigned char bool;
#define true 1
#define false 0

#else

#include <SDL2\SDL.h>

#endif

#endif
