#include <stdio.h>

#include "codeeditor.h"

Window* new_code_editor_window(unsigned int width, unsigned int height) {
    Window* window = new_window(width, height, code_editor_handler);

    window->metadata = 0;

    return window;
}

void code_editor_handler(Window* window, Event event) {
    if (event.type == EVENT_WINDOW_SURFACE_CLEAN) {
        wchar_t* wstr = L"Hello, world!";

        window_set_cursor(
            window,
            (window->bounds.width / 2) - (wcslen(wstr) / 2),
            window->bounds.height / 2
        );

        window_print_wstr(window, wstr);
    }

    if (event.type == EVENT_MOUSE_DOWN || (event.type == EVENT_MOUSE_MOVE && window->metadata == (void*)1)) {
        window_set_cursor(window, event.data.as_mouse.inner_x, event.data.as_mouse.inner_y);
        window_print_wc(window, L'#');
        window_redraw(window);

        window->metadata = (void*)1;
    }

    if (event.type == EVENT_MOUSE_UP) {
        window->metadata = 0;
    }

    if (event.type == EVENT_KEY_PRESS) {
        if (event.data.as_key.utf8[0] != '\0') {
            window_print_wc(window, event.data.as_key.utf8[0]);
            window_redraw(window);
        }
    }
}