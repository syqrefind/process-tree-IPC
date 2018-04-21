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


// Globals
int * arr_pt;
int sum_first_half;
int sum_second_half;
//int * sorted_arr_pt;
int min_value = INT_MIN;
int max_value = INT_MAX;
sig_atomic_t sigusr1_count = 0;
sig_atomic_t sigusr2_count = 0;

// Defines the signal handler
static void handler1 (int signum, siginfo_t *info, void *context);
// Alternative handler  for SIGUSR1
void handler(int signum);
// Alternative handler for SIGUSR2
void handler_usr2(int signum);
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
// Func gets the lower of the two

int main(int argc, const char * argv[]) {
    // Variable indicates both halves of sum are received

    // Allocate memory for global pointers
    arr_pt = malloc(1000*sizeof(int));
    //sorted_arr_pt = (int*)malloc(1000*sizeof(int));


    // Signal Action
    struct sigaction act, act1;
    memset (&act, 0, sizeof (act));
    //sigemptyset(&act.sa_mask);
    //act.sa_sigaction = &handler1;
    //act.sa_flags = SA_SIGINFO;
    act.sa_handler=&handler;
    act1.sa_handler=&handler_usr2;
    sigaction (SIGUSR1, &act, NULL);
    sigaction (SIGUSR2, &act1, NULL);

	int size=0;
	FILE *nums = fopen("in1000.txt", "r");
	while(fscanf(nums, "%d\n", &arr_pt[size++])!=EOF);
	fclose(nums);


    pid_t pid, pid1, pid2, pid3, pid4, pid5;
    pid=fork();
    if (pid==0){//sum
      pid1=fork();
      if(pid1==0){//sum 1
        sum_first_half=sum(arr_pt,pid1);//pid1==0, first half
        // write to a output txt FILE
        FILE *output= fopen("output.txt", "w+");
        fprintf(output, "%d\n", sum_first_half);
        fclose(output);


        if(kill(getppid(), SIGUSR1) == 0) {
            //printf("signal 1 sent successfully!!\n");
        }
        else {
            perror("Kill Error.\n");
        }
      }
      else if (pid1>0){//sum
          // busy wait until SIGUSR1 is receivd once
          // while(sigusr1_count!=1)
          //     ;

          // read the sum of first first_half
          int first_half;
          //int i = 0;
          FILE *output = fopen("output.txt", "r");
          fscanf (output, "%d", &first_half);
          fclose(output);
        pid5=fork();
        if (pid5==0){//sum 2
          sum_second_half=sum(arr_pt,1);//pid1!=0, second half
          //printf("sum2: the sum of second half is %d\n", sum_second_half);
          FILE *output= fopen("output.txt", "a+");
          fprintf(output, "%d\n", sum_second_half);
          fclose(output);

          if(kill(getppid(), SIGUSR2) == 0) {
          }
          else {
              perror("Kill Error.\n");
          }
        }
        else if (pid5>0){//sum
            // Sum receives SIGUSR1 and SIGUSR2

            // busy wait until receiving SIGUSR2
            //printf("This is parent sum after spawning sum2\n");
            // while(sigusr1_count!=2)
            //     ;

            while(1){
                if ((sigusr1_count == 1)&& (sigusr2_count == 1)){
                    break;
                }
            }
            // printf("sigusr1 is received %dth\n", sigusr1_count);
            // FILE *output= fopen("output.txt", "w+");
            // fprintf(output, "%d\n", sum_first_half);
            // fclose(output);

            int first_half;
            int second_half;
            //int i = 0;
            FILE *output = fopen("output.txt", "r");
            fscanf(output, "%d\n", &first_half);
            fscanf(output, "%d\n", &second_half);
            fclose(output);

            if(kill(getppid(), SIGUSR2) == 0) {

            }

              printf("The sum of the numbers is %d.\n", first_half+second_half);
        }
      }
      else{
        printf("Error\n");
      }
    }
    else if (pid>0){//parent

        while(sigusr2_count!=1)
            ;
        //printf("The SIGUSR2 is received, now forking sort\n");
        sigusr1_count = 0;
        sigusr2_count = 0;
        //sleep(5);
      pid2=fork();
      if (pid2==0){//sort
        // before forking min and max, sort will sort the arrary
        int * sorted_array = arr_pt;

        sort(sorted_array, 1000);
        //printArray(sorted_array,1000);
        pid3=fork();
        if (pid3==0){//min
            // write min into file and nofity `sort` using SIGUSR1
            FILE *output= fopen("output.txt", "w+");
            fprintf(output, "%d\n", min(sorted_array, 1000));
            fclose(output);
            if(kill(getppid(), SIGUSR2) == 0) {
                //printf("signal 1 sent successfully!!\n");
                //printf("%d\n",parent);
            }
            else {
                perror("Kill Error.\n");
            }
        }
        else if (pid3>0){//sort
            // read min from file when notified
            // while(sigusr1_count!=1)
            //     ;
            // FILE *output = fopen("output.txt", "r");
            // fscanf (output, "%d", &read_min);
            // fclose(output);
          pid4=fork();
          if (pid4==0){//max
              // write max into output file and notify `sort` using SIGUSR1
              FILE *output= fopen("output.txt", "a+");
              fprintf(output, "%d\n", max(sorted_array, 1000));
              fclose(output);
              if(kill(getppid(), SIGUSR1) == 0) {
                  //printf("signal 2 sent successfully!!\n");
              }
              else {
                  perror("Kill Error.\n");
              }

          }
          else if (pid4>0){//sort
            // read max from file when notified
            // print min and max
            // while(sigusr1_count!=2)
            //     ;
            while(1){
                if ((sigusr1_count == 1)&& (sigusr2_count == 1)){
                    break;
                }
            }
            int read_max;
            int read_min;
            int tmp;
            FILE *output = fopen("output.txt", "r");
            fscanf (output, "%d", &read_min);
            fscanf (output, "%d", &read_max);
            fclose(output);
            if (read_max < read_min){
                tmp = read_max;
                read_max = read_min;
                read_min = tmp;
            }
            printf("The min is %d and the max is %d.\n", read_min, read_max);
          }
        }
      }
      else if (pid2>0){//still parent
          //waitpid();

      }
    }
    //sleep(1);
  return 0;
}


static void handler1 (int signum, siginfo_t *info, void *context){
    printf("sig no = %d \n", signum);
    fprintf(stderr," sent value is  = %d \n" ,info->si_value.sival_int);
    if(signum == SIGINT){
        exit(0);
      }
    else if (signum == SIGUSR1){
        ++sigusr1_count;
        // Do nothing
    }
    else if (signum == SIGUSR2){
        ++sigusr2_count;
      //Do even more nothing
    }
    //fprintf(stderr," sent value is  = %d \n" ,info->si_value.sival_int);
    // printf ("Sending PID: %ld, UID: %ld\n",
    //         (long)siginfo->si_pid, (long)siginfo->si_uid);
}


 void handler(int signum){
    ++sigusr1_count;
 }
void handler_usr2(int signum){
    ++sigusr2_count;
}

void sort(int arr[], int size){
    qsort(arr, size, sizeof(int), compare);
    //qsort(arr, sizeof(&arr)/sizeof(arr), sizeof(arr), compare);
    return;
}


int sum(int arr[], int option){//option 0 = first half
    int sump=0;
    if (option==0){
        for (int i=0;i<500;i++){
            sump+=arr[i];
        }
    }
    else {
      for (int i=500;i<1000;i++){
            sump+=arr[i];
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
