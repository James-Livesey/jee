#ifndef WINDOWS_CODELINE_H_
#define WINDOWS_CODELINE_H_

#include "codeeditor.h"

struct CodeEditor;

typedef struct CodeLine {
    unsigned int number;
    wchar_t* buffer;
    size_t length;
    struct CodeLine* prev;
    struct CodeLine* next;
} CodeLine;

CodeLine* new_code_line(struct CodeEditor* editor, CodeLine* insert_before);
void destroy_code_line(struct CodeEditor* editor, CodeLine* line);
bool set_code_line_buffer(CodeLine* line, const wchar_t* data);

#endif