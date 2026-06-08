#include <wchar.h>

#include "codeline.h"

struct CodeLine* new_code_line(CodeEditor* editor, CodeLine* insert_before) {
    CodeLine* line = malloc(sizeof(CodeLine));

    if (!line) {
        return NULL;
    }

    line->number = insert_before ? insert_before->number : 1;
    line->buffer = malloc(sizeof(wchar_t));
    line->length = 0;
    line->prev = NULL;
    line->next = NULL;

    line->buffer[0] = L'\0';

    if (insert_before) {
        if (insert_before->prev) {
            line->prev = insert_before->prev;
            insert_before->prev->next = line;
        }

        line->next = insert_before;
        insert_before->prev = line;
    } else {
        if (editor->last_line) {
            editor->last_line->next = line;
        }

        editor->last_line = line;
    }

    if (!editor->first_line || editor->first_line == insert_before) {
        editor->first_line = line;
    }

    editor->line_numbers_computed = false;

    return line;
}

void destroy_code_line(CodeEditor* editor, CodeLine* line) {
    if (editor->first_line == line) {
        editor->first_line = line->next;
    }

    if (editor->last_line == line) {
        editor->last_line = line->prev;
    }

    if (line->prev) {
        line->prev->next = line->next;
    }

    if (line->next) {
        line->next->prev = line->prev;
    }

    free(line->buffer);
    free(line);

    editor->line_numbers_computed = false;
    editor->line_number_recompute_from = line->prev;
}

bool set_code_line_buffer(CodeLine* line, const wchar_t* data) {
    size_t length = wcslen(data);
    wchar_t* new_buffer = realloc(line->buffer, sizeof(wchar_t) * (length + 1));

    if (!new_buffer) {
        return false;
    }

    line->buffer = new_buffer;
    line->length = length;

    wcscpy(line->buffer, data);

    return true;
}