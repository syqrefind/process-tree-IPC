// This program is used to send signals to the program of partc.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int main(int argc, char ** argv){
    long pid_to_read;
    FILE *pid_file = fopen("mypid.txt", "r");
    fscanf (pid_file, "%ld", &pid_to_read);
    fclose(pid_file);
    kill((pid_t)pid_to_read, SIGINT);
    kill((pid_t)pid_to_read, SIGQUIT);
    kill((pid_t)pid_to_read, SIGSTOP);
    kill((pid_t)pid_to_read, SIGTSTP);
    kill((pid_t)pid_to_read, SIGABRT);
    kill((pid_t)pid_to_read, SIGTERM);
    kill((pid_t)pid_to_read, SIGKILL);
    return 0;
}
