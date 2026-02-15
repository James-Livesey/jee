#include <stdio.h>
#include <poll.h>

#include "term.h"
#include "window.h"

int main(int argc, char* argv[]) {
    enter_term_mode();

    init_windows();

    Window* window0 = new_window(20, 10);
    Window* window1 = new_window(20, 10);
    Window* window2 = new_window(20, 10);

    window1->bounds.x = 4;
    window1->bounds.y = 4;

    window2->bounds.x = 8;
    window2->bounds.y = 8;

    if (!window0 || !window1 || !window2) {
        fprintf(stderr, "Couldn't create windows\n");

        return 1;
    }

    clear_term();
    redraw_windows();
    fflush(stdout);

    while (!has_input()) {}
    get_input_char();

    clear_term();
    printf("Initial window report:\n");
    report_windows();

    while (!has_input()) {}
    get_input_char();

    window_focus(window1);

    clear_term();
    redraw_windows();
    fflush(stdout);

    while (!has_input()) {}
    get_input_char();

    clear_term();
    printf("After focus:\n");
    report_windows();

    if (!destroy_window(window0) || !destroy_window(window1) || !destroy_window(window2)) {
        fprintf(stderr, "Couldn't destroy windows\n");

        return 1;
    }

    printf("Testing succeeded\n");

    while (!has_input()) {}
    get_input_char();

    exit_term_mode();

    return 0;
}