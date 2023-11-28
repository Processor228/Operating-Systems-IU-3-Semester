#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int fd;
int fd_out;

typedef struct pressed {
    bool E, X, C, A, P, L, CAPS, SHIFT;
} pressed;

pressed pr = {false, false, false, false, false, false, false, false};

void termination() {
    close(fd);
    close(fd_out);
    exit(EXIT_SUCCESS);
}

void handle_CAPS(int code) {
    pr.CAPS = !pr.CAPS;
}

void assign_flag_on_event(int code, bool value) {
    switch (code) {
        case KEY_P:
            pr.P = value;
            break;
        case KEY_E:
            pr.E = value;
            break;
        case KEY_X:
            pr.X = value;
            break;
        case KEY_C:
            pr.C = value;
            break;
        case KEY_A:
            pr.A = value;
            break;
        case KEY_L:
            pr.L = value;
            break;
        case KEY_LEFTSHIFT:
            pr.SHIFT = value;
            break;
    }
}

bool is_uppercase() {
    return (pr.CAPS + pr.SHIFT) % 2;
}

void check_shortcut() {
    if (pr.P && pr.E) {
        printf("I passed the Exam!\n");
    }
    if (pr.C && pr.A && pr.P) {
        printf("Get some cappuccino!\n");
    }
    if (pr.E && pr.X) {
        printf("E + X pressed, termination!\n");
        termination();
    }
    if (pr.L && pr.A) {
        printf("L + A is pressed, you are going to Los-Angeles !\n");
    }
}

void introduce_shortcuts() {
    printf("The following are the shortcuts, available in this program:\n");
    printf("E + X - to terminate the program\n");
    printf("C + A + P - Get a cappuccino\n");
    printf("P + E - Pass an exam !\n");
    printf("L + A - Okay, I am buying you a ticket to Los Angeles !\n");
}

int main () {

    fd =  open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);
    fd_out = open("ex1.txt", O_RDWR);

    struct input_event event;
    introduce_shortcuts();

    while(true) {
        read(fd, &event, sizeof(event));
        if(event.type == EV_KEY) {

            switch (event.value) {
                case 1: // Pressed
                    printf("PRESSED: 0x%04x (%hu)\n", event.code, event.code);
                    assign_flag_on_event(event.code, true);
                    handle_CAPS(event.code);
                    break;
                case 0:  // released
                    printf("RELEASED: 0x%04x (%hu)\n", event.code, event.code);
                    assign_flag_on_event(event.code, false);
                    break;
                case 2:  // repeared
                    printf("REPEATED: 0x%04x (%hu)\n", event.code, event.code);
                    break;
            }

            check_shortcut();

        }
    }

    return EXIT_SUCCESS;
}

