#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <poll.h>

#include "term.h"

struct termios original_term;

void enter_term_mode() {
    struct termios term;

    tcgetattr(fileno(stdin), &term);
    
    original_term = term;

    term.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(fileno(stdin), TCSANOW, &term);

    printf("\e[?1049h"); // Enter alternate screen
}

void exit_term_mode() {
    printf("\e[?1049l"); // Exit alternate screen

    tcsetattr(fileno(stdin), TCSANOW, &original_term);
}

void clear_term() {
    printf("\e[2J");
    set_cursor_pos(0, 0);
}

TermBounds get_term_bounds() {
    struct winsize size;

    ioctl(fileno(stdout), TIOCGWINSZ, &size);

    return (TermBounds) {.width = size.ws_col, .height = size.ws_row};
}

void set_cursor_pos(unsigned int x, unsigned int y) {
    printf("\e[%d;%dH", y + 1, x + 1);
}

bool has_input() {
    struct pollfd fds[1];

    fds[0].fd = fileno(stdin);
    fds[0].events = POLLIN;

    return poll(fds, 1, 0) != 0;
}

char get_input_char() {
    return getchar();
}