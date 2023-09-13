#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXIMUM_CHARS 256  /// according to the assignment conditions

int main() {
    char input[MAXIMUM_CHARS];

    int read = 0;
    for (char cur_char = 'a'; cur_char != '.' && (read < MAXIMUM_CHARS); ++read) {
        fscanf(stdin, "%c", &cur_char);
        if (cur_char == '.')
            break;

        input[read] = cur_char;
    }

    /*
     * Just reading the string backwards
     */
    printf("\"");
    for (int i = strlen(input)-1; i >= 0; i--) { // add '\0' when writing char* by individual characters !
        printf("%c", input[i]);
    }
    printf("\"\n");

    return EXIT_SUCCESS;
}
