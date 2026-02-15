#include <stdio.h>

#include "window.h"

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    init_windows();

    Window* window0 = new_window(10, 10);
    Window* window1 = new_window(10, 10);
    Window* window2 = new_window(10, 10);

    if (!window0 || !window1 || !window2) {
        fprintf(stderr, "Couldn't create windows\n");

        return 1;
    }

    printf("Initial window report:\n");

    report_windows();

    window_bring_to_front(window1);

    printf("After bring to front:\n");

    report_windows();

    if (!destroy_window(window0) || !destroy_window(window1) || !destroy_window(window2)) {
        fprintf(stderr, "Couldn't destroy windows\n");

        return 1;
    }

    printf("Testing succeeded\n");

    return 0;
}