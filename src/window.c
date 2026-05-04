#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "term.h"

Vector windows;
Window* focused_window = NULL;

WindowEditMode window_edit_mode = 0;
int window_handle_x = 0;
int window_handle_y = 0;
unsigned int window_original_top = 0;
unsigned int window_original_bottom = 0;
unsigned int window_original_left = 0;
unsigned int window_original_right = 0;

void init_windows() {
    new_vector(sizeof(Window*), &windows);
}

void report_windows() {
    for (size_t i = 0; i < windows.length; i++) {
        Window* window = *(Window**)vector_get(&windows, i);

        printf("Window %d: z = %d\n", window->id, window->z);
    }
}

void redraw_windows() {
    for (size_t i = 0; i < windows.length; i++) {
        Window* window = *(Window**)vector_get(&windows, i);

        window_redraw(window);
    }
}

void redraw_all() {
    clear_term();
    redraw_windows();
}

void dispatch_window_event(Event event) {
    if (event.type == EVENT_KEY_PRESS && focused_window) {
        window_handle_event(focused_window, event);
    }

    if (event.type == EVENT_MOUSE_UP || event.type == EVENT_MOUSE_DOWN || event.type == EVENT_MOUSE_MOVE) {
        if (window_edit_mode != 0) {
            bool should_redraw = false;

            if (event.type == EVENT_MOUSE_UP) {
                window_edit_mode = 0;
            }

            if (event.type == EVENT_MOUSE_MOVE) {
                bool width_resize_prevented = false;
                bool height_resize_prevented = false;

                if (window_edit_mode & WINDOW_EDIT_RESIZE_WIDTH) {
                    int new_width = event.data.as_mouse.x - window_original_left;

                    if (window_edit_mode & WINDOW_EDIT_MOVE_X) {
                        new_width = window_original_right - event.data.as_mouse.x - 1;
                    }

                    if (new_width <= 0 || new_width < focused_window->min_width || new_width > focused_window->max_width) {
                        width_resize_prevented = true;
                    } else {
                        window_resize(
                            focused_window,
                            new_width,
                            focused_window->bounds.height
                        );

                        should_redraw = true;
                    }
                }

                if (window_edit_mode & WINDOW_EDIT_RESIZE_HEIGHT) {
                    int new_height = event.data.as_mouse.y - window_original_top;

                    if (window_edit_mode & WINDOW_EDIT_MOVE_Y) {
                        new_height = window_original_bottom - event.data.as_mouse.y - 1;
                    }

                    if (new_height <= 0 || new_height < focused_window->min_height || new_height > focused_window->max_height) {
                        height_resize_prevented = true;
                    } else {
                        window_resize(
                            focused_window,
                            focused_window->bounds.width,
                            new_height
                        );

                        should_redraw = true;
                    }
                }

                if ((window_edit_mode & WINDOW_EDIT_MOVE_X) && !width_resize_prevented) {
                    focused_window->bounds.x = event.data.as_mouse.x - window_handle_x;
                    should_redraw = true;
                }
    
                if ((window_edit_mode & WINDOW_EDIT_MOVE_Y) && !height_resize_prevented) {
                    focused_window->bounds.y = event.data.as_mouse.y - window_handle_y;
                    should_redraw = true;
                }
            }

            if (should_redraw) {
                redraw_all();
            }

            return;
        }

        for (size_t i = 0; i < windows.length; i++) {
            Window* window = *(Window**)vector_get(&windows, windows.length - i - 1);
            WindowBounds bounds = window_get_outer_bounds(window);

            if (
                event.data.as_mouse.x >= bounds.x &&
                event.data.as_mouse.y >= bounds.y &&
                event.data.as_mouse.x < bounds.x + bounds.width &&
                event.data.as_mouse.y < bounds.y + bounds.height
            ) {
                window_handle_event(window, event);
                break;
            }
        }
    }
}

void default_window_handler(Window* window, Event event) {}

Window* new_window(unsigned int width, unsigned int height, window_handler_t handler) {
    Window* window = malloc(sizeof(Window));

    if (!window) {
        return NULL;
    }

    window->id = windows.length;
    window->bounds.x = 0;
    window->bounds.y = 0;
    window->bounds.width = width;
    window->bounds.height = height;
    window->min_width = 0;
    window->min_height = 0;
    window->max_width = -1;
    window->max_height = -1;
    window->z = windows.length;
    window->surface = malloc(width * height * sizeof(wchar_t));
    window->cursor_x = 0;
    window->cursor_y = 0;
    window->handler = handler;
    window->metadata = NULL;

    if (!window->surface) {
        return NULL;
    }

    memset(window->surface, 0, width * height * sizeof(wchar_t));

    vector_push(&windows, &window);

    window->handler(window, (Event) {
        .type = EVENT_WINDOW_CREATE
    });

    window->handler(window, (Event) {
        .type = EVENT_WINDOW_SURFACE_CLEAN
    });

    return window;
}

bool destroy_window(Window* window) {
    window->handler(window, (Event) {
        .type = EVENT_WINDOW_DESTROY
    });

    if (window == focused_window) {
        focused_window = NULL;
    }

    if (!vector_remove(&windows, &window)) {
        return false;
    }

    free(window);

    return true;
}

int window_sorter(const void* a, const void* b) {
    return (int)(*(Window**)a)->z - (int)(*(Window**)b)->z;
}

void window_focus(Window* window) {
    for (size_t i = 0; i < windows.length; i++) {
        Window* current_window = *(Window**)vector_get(&windows, i);

        if (current_window != window) {
            window->handler(window, (Event) {
                .type = EVENT_WINDOW_BLUR
            });
        }
        
        if (current_window->z > window->z) {
            current_window->z--;
        }
    }

    window->z = windows.length - 1;

    focused_window = window;

    window->handler(window, (Event) {
        .type = EVENT_WINDOW_FOCUS
    });

    vector_sort(&windows, window_sorter);
}

bool window_resize(Window* window, unsigned int width, unsigned int height) {
    wchar_t* new_surface = realloc(window->surface, width * height * sizeof(wchar_t));

    if (!new_surface) {
        return false;
    }

    memset(new_surface, 0, width * height * sizeof(wchar_t));

    window->surface = new_surface;
    window->bounds.width = width;
    window->bounds.height = height;

    window->handler(window, (Event) {
        .type = EVENT_WINDOW_RESIZE
    });

    window->handler(window, (Event) {
        .type = EVENT_WINDOW_SURFACE_CLEAN
    });

    return true;
}

WindowBounds window_get_outer_bounds(Window* window) {
    return (WindowBounds) {
        .x = window->bounds.x - 1,
        .y = window->bounds.y - 1,
        .width = window->bounds.width + 2,
        .height = window->bounds.height + 2
    };
}

bool window_is_occluded(Window* window, unsigned int outer_x, unsigned int outer_y) {
    size_t index = vector_get_index(&windows, &window);

    if (index == -1) {
        return false;
    }

    for (unsigned int i = index + 1; i < windows.length; i++) {
        Window* other_window = *(Window**)vector_get(&windows, i);
        WindowBounds other_outer_bounds = window_get_outer_bounds(other_window);

        if (
            outer_x >= other_outer_bounds.x &&
            outer_y >= other_outer_bounds.y &&
            outer_x < other_outer_bounds.x + other_outer_bounds.width &&
            outer_y < other_outer_bounds.y + other_outer_bounds.height
        ) {
            return true;
        }
    }

    return false;
}

void window_redraw(Window* window) {
    WindowBounds outer_bounds = window_get_outer_bounds(window);
    TermBounds term_bounds = get_term_bounds();
    bool needs_set_pos = true;

    for (unsigned int y_offset = 0; y_offset < outer_bounds.height; y_offset++) {
        needs_set_pos = true;

        for (unsigned int x_offset = 0; x_offset < outer_bounds.width; x_offset++) {
            int x = outer_bounds.x + x_offset;
            int y = outer_bounds.y + y_offset;

            if (x < 0 || y < 0 || x >= term_bounds.width || y >= term_bounds.height) {
                needs_set_pos = true;

                continue;
            }

            if (window_is_occluded(window, x, y)) {
                needs_set_pos = true;

                continue;
            }

            if (needs_set_pos) {
                set_cursor_pos(x, y);

                needs_set_pos = false;
            }

            bool left_edge = x_offset == 0;
            bool right_edge = x_offset == window->bounds.width + 1;
            bool top_edge = y_offset == 0;
            bool bottom_edge = y_offset == window->bounds.height + 1;
            bool focused = window == focused_window;

            if (top_edge && left_edge)      {printf(focused ? "╔" : "┌"); continue;}
            if (top_edge && right_edge)     {printf(focused ? "╗" : "┐"); continue;}
            if (bottom_edge && left_edge)   {printf(focused ? "╚" : "└"); continue;}
            if (bottom_edge && right_edge)  {printf(focused ? "╝" : "┘"); continue;}
            if (top_edge || bottom_edge)    {printf(focused ? "═" : "─"); continue;}
            if (left_edge || right_edge)    {printf(focused ? "║" : "│"); continue;}

            if (
                x_offset > 0 && x_offset <= window->bounds.width &&
                y_offset > 0 && y_offset <= window->bounds.height
            ) {
                int inner_x = x_offset - 1;
                int inner_y = y_offset - 1;

                wchar_t wstr[2];

                wstr[0] = window->surface[(inner_y * window->bounds.width) + inner_x];
                wstr[1] = 0;

                if (wstr[0] == L'\0' || wstr[0] == 0x7F) {
                    wstr[0] = ' ';
                }

                printf("%ls", wstr);

                if (x_offset == window->bounds.width) {
                    needs_set_pos = true;
                } else {
                    x_offset += wcwidth(wstr[0]) - 1;
                }

                continue;
            }
        }
    }

    fflush(stdout);
}

void window_handle_event(Window* window, Event event) {
    WindowBounds bounds = window_get_outer_bounds(window);

    if (event.type == EVENT_MOUSE_DOWN && event.data.as_mouse.button == BUTTON_1) {
        Window* prev_focused_window = focused_window;

        if (prev_focused_window != window) {
            window_focus(window);

            if (prev_focused_window) {
                window_redraw(prev_focused_window);
            }

            window_redraw(window);
            fflush(stdout);
        }

        window_edit_mode = 0;
        window_handle_x = event.data.as_mouse.x - window->bounds.x;
        window_handle_y = event.data.as_mouse.y - window->bounds.y;
        window_original_top = window->bounds.y;
        window_original_bottom = window->bounds.y + window->bounds.height;
        window_original_left = window->bounds.x;
        window_original_right = window->bounds.x + window->bounds.width;

        window_edit_mode = 0;

        if (event.data.as_mouse.x == bounds.x) {
            window_edit_mode |= WINDOW_EDIT_MOVE_X | WINDOW_EDIT_RESIZE_WIDTH;
        }

        if (event.data.as_mouse.x == bounds.x + bounds.width - 1) {
            window_edit_mode |= WINDOW_EDIT_RESIZE_WIDTH;
        }

        if (event.data.as_mouse.y == bounds.y) {
            window_edit_mode |= WINDOW_EDIT_MOVE_Y;

            if (event.data.as_mouse.x == bounds.x) {
                window_edit_mode |= WINDOW_EDIT_RESIZE_HEIGHT;
            }

            if (event.data.as_mouse.x == bounds.x + bounds.width - 1) {
                window_edit_mode |= WINDOW_EDIT_RESIZE_HEIGHT;
            } else {
                window_edit_mode |= WINDOW_EDIT_MOVE_X;
            }
        }

        if (event.data.as_mouse.y == bounds.y + bounds.height - 1) {
            window_edit_mode |= WINDOW_EDIT_RESIZE_HEIGHT;
        }
    }

    if (event.type == EVENT_MOUSE_UP || event.type == EVENT_MOUSE_DOWN || event.type == EVENT_MOUSE_MOVE) {
        if (
            event.data.as_mouse.x >= window->bounds.x &&
            event.data.as_mouse.y >= window->bounds.y &&
            event.data.as_mouse.x < window->bounds.x + window->bounds.width &&
            event.data.as_mouse.y < window->bounds.y + window->bounds.height
        ) {
            event.data.as_mouse.inner_x = event.data.as_mouse.x - window->bounds.x;
            event.data.as_mouse.inner_y = event.data.as_mouse.y - window->bounds.y;

            window->handler(window, event);
        }

        return;
    }

    window->handler(window, event);
}

void window_set_cursor(Window* window, unsigned int x, unsigned int y) {
    window->cursor_x = x;
    window->cursor_y = y;
}

void window_print_wc(Window* window, wchar_t wc) {
    if (window->cursor_x >= window->bounds.width || wc == L'\n') {
        window->cursor_x = 0;
        window->cursor_y += 1;
    }

    if (wc == L'\n') {
        return;
    }

    size_t index = (window->cursor_y * window->bounds.width) + window->cursor_x;

    if (index >= window->bounds.width * window->bounds.height) {
        window->cursor_x = 0;
        window->cursor_y = 0;
        index = 0;
    }

    window->surface[index] = wc;

    window->cursor_x += wcwidth(wc);
}

void window_print_wstr(Window* window, wchar_t* wstr) {
    while (*wstr) {
        window_print_wc(window, *(wstr++));
    }
}