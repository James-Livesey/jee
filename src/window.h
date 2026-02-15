#ifndef WINDOW_H_
#define WINDOW_H_

#include <wchar.h>

#include "vector.h"

typedef struct WindowBounds {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} WindowBounds;

typedef struct Window {
    unsigned int id;
    WindowBounds bounds;
    unsigned int z;
    bool focused;
    wchar_t* surface;
} Window;

extern Vector windows;

void init_windows();
void report_windows();
void redraw_windows();

Window* new_window(unsigned int width, unsigned int height);
bool destroy_window(Window* window);
void window_focus(Window* window);
WindowBounds window_get_outer_bounds(Window* window);
bool window_is_occluded(Window* window, unsigned int outer_x, unsigned int outer_y);
void window_redraw(Window* window);

#endif