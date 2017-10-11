#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

void Read_vector(int my_rank, int n, int local_n, double* vec, double* local_vec);
void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm); 

int main(void)
{
  int my_rank, comm_sz;

  int i;
  int n, local_n;
  double *vec1 = NULL, *vec2 = NULL, *local_vec1 = NULL, *local_vec2 = NULL, *local_vec_sum = NULL;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  if (my_rank == 0) {
    printf("Enter n:\n");
    scanf("%d", &n);
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  local_n = n / comm_sz;

  local_vec1 = malloc(local_n * sizeof(double));
  local_vec2 = malloc(local_n * sizeof(double));
  Read_vector(my_rank, n, local_n, vec1, local_vec1);
  Read_vector(my_rank, n, local_n, vec2, local_vec2);
  
  Print_vector(local_vec1, local_n, n, "1", my_rank, MPI_COMM_WORLD);
  Print_vector(local_vec2, local_n, n, "2", my_rank, MPI_COMM_WORLD);

  local_vec_sum = malloc(local_n * sizeof(double));
  for (i = 0; i < local_n; i++) {
    local_vec_sum[i] = local_vec1[i] + local_vec2[i];
  }

  Print_vector(local_vec_sum, local_n, n, "sum", my_rank, MPI_COMM_WORLD);

  free(vec1);
  free(vec2);
  free(local_vec1);
  free(local_vec2);
  free(local_vec_sum);

  MPI_Finalize();
  return 0;
}

void Read_vector(int my_rank, int n, int local_n, double* vec, double* local_vec)
{
  int i;

  if (my_rank == 0) {
    vec = malloc(n * sizeof(double));
    printf("Enter a vector\n");
    for (i = 0; i < n; i++) {
      scanf("%lf", &vec[i]);
    }
  }

  MPI_Scatter(vec, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm) {
  double* a = NULL;
  int i;

  if (my_rank == 0) {
    a = malloc(n * sizeof(double));
    MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
    printf("%s\n", title);
    printf("[");
    for (i = 0; i < n; i++) {
      printf("%f", a[i]);
      if (i != n - 1) {
        printf(", ");
      }
    }
    printf("]\n");
    free(a);
  } else {
    MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
  }
}

