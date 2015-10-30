#include "button.h"

void Button_init(Button* button) {
    button->pressedTime = 0;
    button->state = 0;
    button->taps = 0;
    button->id = 0;
}

// TODO: incorporate tap counting
void Button_update(Button* button) {
    if(button->state == PRESSEDUSED) {
        button->state = HELD;
    }
    if(button->state == PRESSED) {
        button->state = PRESSEDUSED;
    }
    if(button->state == RELEASEDUSED) {
        button->state = IDLE;
    }
    if(button->state == RELEASED) {
        button->state = RELEASEDUSED;
    }
}

bool Button_isPressed(Button* button) {
    return button != 0 && button->state == PRESSEDUSED;
}

bool Button_isHeld(Button* button) {
    return button != 0 && (button->state == PRESSEDUSED || button->state == HELD);
}

bool Button_isReleased(Button* button) {
    return button != 0 && button->state == RELEASEDUSED;
}
