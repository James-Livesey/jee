#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "term.h"

Vector windows;

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

Window* new_window(unsigned int width, unsigned int height) {
    Window* window = malloc(sizeof(Window));

    if (!window) {
        return NULL;
    }

    window->id = windows.length;
    window->bounds.x = 0;
    window->bounds.y = 0;
    window->bounds.width = width;
    window->bounds.height = height;
    window->z = windows.length;
    window->focused = false;
    window->surface = malloc(width * height * sizeof(wchar_t));

    if (!window->surface) {
        return NULL;
    }

    vector_push(&windows, &window);

    return window;
}

bool destroy_window(Window* window) {
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
        
        if (current_window->z > window->z) {
            current_window->z--;
        }

        current_window->focused = false;
    }

    window->z = windows.length - 1;
    window->focused = true;

    vector_sort(&windows, window_sorter);
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
            int x = (int)outer_bounds.x + (int)x_offset;
            int y = (int)outer_bounds.y + (int)y_offset;

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
            bool focused = window->focused;

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
                unsigned int inner_x = x_offset - 1;
                unsigned int inner_y = y_offset - 1;

                wchar_t wstr[2];

                wstr[0] = window->surface[(inner_y * window->bounds.width) + inner_x];
                wstr[1] = 0;

                if (wstr[0] == 0) {
                    wstr[0] = ' ';
                }

                printf("%ls", wstr);

                continue;
            }
        }
    }
}