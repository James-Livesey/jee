#ifndef WINDOWS_CODEEDITOR_H_
#define WINDOWS_CODEEDITOR_H_

#include "../window.h"
#include "codeline.h"

typedef struct CodeEditor {
    bool initially_drawn;
    struct CodeLine* first_line;
    struct CodeLine* last_line;
    bool line_numbers_computed;
    struct CodeLine* line_number_recompute_from;
    unsigned int last_line_number;
    unsigned int scroll_x;
    unsigned int scroll_y;
} CodeEditor;

Window* new_code_editor_window(unsigned int width, unsigned int height);
void code_editor_recompute_line_numbers(Window* window);
void code_editor_handler(Window* window, Event event);
void code_editor_clear_all(Window* window);
bool code_editor_add_line(Window* window, const wchar_t* data);
bool code_editor_set_lines(Window* window, const wchar_t* data);

#endif