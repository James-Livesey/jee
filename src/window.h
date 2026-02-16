#ifndef WINDOW_H_
#define WINDOW_H_

#include <wchar.h>

#include "vector.h"
#include "event.h"

struct Window;

typedef void (*window_handler_t)(struct Window* window, Event event);

typedef enum {
    WINDOW_EDIT_MOVE_X          = 0x01,
    WINDOW_EDIT_MOVE_Y          = 0x02,
    WINDOW_EDIT_RESIZE_WIDTH    = 0x04,
    WINDOW_EDIT_RESIZE_HEIGHT   = 0x08
} WindowEditMode;

typedef struct WindowBounds {
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} WindowBounds;

typedef struct Window {
    unsigned int id;
    WindowBounds bounds;
    unsigned int min_width;
    unsigned int min_height;
    unsigned int max_width;
    unsigned int max_height;
    unsigned int z;
    wchar_t* surface;
    unsigned int cursor_x;
    unsigned int cursor_y;
    window_handler_t handler;
    void* metadata;
} Window;

extern Vector windows;
extern Window* focused_window;

void init_windows();
void report_windows();
void redraw_windows();
void redraw_all();
void dispatch_window_event(Event event);

void default_window_handler(Window* window, Event event);

Window* new_window(unsigned int width, unsigned int height, window_handler_t handler);
bool destroy_window(Window* window);
void window_focus(Window* window);
bool window_resize(Window* window, unsigned int width, unsigned int height);
WindowBounds window_get_outer_bounds(Window* window);
bool window_is_occluded(Window* window, unsigned int outer_x, unsigned int outer_y);
void window_redraw(Window* window);
void window_handle_event(Window* window, Event event);
void window_set_cursor(Window* window, unsigned int x, unsigned int y);
void window_print_wc(Window* window, wchar_t wc);
void window_print_wstr(Window* window, wchar_t* wstr);

#endif