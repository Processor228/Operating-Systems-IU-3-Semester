#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXIMUM_SIZE 256

int count(int n, char* s, char target) {
    int cnt = 0;
    for (int i = 0; i < n; ++i) {
        if (tolower(s[i]) == tolower(target)) {
            cnt++;
        }
    }
    return cnt;
}

int countAll(int n, char* s) {
    for (int i = 0; i < n; ++i) {
        printf("%c: %d", tolower(s[i]), count(n, s, s[i]));
        if (i != n-1)
            printf(", ");
    }
}

int main() {
    char string[MAXIMUM_SIZE];
    fscanf(stdin, "%s", string);
    countAll(strlen(string), string);

    return EXIT_SUCCESS;
}
