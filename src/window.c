#include <stdlib.h>

#include "window.h"

Vector windows;

void init_windows() {
    new_vector(sizeof(Window*), &windows);
}

Window* new_window(unsigned int width, unsigned int height) {
    Window* window = malloc(sizeof(Window));

    if (!window) {
        return NULL;
    }

    window->x = 0;
    window->y = 0;
    window->width = width;
    window->height = height;
    window->surface = malloc(width * height * sizeof(wchar_t));

    if (!window->surface) {
        return NULL;
    }

    vector_push(&windows, window);

    return window;
}

bool destroy_window(Window* window) {
    if (!vector_remove(&windows, window)) {
        return false;
    }

    free(window);

    return true;
}