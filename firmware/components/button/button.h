#ifndef BUTTON_
#define BUTTON_

void button_init();

typedef enum btn_ {
    PRESS_SHORT,
    PRESS_MEDIUM,
    PRESS_LONG,
} ButtonPress;

#endif  // BUTTON_
