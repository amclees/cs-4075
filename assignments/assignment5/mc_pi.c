#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

pthread_mutex_t mutex;
long thread_count;
long n;
int in_circle;

void* pi_calc(void* rank);

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

double random_double()
{
  double base = (double) rand() / RAND_MAX;
  return (base * 2) - 1;
}

int main(int argc, char* argv[]) {
  long i;
  srand(time(NULL));

  pthread_t* thread_handles;

  Get_args(argc, argv);
  thread_handles = malloc(sizeof(pthread_t) * thread_count);
  printf("Making %ld threads, %ld throws\n", thread_count, n);

  pthread_mutex_init(&mutex, NULL);

  for (i = 0; i < thread_count; i++) {
    long* index = malloc(sizeof(long));
    *index = i;
    pthread_create(&thread_handles[i], NULL, pi_calc, index);
  }

  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  printf("Estimate of pi: %lf\n", 4.0 * (((double) in_circle) / ((double) n)));

  pthread_mutex_destroy(&mutex);

  return 0;
}

void* pi_calc(void* rank) {
  int i;
  int local_in_circle = 0;
  long local_tosses = n / thread_count;
  double x, y, distance_squared;
  for (i = 0; i < local_tosses; i++) {
    x = random_double();
    y = random_double();
    distance_squared = (x * x) + (y * y);
    if (distance_squared <= 1) {
      local_in_circle++;
    }
  }

  pthread_mutex_lock(&mutex);
  in_circle += local_in_circle;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

void Get_args(int argc, char* argv[]) {
  if (argc != 3) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);
  n = strtol(argv[2], NULL, 10);
  if (thread_count <= 0) Usage(argv[0]);
}


void Usage(char* prog_name) {
  fprintf(stderr, "usage: %s <k> <n>\n", prog_name);
  fprintf(stderr, "   k should be the number of threads\n   n should be the number of throws\n");
  exit(0);
}
