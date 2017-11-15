#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

pthread_mutex_t mutex;
long thread_count;
long bin_count;
long data_count;
long* bins;
double* data;
double bin_width;

void* histogram(void* rank);

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
  printf("Making %ld threads, %ld bins, %ld data points\n", thread_count, bin_count, data_count);

  bin_width = 10 / bin_count;

  bins = malloc(bin_count * sizeof(long));
  for (i = 0; i < bin_count; i++) {
    bins[i] = 0;
  }
  data = malloc(data_count * sizeof(double));
  printf("data:\n");
  for (i = 0; i < data_count; i++) {
    data[i] = (double) (rand() % 10);
    printf("%lf ", data[i]);
  }
  printf("\n");

  pthread_mutex_init(&mutex, NULL);

  for (i = 0; i < thread_count; i++) {
    long* index = malloc(sizeof(long));
    *index = i;
    pthread_create(&thread_handles[i], NULL, histogram, index);
  }

  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  printf("bins:\n");
  double bin_location = 0.0;
  for (i = 0; i < bin_count; i++) {
    printf("%ld in [%lf, %lf)\n", bins[i], bin_location, bin_location + bin_width);
    bin_location += bin_width;
  }

  pthread_mutex_destroy(&mutex);

  return 0;
}

void place_data(long* local_bins, double data) {
  long position = floor(((double) data) / bin_width);
  local_bins[position]++;
}

void* histogram(void* rank) {
  long my_rank = *((long*) rank);
  long local_data_count = data_count / thread_count;
  long i;
  long* local_bins = malloc(bin_count * sizeof(long));
  for (i = 0; i < bin_count; i++) {
    local_bins[i] = 0;
  }

  for (i = my_rank * local_data_count; i < ((my_rank + 1) * local_data_count); i++) {
    place_data(local_bins, data[i]);
  }

  pthread_mutex_lock(&mutex);
  for (i = 0; i < bin_count; i++) {
    bins[i] += local_bins[i];
  }
  pthread_mutex_unlock(&mutex);

  free(local_bins);
  return NULL;
}

void Get_args(int argc, char* argv[]) {
  if (argc != 4) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);
  data_count = strtol(argv[2], NULL, 10);
  bin_count = strtol(argv[3], NULL, 10);
  if (thread_count <= 0) Usage(argv[0]);
}


void Usage(char* prog_name) {
  fprintf(stderr, "usage: %s <k> <data count> <bin count>\n", prog_name);
  fprintf(stderr, "   k should be the number of threads\n");
  exit(0);
}
