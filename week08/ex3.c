#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>

#define ALLOC 10485760

int main() {

    for (int i = 0; i < 10; i++) {
        /* Unfortunately we can't...
            calloc(ALLOC, 1);
        */
        void* ptr = malloc(ALLOC);
        memset(ptr, 0, ALLOC);

        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        printf("idrss: %ld,\nisrss: %ld,\nixrss: %ld,\nmaxrss: %ld\n",
               usage.ru_idrss, usage.ru_isrss, usage.ru_ixrss, usage.ru_maxrss);
        sleep(1);
    }

    return EXIT_SUCCESS;
}
