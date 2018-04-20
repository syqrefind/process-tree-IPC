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

// Globals
int * arr_pt;
int sum_first_half;
int sum_second_half;
//int * sorted_arr_pt;
int min_value = INT_MIN;
int max_value = INT_MAX;


// Defines the signal handler
static void handler1 (int signum, siginfo_t *siginfo, void *context);
// Find the sum of the firt half or the second half of an array
int sum(int arr[], int option);
// Quicksort an array
void sort(int arr[], int size);
// Comparison Func required by Quicksort
int compare (const void *, const void *);
// Func prints an array
void printArray(int a[], int size);
// Func finds the min of the sorted arrary
int min(int arr[], int size);
// Func finds the max of the sorted arrary
int max(int arr[], int size);


int main(int argc, const char * argv[]) {
    // Variable indicates both halves of sum are received

    // Allocate memory for global pointers
    arr_pt = malloc(1000*sizeof(int));
    //sorted_arr_pt = (int*)malloc(1000*sizeof(int));


    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = &handler1;
    act.sa_flags = SA_SIGINFO;


    //int numbers[1000];
	int size=0;
	FILE *nums = fopen("in1000.txt", "r");
	while(fscanf(nums, "%d\n", &arr_pt[size++])!=EOF);
	fclose(nums);
	FILE *output= fopen("out1000c.txt", "w+");
    //printArray(arr_pt, 1000);


    pid_t pid, pid1, pid2, pid3, pid4, pid5;
    pid=fork();
    if (pid==0){//sum
        // Variable indicates sum 1 is received
        bool received_sum_1 = false;
        bool received_sum_2 = false;
      int parent=getppid();//parent pid
      pid1=fork();
      if(pid1==0){//sum 1
        sum_first_half=sum(arr_pt,pid1);//pid1==0, first half
        if(kill(getppid(), SIGUSR1) == 0) {
            printf("signal 1 sent successfully!!\n");
            //printf("%d\n",parent);
        }
        else {
            perror("Kill Error.\n");
        }
      }
      else if (pid1>0){//sum

          printf("The pid of sum %d\n", getpid());
        pid5=fork();
        if (pid5==0){//sum 2
          sum_second_half=sum(arr_pt,pid1);//pid1!=0, second half
          printf("The pid of the parent sum %d\n", getppid());
          if(kill(getppid(), SIGUSR2) == 0) {
              printf("signal 2 sent successfully!!\n");
              //printf("%d\n",parent);
          }
          else {
              perror("Kill Error.\n");
          }
        }
        else if (pid5>0){//sum
            //waitpid();
            // Sum receives SIGUSR1 and SIGUSR2
            int a = waitpid(pid1,NULL, 0);//first sum
            printf("a is %d\n", a);
            int b = waitpid(pid5,NULL, 0);//second sum
            printf("b is %d\n", b);
            if (sigaction(SIGUSR1, &act, NULL) == -1) {
                perror("sigusr: sigaction");
            }
            else if(sigaction(SIGUSR1, &act, NULL)==0){//if no error
                received_sum_1 = true;
                printf("signal 1 received\n");
            }
            if (sigaction(SIGUSR2, &act, NULL) == -1){
                perror("sigusr: sigaction");
            }
            else if(sigaction(SIGUSR2, &act, NULL) == 0){ // no error
                received_sum_2 = true;
                printf("signal 2 received\n");
            }
            while(!(received_sum_1)){
              ;
            }
            while(!(received_sum_2)){
              ;
            }
              printf("The sum of the numbers is %d.\n", sum_first_half+sum_second_half);

            // waitpid(pid1,NULL, 0);//first sum
            // waitpid(pid5,NULL, 0);//second sum
        }
      }
      else{
        printf("Error\n");
      }
    }
    else if (pid>0){//parent

      pid2=fork();
      if (pid2==0){//sort
        int parent = getppid();
        pid3=fork();
        if (pid3==0){//min
          //printArray(arr_pt, 1000);
          //printf("Min\n");
          //printArray(arr_pt, 1000);
          int minp=min(arr_pt, 1000);
        }
        else if (pid3>0){//sort
          pid4=fork();
          if (pid4==0){//max
            int maxp=max(arr_pt, 1000);
          }
          else if (pid4>0){//sort
            sort(arr_pt, 1000);
            //printArray(arr_pt, 1000);
          }
        }
      }
      else if (pid2>0){//still parent
          //waitpid();

      }
    }
    sleep(1);
  return 0;
}


static void handler1 (int signum, siginfo_t *siginfo, void *context){
    printf("sig no = %d \n", signum);
    if(signum == SIGINT){
        exit(0);
      }
    else if (signum == SIGUSR1){
        // Do nothing
    }
    else if (signum == SIGUSR2){
      //Do even more nothing
    }
    // printf ("Sending PID: %ld, UID: %ld\n",
    //         (long)siginfo->si_pid, (long)siginfo->si_uid);
}


void sort(int arr[], int size){
    qsort(arr, size, sizeof(int), compare);
    //qsort(arr, sizeof(&arr)/sizeof(arr), sizeof(arr), compare);
    return;
}


int sum(int arr[], int option){//option 0 = first half
    int sump=0;
    if(option==0){
        for(int i=0;i<500;i++){
            sump+=arr[i];
        }
    }
    else {
      for(int i=500;i<1000;i++){
            sump=+arr[i];
      }

    }
    return sump;
}


int compare(const void * elem1, const void * elem2)
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}


void printArray(int a[], int size){
    for (int i = 0; i < size; i++){
        printf("%d ", a[i]);
    }
    printf("\n");
    return;
}


int min(int arr[], int size){
    return arr[0];
}


int max(int arr[], int size){
    return arr[size - 1];
}
