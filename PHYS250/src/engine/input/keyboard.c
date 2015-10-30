#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>

void Keyboard_init(Keyboard* keyboard, int size) {
    keyboard->nKeys = size;
    keyboard->curKeyIndex = 0;
    keyboard->keys = (Button**) malloc(sizeof(Button*) * size);
    for (int i = 0; i < keyboard->curKeyIndex; i++) {
		keyboard->keys[i] = 0;
	}
}

void Keyboard_deinit(Keyboard* keyboard) {
    for (int i = 0; i < keyboard->curKeyIndex; i++) {
        if(keyboard->keys[i] != 0) {
            free(keyboard->keys[i]);
        }
    }
    free(keyboard->keys);
}

void Keyboard_handleEvent(Keyboard* keyboard, SDL_Event* event) {
    int keycode = event->key.keysym.sym;
    Button* key = Keyboard_getKey(keyboard, keycode);
    if(event->key.type == SDL_KEYDOWN) {
        if(key == 0) {
            Keyboard_addKey(keyboard, keycode);
        }
		else {
			key->state = PRESSED;
		}
    }
    else if(event->key.type == SDL_KEYUP) {
        if(key != 0) {
            key->state = RELEASED;
        }
    }
}

void Keyboard_update(Keyboard* keyboard) {
    for (int i = 0; i < keyboard->curKeyIndex; i++) {
		Button* key = keyboard->keys[i];
		if (key != 0) {
			Button_update(key);
		}
	}
}

Button* Keyboard_getKey(Keyboard* keyboard, int keycode) {
    for(int i = 0; i < keyboard->curKeyIndex; i++) {
		Button* key = keyboard->keys[i];
        if(key != 0 && key->id == keycode) {
            return key;
        }
    }
    return 0;
}

void Keyboard_addKey(Keyboard* keyboard, int keycode) {
    if(keyboard->curKeyIndex >= keyboard->nKeys) {
        // handle error better
        printf("%s\n", "Error: too many keys in key array");
        return;
    }
    Button* key = (Button*) malloc(sizeof(Button));
    Button_init(key);
    key->id = keycode;
    key->state = PRESSED;
    keyboard->keys[keyboard->curKeyIndex] = key;
    keyboard->curKeyIndex++;
}
