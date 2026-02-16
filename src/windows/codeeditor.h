#ifndef WINDOWS_CODEEDITOR_H_
#define WINDOWS_CODEEDITOR_H_

#include "../window.h"

Window* new_code_editor_window(unsigned int width, unsigned int height);
void code_editor_handler(Window* window, Event event);

#endif