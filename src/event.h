#ifndef EVENT_H_
#define EVENT_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    EVENT_KEY_PRESS,
    EVENT_MOUSE_DOWN,
    EVENT_MOUSE_UP,
    EVENT_MOUSE_MOVE
} EventType;

typedef enum {
    BUTTON_NONE,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    WHEEL_UP,
    WHEEL_DOWN
} MouseButton;

typedef enum {
    KEY_NONE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_DEL,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_HOME,
    KEY_END,
    KEY_PGUP,
    KEY_PGDOWN
} SpecialKey;

typedef enum {
    MOD_CTRL    = 0x01,
    MOD_ALT     = 0x02,
    MOD_SHIFT   = 0x04,
    MOD_META    = 0x08
} ModifierKey;

typedef struct Event {
    EventType type;
    union {
        struct {
            char utf8[4];
            SpecialKey special;
            ModifierKey modifiers;
        } as_key;
        struct {
            unsigned char x;
            unsigned char y;
            MouseButton button;
            ModifierKey modifiers;
        } as_mouse;
    } data;
} Event;

void handle_events();

#endif