#include <stdio.h>
#include <math.h>

#include "codeeditor.h"

Window* new_code_editor_window(unsigned int width, unsigned int height) {
    Window* window = new_window(width, height, code_editor_handler);

    if (!window) {
        return NULL;
    }

    CodeEditor* editor = malloc(sizeof(CodeEditor));

    if (!editor) {
        return NULL;
    }

    editor->initially_drawn = false;
    editor->first_line = NULL;
    editor->last_line = NULL;
    editor->line_numbers_computed = true;
    editor->line_number_recompute_from = NULL;
    editor->scroll_x = 0;
    editor->scroll_y = 0;

    window->metadata = editor;

    return window;
}

void code_editor_recompute_line_numbers(Window* window) {
    CodeEditor* editor = window->metadata;
    CodeLine* line = editor->line_number_recompute_from;
    unsigned int number = 0;

    if (editor->line_numbers_computed) {
        return;
    }

    if (!line) {
        line = editor->first_line;
    } else if (line->prev) {
        number = line->prev->number;
    }

    for (; line; line = line->next) {
        line->number = ++number;
    }
    
    editor->line_numbers_computed = true;
    editor->line_number_recompute_from = NULL;
    editor->last_line_number = number;
}

void code_editor_redraw(Window* window) {
    CodeEditor* editor = window->metadata;

    code_editor_recompute_line_numbers(window);

    window_clear(window);

    wchar_t line_number[8];

    CodeLine* line = editor->first_line;
    unsigned int line_number_digits = editor->last_line_number > 0 ? ceil(log10(editor->last_line_number)) + 1 : 1;

    if (line_number_digits < 3) {
        line_number_digits = 3;
    }

    if (line_number_digits > 7) {
        line_number_digits = 7;
    }

    for (unsigned int i = 0; i < editor->scroll_y; i++) {
        if (!line) {
            break;
        }

        line = line->next;
    }

    for (unsigned int y = 0; y < window->bounds.height; y++) {
        if (!line) {
            break;
        }

        swprintf(line_number, 8, L"% 7d", line->number);

        window_set_cursor(window, 0, y);
        window_print_wstr(window, line_number + (7 - line_number_digits));

        window_set_cursor(window, line_number_digits + 2, y);

        unsigned int i = 0;

        for (unsigned int x = window->cursor_x; x < window->bounds.width; x++) {
            wchar_t wc = line->buffer[i++];

            if (!wc) {
                break;
            }

            window_print_wc(window, wc);
        }

        line = line->next;
    }
}

void code_editor_handler(Window* window, Event event) {
    CodeEditor* editor = window->metadata;

    if (!editor) {
        return;
    }

    if (event.type == EVENT_TICK && !editor->initially_drawn) {
        code_editor_redraw(window);
        window_redraw(window);

        editor->initially_drawn = true;
    }

    if (event.type == EVENT_WINDOW_DESTROY) {
        code_editor_clear_all(window);

        free(editor);

        return;
    }

    if (event.type == EVENT_WINDOW_SURFACE_CLEAN) {
        code_editor_redraw(window);
    }
}

void code_editor_clear_all(Window* window) {
    CodeEditor* editor = window->metadata;

    while (editor->first_line) {
        destroy_code_line(editor, editor->first_line);
    }
}

bool code_editor_add_line(Window* window, const wchar_t* data) {
    CodeEditor* editor = window->metadata;
    CodeLine* line = new_code_line(editor, NULL);

    if (!line) {
        return false;
    }

    return set_code_line_buffer(line, data);
}

bool code_editor_set_lines(Window* window, const wchar_t* data) {
    CodeEditor* editor = window->metadata;

    code_editor_clear_all(window);

    wchar_t* buffer = malloc(sizeof(wchar_t));
    wchar_t* new_buffer;
    size_t line_length = 0;

    if (!buffer) {
        return false;
    }

    buffer[0] = L'\0';

    for (size_t i = 0; data[i]; i++) {
        if (data[i] == L'\r') {
            continue;
        }

        if (data[i] == L'\n') {
            if (!code_editor_add_line(window, buffer)) {
                free(buffer);

                return false;
            }

            new_buffer = realloc(buffer, sizeof(wchar_t));
            line_length = 0;

            if (!new_buffer) {
                free(buffer);

                return false;
            }

            buffer = new_buffer;
            buffer[0] = L'\0';

            continue;
        }

        new_buffer = realloc(buffer, sizeof(wchar_t) * (++line_length + 1));

        if (!new_buffer) {
            free(buffer);

            return false;
        }

        buffer = new_buffer;
        buffer[line_length - 1] = data[i];
        buffer[line_length] = L'\0';
    }

    bool result = code_editor_add_line(window, buffer);

    free(buffer);

    editor->scroll_x = 0;
    editor->scroll_y = 0;

    return result;
}