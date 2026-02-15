#include <stdio.h>
#include <stdlib.h>

#include "window.h"

Vector windows;

void init_windows() {
    new_vector(sizeof(Window*), &windows);
}

void report_windows() {
    for (size_t i = 0; i < windows.length; i++) {
        Window* window = *(Window**)vector_get(&windows, i);

        printf("Window %d: z = %d\n", i, window->z);
    }
}

Window* new_window(unsigned int width, unsigned int height) {
    Window* window = malloc(sizeof(Window));

    if (!window) {
        return NULL;
    }

    window->x = 0;
    window->y = 0;
    window->z = windows.length;
    window->width = width;
    window->height = height;
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
    return (int)((Window*)a)->z - (int)((Window*)b)->z;
}

void window_bring_to_front(Window* window) {
    for (size_t i = 0; i < windows.length; i++) {
        Window* current_window = *(Window**)vector_get(&windows, i);

        if (current_window->z > window->z) {
            current_window->z--;
        }
    }

    window->z = windows.length - 1;

    vector_sort(&windows, window_sorter);
}