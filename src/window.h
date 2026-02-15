#include <wchar.h>

#include "vector.h"

typedef struct Window {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int width;
    unsigned int height;
    wchar_t* surface;
} Window;

extern Vector windows;

void init_windows();
void report_windows();

Window* new_window(unsigned int width, unsigned int height);
bool destroy_window(Window* window);
void window_bring_to_front(Window* window);