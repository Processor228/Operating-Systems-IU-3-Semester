#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <limits.h>
#define PS_MAX 10

// holds the scheduling data of one process
typedef struct{
    int idx; // process idx (index)
    int done;
    int quant;
    int at, bt, rt, wt, ct, tat; // arrival time, burst time, response time, waiting time, completion time, turnaround time.
    int burst; // remaining burst (this should decrement when the process is being executed)
} ProcessData;

int queue[PS_MAX];
int cur_proc_que_idx = 0;
int queue_size = 0;
int available = 0;

// the idx of the running process
int running_process = -1; // -1 means no running processes

// the total time of the timer
unsigned total_time; // should increment one second at a time by the scheduler

// data of the processes
ProcessData data[PS_MAX]; // array of process data

// array of all process pids
pid_t ps[PS_MAX]; // zero valued pids - means the process is terminated or not created yet

// size of data array
unsigned data_size;
unsigned quantum_size;

void read_file(FILE* file){
    // and store them in the array {data}
    // initialize ps array to zeros (the process is terminated or not created yet)
    char buffer[32];
    int i = 0;
    fscanf(file, "%[^\n]%*c", buffer);
    do {
        fscanf(file, "%d", &data[i].idx);
        fscanf(file, "%d", &data[i].at);
        fscanf(file, "%d", &data[i].bt);
        data[i].burst = data[i].bt;
        ps[i] = 0;
        i++;
    } while (!feof(file));
    data_size = i;
}

// send signal SIGCONT to the worker process
void resume(pid_t process) {
    if (ps[process] == 0)
        return;
    kill(ps[process], SIGCONT);
}

// send signal SIGTSTP to the worker process
void suspend(pid_t process) {
    if (ps[process] == 0)
        return;
    kill(ps[process], SIGTSTP);
}

// send signal SIGTERM to the worker process
void terminate(pid_t process) {
    if (ps[process] == 0)
        return;
    kill(ps[process], SIGTERM);
    ps[process] = 0;
}

// create a process using fork
void create_process(int new_process) {
    suspend(running_process);
    running_process = new_process;
    ps[running_process] = fork();
    if (ps[running_process] == 0) {
        char strnum[8];
        sprintf(strnum, "%d", running_process);
        char* args[] = {"./wor", strnum, NULL};
        execve("./wor", args, NULL);
    } else {
        data[running_process].rt = total_time - data[running_process].at;
    }
}

void update_queue() {
    for(int i=0; i < data_size; i++) {
        if (data[i].at == total_time) {  // if this process just came, we need to put it in the end of the queue.
            queue[queue_size] = i;
            queue_size++;
            available++;
        }
    }
}

// find next process for running
ProcessData find_next_process() {
    // location of next process in {data} array
    int location = 0;
//    update_queue();
    printf("queue: ");
    for (int i = 0; i < queue_size; i++) printf("%d ", queue[i]);
    printf("\n");

     do {
        cur_proc_que_idx++;
        if (cur_proc_que_idx >= queue_size)
            cur_proc_que_idx = 0;

    } while (data[queue[cur_proc_que_idx]].done && available != 0);

    location = queue[cur_proc_que_idx];

    // if next_process did not arrive so far,
    // then we recursively call this function after incrementing total_time
    if(available == 0) {
        printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, location);
        // increment the time
        total_time++;
        update_queue();
        return find_next_process();
    }

    // return the data of next process
    return data[location];
}

// reports the metrics and simulation results
void report(){
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i=0; i< data_size; i++){
        printf("process %d: \n", i);
        printf("	at=%d\n", data[i].at);
        printf("	bt=%d\n", data[i].bt);
        printf("	ct=%d\n", data[i].ct);
        printf("	wt=%d\n", data[i].wt);
        printf("	tat=%d\n", data[i].tat);
        printf("	rt=%d\n", data[i].rt);
        sum_wt += data[i].wt;
        sum_tat += data[i].tat;
    }

    printf("data size = %d\n", data_size);
    float avg_wt = (float)sum_wt/data_size;
    float avg_tat = (float)sum_tat/data_size;
    printf("Average results for this run:\n");
    printf("	avg_wt=%f\n", avg_wt);
    printf("	avg_tat=%f\n", avg_tat);
}

void check_burst(){
    for(int i = 0; i < data_size; i++) {
        if (data[i].burst > 0)
            return;
    }
    sleep(1);
    // report simulation results
    report();
    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}

// This function is called every one second as handler for SIGALRM signal
void schedule_handler(int signum) {
    // increment the total time
    total_time++;
    update_queue();
    data[running_process].burst--;
    data[running_process].quant--;
    printf("Scheduler: Runtime: %d seconds\n", total_time);
    printf("Process %d is running with %d seconds left\n", running_process, data[running_process].burst);
    printf("Quants remain %d\n", data[running_process].quant);
    if (data[running_process].burst <= 0 && running_process != -1) {
        data[running_process].ct = total_time;  // completion time
        data[running_process].tat = total_time - data[running_process].at; // turnaround time
        data[running_process].wt = total_time - data[running_process].at - data[running_process].bt;  // waiting time
        printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
        terminate(running_process);
        data[running_process].done = 1;
        available--;
    } else {
        if (running_process != -1 && data[running_process].quant > 0)
            return;
    }
    check_burst();

    if (data[running_process].quant <= 0)
        suspend(running_process);

    ProcessData next_process = find_next_process();
    running_process = next_process.idx;
    data[running_process].quant = quantum_size;

    if (data[running_process].bt == data[running_process].burst) {  // the process was not created yet.
        printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
        create_process(running_process);
    } else {
        resume(running_process);
    }
}

int main(int argc, char *argv[]) {
    // read the data file
    char * argv1 = argv[1];
    FILE *in_file  = fopen(argv1, "r");
    if (in_file == NULL) {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    } else {
        read_file(in_file);
    }
    fclose(in_file);

    scanf("%d", &quantum_size);
    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second at a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
//    running_process = find_next_process().idx;
    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);
    // Wait till all processes finish
    while(1); // infinite loop
}
