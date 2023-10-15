#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define FIRST_FILE "/var/run/agent.pid"
#define BUFFER_SIZE 128

pid_t agent_pid;

void termination(int signum) {
       kill(agent_pid, SIGTERM);
}

int main () {
    signal(SIGTERM, termination);

    FILE* file;
    file = fopen(FIRST_FILE, "r");

    if (file == NULL) {
        printf("Agent not found\n");
        perror("Reason");
        return EXIT_FAILURE;
    }

    fscanf(file, "%d", &agent_pid);
    fclose(file);
    printf("agent found. PID %d\n", agent_pid);

    char buffer[BUFFER_SIZE];
    do {
        printf("Choose a command {\"read\", \"exit\", \"stop\", \"continue\"}\n");
        scanf("%s", buffer);
        if (strcmp(buffer, "read") == 0) {
            kill(agent_pid, SIGUSR1);
            continue;
        }
        if (strcmp(buffer, "stop") == 0) {
            kill(agent_pid, SIGSTOP);
            continue;
        }
        if (strcmp(buffer, "continue") == 0) {
            kill(agent_pid, SIGCONT);
            continue;
        }
    } while (strcmp(buffer, "exit") != 0);

    /*
     * Terminating the agent
     */
    kill(agent_pid, SIGUSR2);

    return EXIT_SUCCESS;
}
