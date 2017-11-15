#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

pthread_mutex_t mutex;
long thread_count;
int n;
double a;
double b;
double total_estimate;

void* trapezoid(void* rank);

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

double f(double input) {
  return input * input;
}

double Trap(
    double left_endpt,
    double right_endpt,
    int trap_count,
    double base_len) {
  double estimate, x;
  int i;

  estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
  for (i = 1; i < trap_count - 1; i++) {
    x = left_endpt + i * base_len;
    estimate += f(x);
  }
  estimate = estimate * base_len;

  return estimate;
}

int main(int argc, char* argv[]) {
  long i;
  srand(time(NULL));

  pthread_t* thread_handles;

  Get_args(argc, argv);
  thread_handles = malloc(sizeof(pthread_t) * thread_count);
  printf("Making %ld threads, %d trapezoids, %lf to %lf\n", thread_count, n, a, b);

  pthread_mutex_init(&mutex, NULL);

  for (i = 0; i < thread_count; i++) {
    long* index = malloc(sizeof(long));
    *index = i;
    pthread_create(&thread_handles[i], NULL, trapezoid, index);
  }

  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  printf("Estimate is: %lf\n", total_estimate);

  pthread_mutex_destroy(&mutex);

  return 0;
}

void* trapezoid(void* rank) {
  long my_rank = *((long*) rank);

  double h = (b - a) / n;
  double local_n = n / thread_count;

  double local_a = a + my_rank * local_n * h;
  double local_b = local_a + local_n * h;
  double local_int = Trap(local_a, local_b, local_n, h);

  pthread_mutex_lock(&mutex);
  total_estimate += local_int;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

void Get_args(int argc, char* argv[]) {
  if (argc != 5) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);
  n = strtol(argv[2], NULL, 10);
  a = atof(argv[3]);
  b = atof(argv[4]);
  if (thread_count <= 0) Usage(argv[0]);
}


void Usage(char* prog_name) {
  fprintf(stderr, "usage: %s <k> <number of trapezoids> <left endpoint> <right endpoint>\n", prog_name);
  fprintf(stderr, "   k should be the number of threads\n");
  exit(0);
}
