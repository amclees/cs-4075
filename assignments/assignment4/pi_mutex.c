#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

const int MAX_THREADS = 1024;

long thread_count;
long long n;
double sum = 0;
pthread_mutex_t mutex;

void* Thread_sum(void* rank);

/* Only executed by main thread */
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
double Serial_pi(long long n);

int main(int argc, char* argv[]) {
  pthread_t* thread_handles;
  double start, finish, elapsed;

  /* Get number of threads from command line */
  Get_args(argc, argv);

  double serial_sum;
  GET_TIME(start);
  serial_sum = Serial_pi(n);
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Serial pi sum %lf took %lf seconds\n", serial_sum, elapsed);

  GET_TIME(start);
  pthread_mutex_init(&mutex, NULL);
  thread_handles = malloc(sizeof(pthread_t) * thread_count);
  int i;
  for (i = 0; i < thread_count; i++) {
    int* index = malloc(sizeof(int));
    *index = i;
    pthread_create(thread_handles, NULL, Thread_sum, index);
  }
  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }
  sum *= 4.0;
  pthread_mutex_destroy(&mutex);
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Threaded pi sum %lf took %lf seconds\n", sum, elapsed);

  return 0;
}  /* main */

/*------------------------------------------------------------------*/
void* Thread_sum(void* rank) {
  int i;
  int my_rank = *((int*) rank);
  //printf("My rank is %d\n", my_rank);
  int count = n / thread_count;
  double factor = (my_rank * count) % 2 == 0 ? 1 : -1;
  //printf("Rank %d gets %d/%d = %d things. It starts from index %d and uses a factor %lf\n", my_rank, n, thread_count, count, my_rank * count, factor);

  for (i = my_rank * count; i < (my_rank * count) + count; i++, factor *= -1) {
    pthread_mutex_lock(&mutex);
    sum += factor / ((double) ((2.0 * i) + 1.0));
    //printf("Rank %d is adding %lf to sum to get %lf\n", my_rank, factor / ((2 * i) + 1), sum);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}  /* Thread_sum */

/*------------------------------------------------------------------
 * Function:   Serial_pi
 * Purpose:    Estimate pi using 1 thread
 * In arg:     n
 * Return val: Estimate of pi using n terms of Maclaurin series
 */
double Serial_pi(long long n) {
  double sum = 0.0;
  long long i;
  double factor = 1.0;

  for (i = 0; i < n; i++, factor = -factor) {
    sum += factor/(2*i+1);
  }
  return 4.0*sum;

}  /* Serial_pi */

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
  if (argc != 3) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
  n = strtoll(argv[2], NULL, 10);
  if (n <= 0) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
  fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
  fprintf(stderr, "   n is the number of terms and should be >= 1\n");
  fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
  exit(0);
}  /* Usage */
