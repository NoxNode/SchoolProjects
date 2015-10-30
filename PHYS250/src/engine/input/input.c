#include "input.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

void Input_init(Input* input) {
    if(!SDL_WasInit(SDL_INIT_EVENTS)) {
        SDL_Init(SDL_INIT_EVENTS);
    }
    input->keyboard = 0;
    input->isClosing = false;
}

void Input_deinit(Input* input) {
    if(input->keyboard != 0) {
        Keyboard_deinit(input->keyboard);
        free(input->keyboard);
    }
}

void Input_handleInput(Input* input) {
    SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
			input->isClosing = true;
		}
        if(input->keyboard != 0) {
            Keyboard_handleEvent(input->keyboard, &event);
        }
	}
}

void Input_update(Input* input) {
    if (input->keyboard != 0) {
        Keyboard_update(input->keyboard);
	}
}

void Input_enableKeyboard(Input* input, int size) {
    if(input->keyboard != 0) {
        return;
    }
    input->keyboard = (Keyboard*) malloc(sizeof(Keyboard));
    Keyboard_init(input->keyboard, size);
}

void Input_disableKeyboard(Input* input) {
    if(input->keyboard == 0) {
        return;
    }
    Keyboard_deinit(input->keyboard);
    free(input->keyboard);
    input->keyboard = 0;
}
