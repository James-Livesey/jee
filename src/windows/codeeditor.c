#include <stdio.h>

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

    editor->first_line = NULL;
    editor->last_line = NULL;
    editor->line_numbers_computed = true;
    editor->line_number_recompute_from = NULL;

    window->metadata = editor;

    return window;
}

void code_editor_handler(Window* window, Event event) {
    CodeEditor* editor = window->metadata;

    if (event.type == EVENT_WINDOW_DESTROY) {
        code_editor_clear_all(window);

        free(editor);

        return;
    }

    if (event.type == EVENT_WINDOW_SURFACE_CLEAN) {
        wchar_t* wstr = L"Hello, world!";

        window_set_cursor(
            window,
            (window->bounds.width / 2) - (wcslen(wstr) / 2),
            window->bounds.height / 2
        );

        window_print_wstr(window, wstr);
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

    return result;
}