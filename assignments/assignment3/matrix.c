#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void fill_array(double *array, int length) {
  int i;
  for (i = 0; i < length; i++) {
    array[i] = rand() % 5;
  }
}

void print_matrix(char *name, double *array, int length, int row_length) {
  printf("%s\n[\n", name);
  int i;
  for (i = 0; i < length; i++) {
    printf("%lf ", array[i]);
    if (i % row_length == row_length - 1) {
      printf("\n");
    }
  }
  printf("]\n");
}

void print_vector(char *name, double *array, int length) {
  printf("%s\n[\n", name);
  int i;
  for (i = 0; i < length; i++) {
    printf("%lf\n", array[i]);
  }
  printf("]\n");
}

// Can split a matrix in any way
// 4x4 matrix can be flattened into a 16 size vector
// Can just scatter it this way
int main()
{
  int i, j, n, local_matrix_n;
  int my_rank, comm_sz;
  double local_total = 0;
  double *matrix, *local_matrix, *vector, *result, *global_result;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  srand(time(NULL) + my_rank);

  if (my_rank == 0) {
    printf("Please enter n:\n");
    scanf("%d", &n);

    matrix = malloc(n * n * sizeof(double));
    fill_array(matrix, n * n);
    print_matrix("matrix ", matrix, n * n, n);

    vector = malloc(n * sizeof(double));
    fill_array(vector, n);
    print_vector("vector ", vector, n);

    global_result = malloc(n * sizeof(double));
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  local_matrix_n = (n * n) / comm_sz; 

  result = malloc(n * sizeof(double));
  for (i = 0; i < n; i++) {
    result[i] = 0;
  }
  local_matrix = malloc(local_matrix_n * sizeof(double));
  if (my_rank != 0) {
    vector = malloc(n * sizeof(double));
  }

  MPI_Bcast(vector, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Scatter(matrix, local_matrix_n, MPI_DOUBLE, local_matrix, local_matrix_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  int local_rows = local_matrix_n / n;
  for (i = 0; i < local_rows; i++) {
    int base_row_index = i * n;
    local_total = 0;
    for (j = 0; j < n; j++) {
      local_total += local_matrix[base_row_index + j] * vector[j];
    }
    result[(my_rank * local_rows) + i] = local_total;
  }

  MPI_Reduce(result, global_result, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    printf("Result:\n");
    for (i = 0; i < n; i++) {
      printf("%lf ", global_result[i]);
    }
    printf("\n");
  }

  MPI_Finalize();
  return 0;
}
