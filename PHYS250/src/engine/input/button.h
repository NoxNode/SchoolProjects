#ifndef _BUTTON_
#define _BUTTON_

#define IDLE         0
#define PRESSED      1
#define PRESSEDUSED  2
#define HELD         3
#define RELEASED     4
#define RELEASEDUSED 5

#include "../util/booldef.h"

typedef struct {
    long pressedTime;
    int state;
    int taps;
    int id;
} Button;

extern void Button_init(Button* button);
extern void Button_update(Button* button);
extern bool Button_isPressed(Button* button);
extern bool Button_isHeld(Button* button);
extern bool Button_isReleased(Button* button);

#endif
