#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define STR_SIZE 256

struct Thread {
    pthread_t id;
    int i;
    char message[STR_SIZE];
};

void* thread_routine(void* args) {
    struct Thread* trd_info = (struct Thread*) args;
    sprintf(trd_info->message, "Hello from thread %d", trd_info->i);
    printf("%s, pthread id: %ld\n", trd_info->message, trd_info->id);
    return NULL;
}

int main() {
    int n;
    scanf("%d", &n);

    struct Thread threads[n];

    for (int i = 0; i < n; i++)
    {
        threads[i].i = i;
        pthread_create(&threads[i].id, NULL, thread_routine, &threads[i]);

        printf("Thread %d is created\n", i);

        // But if we will wait for each thread
        pthread_join(threads[i].id, NULL);
        /* We get:
         * Thread 0 is created
         * Hello from thread 0, pthread id: 140128309737024
         * Thread 1 is created
         * Hello from thread 1, pthread id: 140128309737024
         * Thread 2 is created
         * Hello from thread 2, pthread id: 140128309737024
         */
    }

//    sleep(2); // if we will sleep
    // but not wait for each thread to terminate, we may see the following:
    /*
     * Thread 0 is created
     * Hello from thread 0, pthread id: 140658905970240
     * Hello from thread 1, pthread id: 140658897577536
     * Thread 1 is created
     * Thread 2 is created
     * Hello from thread 2, pthread id: 140658889184832
     */
    return EXIT_SUCCESS;
}
