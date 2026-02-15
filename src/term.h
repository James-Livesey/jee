#ifndef GFX_H_
#define GFX_H_

#include <stdbool.h>

typedef struct TermBounds {
    unsigned int width;
    unsigned int height;
} TermBounds;

void enter_term_mode();
void exit_term_mode();
void clear_term();
TermBounds get_term_bounds();
void set_cursor_pos(unsigned int x, unsigned int y);
bool has_input();
char get_input_char();

#endif