#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int fd;
FILE* out;

typedef struct pressed {
    bool E, X, C, A, P, L;
} pressed;

pressed pr = {false, false, false, false, false, false};

void termination() {
    close(fd);
    fclose(out);
    exit(EXIT_SUCCESS);
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
    }
}

void check_shortcut() {
    if (pr.P && pr.E && !pr.A && !pr.L && !pr.X && !pr.C) {
        printf("I passed the Exam!\n");
    }
    if (pr.C && pr.A && pr.P && !pr.X && !pr.E && !pr.L) {
        printf("Get some cappuccino!\n");
    }
    if (!pr.C && !pr.A && !pr.P && pr.E && pr.X && !pr.L) {
        printf("E + X pressed, termination!\n");
        termination();
    }
    if (pr.L && pr.A && !pr.P && !pr.C && !pr.X && !pr.E) {
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
    out = fopen("ex1.txt", "w");

    struct input_event event;
    introduce_shortcuts();

    while(true) {
        read(fd, &event, sizeof(event));
        if(event.type == EV_KEY) {

            switch (event.value) {
                case 1: // Pressed
                    printf("PRESSED: 0x%04x (%hu)\n", event.code, event.code);
                    fprintf(out, "PRESSED: 0x%04x (%hu)\n", event.code, event.code);
                    assign_flag_on_event(event.code, true);
                    break;
                case 0:  // released
                    printf("RELEASED: 0x%04x (%hu)\n", event.code, event.code);
                    fprintf(out, "RELEASED: 0x%04x (%hu)\n", event.code, event.code);
                    assign_flag_on_event(event.code, false);
                    break;
                case 2:  // repeared
                    printf("REPEATED: 0x%04x (%hu)\n", event.code, event.code);
                    fprintf(out, "REPEATED: 0x%04x (%hu)\n", event.code, event.code);
                    break;
            }

            check_shortcut();
        }
    }

    return EXIT_SUCCESS;
}

