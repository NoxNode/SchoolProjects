#ifndef _KEYBOARD_
#define _KEYBOARD_

#include <SDL2/SDL.h>
#include "button.h"
#include "../util/booldef.h"

// Platform dependent

typedef struct {
    Button** keys;
    int nKeys;
    int curKeyIndex;
} Keyboard;

extern void Keyboard_init(Keyboard* keyboard, int size);
extern void Keyboard_deinit(Keyboard* keyboard);
extern void Keyboard_handleEvent(Keyboard* keyboard, SDL_Event* event);
extern void Keyboard_update(Keyboard* keyboard);
// Note: keycodes means SDL's keycodes found here: https://wiki.libsdl.org/SDL_Keycode
extern Button* Keyboard_getKey(Keyboard* keyboard, int keycode);
// Note: keycodes means SDL's keycodes found here: https://wiki.libsdl.org/SDL_Keycode
extern void Keyboard_addKey(Keyboard* keyboard, int keycode);

#endif
