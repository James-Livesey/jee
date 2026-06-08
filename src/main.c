#include <stdio.h>
#include <poll.h>
#include <unistd.h>

#include "term.h"
#include "event.h"
#include "window.h"
#include "windows/codeeditor.h"

int main(int argc, char* argv[]) {
    enter_term_mode();

    init_windows();

    Window* window0 = new_code_editor_window(100, 30);
    Window* window1 = new_window(20, 10, default_window_handler);
    Window* window2 = new_window(20, 10, default_window_handler);

    window0->bounds.x = 4;
    window0->bounds.y = 4;

    code_editor_set_lines(window0, L"Hello, world!\nThis is a test!\nAnother line");

    window1->bounds.x = 8;
    window1->bounds.y = 8;

    window2->bounds.x = 12;
    window2->bounds.y = 12;
    window2->min_width = 10;
    window2->min_height = 5;
    window2->max_width = 40;
    window2->max_height = 15;

    if (!window0 || !window1 || !window2) {
        fprintf(stderr, "Couldn't create windows\n");

        return 1;
    }

    redraw_all();

    while (true) {
        handle_events();
        sleep(0);
    }

    if (!destroy_window(window0) || !destroy_window(window1) || !destroy_window(window2)) {
        fprintf(stderr, "Couldn't destroy windows\n");

        return 1;
    }
    
    exit_term_mode();

    printf("Testing succeeded\n");

    return 0;
}