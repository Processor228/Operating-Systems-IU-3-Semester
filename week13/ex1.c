#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 32
#define DFS_PATH_SIZE 128

int n, m;
pthread_mutex_t* mutexes;
pthread_t* t_ids;

typedef struct routine_arg {
    int res_id;
    int tread_id;
} routine_arg;

int** usages;

void report_usage(const int count, const int* usage, const int trd_id) {
    printf("Currently, the thread %d possesses resources:\n", trd_id);
    for (int i = 0; i < count; i++) {
        if (usage[i] != -1)
            printf("%d ", usage[i]);
    }
    printf("\n");
}

void terminate() {
    free(t_ids);
    free(mutexes);
    exit(EXIT_SUCCESS);
}

typedef struct graph_node {
    bool is_resource;
    int id;
} g_node;

int* get_neighbours(g_node node) {
    if (node.is_resource) {


    } else {


    }
}

bool in_visited(g_node* visited, g_node node) {
    int c_neigh = (node.is_resource ? m : n);
    for (int i = 0; i < c_neigh; i++) {
        if (visited[i].is_resource == node.is_resource && visited[i].id == node.id)
            return true;
    }
    return false;
}

void dfs(const g_node node, g_node* visited) {
    int* neighbours = get_neighbours(node);
    int c_neigh = (node.is_resource ? m : n);

    for (int i = 0; i < c_neigh; i++) {
        if (neighbours[i] == -1) {
            continue;
        }

        if (in_visited(visited, node)) {
            printf("Deadlock detected!\n");
            terminate();
        }

        dfs(node, visited);
    }

}

void issue_deadlock_check(const g_node node) {
    g_node visited[DFS_PATH_SIZE];

    dfs(node, visited);
}

void* routine(void* _arg) {
    routine_arg* arg = (routine_arg*)_arg;
    report_usage(m, usages[arg->tread_id], arg->tread_id);


    if (pthread_mutex_trylock(&mutexes[arg->res_id]) != 0) {  // can't acquire the mutex. Deadlock is possible.
        g_node node = {false, arg->tread_id};
        issue_deadlock_check(node);
    }

    pthread_mutex_lock(&mutexes[arg->res_id]);

    /*
     * Occupying the resource.
     */
    usages[arg->tread_id][arg->res_id] = arg->res_id;

    sleep(3); // for some number of seconds.

    pthread_mutex_lock(&mutexes[arg->res_id]);

    usages[arg->tread_id][arg->res_id] = -1;
    free(arg);
}

int main(int argc, char* argv[]) {
//    if (argc != 3) {
//        printf("usage: %s <n> <m>", argv[0]);
//    }
//    m = atoi(argv[1]);
//    n = atoi(argv[2]);
    m = 4;
    n = 4;

    mutexes = malloc(sizeof(pthread_mutex_t) * m);
    t_ids = malloc(sizeof(pthread_t) * n);
    usages = malloc(sizeof(int*) * n);
    for (int i = 0; i < m; i++) {
        usages[i] = malloc(sizeof(int) * m);
        for (int j = 0; j < n; j++)
            usages[i][j] = -1;
    }

    for (int i = 0; i < m; i++) { // resources
        pthread_mutex_init(&mutexes[i], NULL);
    }

    for (int i = 0; i < n; i++) { // threads
        t_ids[i] = -1;
    }

    FILE* requests = fopen("input.txt", "r");
    char buffer[BUFFER_SIZE];

    fscanf(requests, "%s", buffer); // thread
    fscanf(requests, "%s", buffer); // resource

    int thread, resource;
    do {
        fscanf(requests, "%d", &thread);
        fscanf(requests, "%d", &resource);
        printf("%d %d\n", thread, resource);

        routine_arg* argument = (routine_arg*)malloc(sizeof(routine_arg));
        argument->tread_id = thread;
        argument->res_id = resource;

        pthread_t tid;
        pthread_create(&tid, NULL, routine, argument);

    } while (!feof(requests));

    printf("No deadlocks.\n");
    sleep(5);
    terminate();

    return EXIT_SUCCESS;
}
