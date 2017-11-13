#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

long thread_count;
long n;
double** A;
double* x;
double* y;

void* Pth_mat_vect(void* rank);

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

double rand_double() {
  return (double) (rand() % 5);
}

int main(int argc, char* argv[]) {
  long i, j;
  srand(time(NULL));

  pthread_t* thread_handles;

  Get_args(argc, argv);
  thread_handles = malloc(sizeof(pthread_t) * thread_count);
  printf("Making %ld threads, %ld by %ld matrix\n", thread_count, n, n);

  A = malloc(n * sizeof(double*));
  x = malloc(n * sizeof(double));
  y = malloc(n * sizeof(double));
  for (i = 0; i < n; i++) {
    x[i] = rand_double();
    y[i] = 0.0;

    A[i] = malloc(n * sizeof(double));
    for (j = 0; j < n; j++) {
      A[i][j] = rand_double();
    }
  }

  for (i = 0; i < thread_count; i++) {
    long* index = malloc(sizeof(long));
    *index = i;
    pthread_create(&thread_handles[i], NULL, Pth_mat_vect, index);
  }

  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  printf("a:\n");
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      printf("%lf ", A[i][j]);
    }
    printf("\n");
  }

  printf("x:\n");
  for (i = 0; i < n; i++) {
    printf("%lf\n", x[i]);
  }
  printf("\n");

  printf("y:\n");
  for (i = 0; i < n; i++) {
    printf("%lf\n", y[i]);
  }
  printf("\n");

  return 0;
}

void* Pth_mat_vect(void* rank) {
  long my_rank = *((long*) rank);

  int i, j, k;
  int local_n = n / thread_count;
  int my_first_row = my_rank * local_n;
  int my_last_row = ((my_rank + 1) * local_n) - 1;
  double* local_y = malloc(local_n * sizeof(double));

  k = 0;
  for (i = my_first_row; i <= my_last_row; i++) {
    local_y[k] = 0.0;
    for (j = 0; j < n; j++) {
      local_y[k] += A[i][j] * x[j];
    }
    k += 1;
  }

  k = 0;
  for (i = my_first_row; i <= my_last_row; i++) {
    y[i] = local_y[k];
    k++;
  }

  free(local_y);
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
  fprintf(stderr, "   k should be the number of threads\n   n should be the width of the matrix\n");
  exit(0);
}
