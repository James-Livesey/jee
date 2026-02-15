#include <stdio.h>
#include <string.h>

#include "event.h"
#include "term.h"
#include "window.h"

void dispatch_event(Event event) {
    dispatch_window_event(event);
}

void make_mouse_input_event() {
    char mouse_event_type = get_input_char();
    unsigned char mouse_x = get_input_char() - 33;
    unsigned char mouse_y = get_input_char() - 33;

    Event event = {
        .type = EVENT_MOUSE_DOWN,
        .data.as_mouse = {
            .x = mouse_x,
            .y = mouse_y,
            .button = BUTTON_NONE,
            .modifiers = 0
        }
    };

    if (mouse_event_type & 0x40) {
        event.type = EVENT_MOUSE_MOVE;
    }

    switch (mouse_event_type & 0x63) {
        case 0x20: event.data.as_mouse.button = BUTTON_1; break;        // x00xxx00
        case 0x21: event.data.as_mouse.button = BUTTON_2; break;        // x00xxx01
        case 0x22: event.data.as_mouse.button = BUTTON_3; break;        // x00xxx10
        case 0x23: event.type = EVENT_MOUSE_UP; break;                  // x00xxx11
        case 0x60: event.data.as_mouse.button = WHEEL_UP; break;        // x11xxx00
        case 0x61: event.data.as_mouse.button = WHEEL_DOWN; break;      // x11xxx01
    }

    if ((mouse_event_type & 0x1C) == 0x04) event.data.as_mouse.modifiers |= MOD_SHIFT;  // xxx001xx
    if ((mouse_event_type & 0x1C) == 0x08) event.data.as_mouse.modifiers |= MOD_META;   // xxx010xx
    if ((mouse_event_type & 0x1C) == 0x10) event.data.as_mouse.modifiers |= MOD_CTRL;   // xxx100xx

    dispatch_event(event);
}

void make_special_key_input_event(SpecialKey special, ModifierKey modifiers) {
    Event event = {
        .type = EVENT_KEY_PRESS,
        .data.as_key = {
            .special = special,
            .modifiers = modifiers
        }
    };

    memset(event.data.as_key.utf8, 0, 4);

    dispatch_event(event);
}

char get_short_special_key(char c) {
    switch (c) {
        case 'A': return KEY_UP;
        case 'B': return KEY_DOWN;
        case 'C': return KEY_RIGHT;
        case 'D': return KEY_LEFT;
        case 'F': return KEY_END;
        case 'H': return KEY_HOME;
        case 'P': return KEY_F1;
        case 'Q': return KEY_F2;
        case 'R': return KEY_F3;
        case 'S': return KEY_F4;
    }

    return KEY_NONE;
}

char process_long_special_key() {
    char code = 0;
    char first_char = 0;
    char last_char = 0;
    ModifierKey modifiers = 0;

    while (true) {
        char c = get_input_char();

        if (!first_char) {
            first_char = c;
        }

        if (c < '0' || c > '9') {
            last_char = c;
            break;
        }

        code *= 10;
        code += c - '0';
    }

    if (code == 0) {
        return first_char;
    }

    if (last_char == ';') {
        char modifiers_char = get_input_char();

        switch (modifiers_char) {
            case '2': modifiers = MOD_SHIFT; break;
            case '3': modifiers = MOD_ALT; break;
            case '4': modifiers = MOD_ALT | MOD_SHIFT; break;
            case '5': modifiers = MOD_CTRL; break;
            case '6': modifiers = MOD_CTRL | MOD_SHIFT; break;
        }
    }

    if (code == 1) {
        make_special_key_input_event(get_short_special_key(get_input_char()), modifiers);
    } else {
        if (last_char == ';') {
            get_input_char();
        }

        SpecialKey key = KEY_NONE;

        switch (code) {
            case 3: key = KEY_DEL; break;
            case 5: key = KEY_PGUP; break;
            case 6: key = KEY_PGDOWN; break;
            case 15: key = KEY_F5; break;
            case 17: key = KEY_F6; break;
            case 18: key = KEY_F7; break;
            case 19: key = KEY_F8; break;
            case 20: key = KEY_F9; break;
            case 21: key = KEY_F10; break;
            case 23: key = KEY_F11; break;
            case 24: key = KEY_F12; break;
        }

        make_special_key_input_event(key, modifiers);
    }

    return 0;
}

void make_escape_input_event() {
    char c1 = get_input_char();

    if (c1 != '[' && c1 != 'O') {
        return;
    }

    char c2 = process_long_special_key();

    if (!c2) {
        return;
    }

    if (c1 == 'O') {
        make_special_key_input_event(get_short_special_key(c2), 0);
        return;
    }

    if (c2 == 'M') {
        make_mouse_input_event();
        return;
    }

    make_special_key_input_event(get_short_special_key(c2), 0);

    return;
}

void make_input_event() {
    Event event = {
        .type = EVENT_KEY_PRESS,
        .data.as_key = {
            .special = KEY_NONE,
            .modifiers = 0
        }
    };

    memset(event.data.as_key.utf8, 0, 4);

    for (size_t i = 0; i < 4; i++) {
        char c = get_input_char();

        if (i == 0 && c == '\e') {
            make_escape_input_event();
            return;
        }

        event.data.as_key.utf8[i] = c;

        if ((c & 0xC0) != 0xC0) {
            break;
        }
    }

    dispatch_event(event);
}

void handle_events() {
    if (has_input()) {
        make_input_event();
    }
}