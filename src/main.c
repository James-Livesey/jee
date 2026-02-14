#include <stdio.h>

#include "window.h"

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    init_windows();

    Window* window = new_window(10, 10);

    if (!window) {
        fprintf(stderr, "Couldn't create window\n");

        return 1;
    }

    if (!destroy_window(window)) {
        fprintf(stderr, "Couldn't destroy window\n");

        return 1;
    }


    printf("Testing succeeded\n");

    return 0;
}