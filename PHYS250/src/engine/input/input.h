#ifndef _INPUT_
#define _INPUT_

#include "keyboard.h"
#include "../util/booldef.h"

// TODO: handle input in another thread so that the game keeps running when you drag the window and stuff
// Platform dependent

typedef struct {
    Keyboard* keyboard;
    bool isClosing;
} Input;

extern void Input_init(Input* input);
extern void Input_deinit(Input* input);
extern void Input_handleInput(Input* input);
extern void Input_update(Input* input);
extern void Input_enableKeyboard(Input* input, int size);
extern void Input_disableKeyboard(Input* input);

#endif
