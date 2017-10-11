#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(void)
{
  int i;
  int my_rank, comm_sz;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  srand(time(NULL) + my_rank);

  int n;
  int *values;
  if (my_rank == 0) {
    printf("Enter n:\n");
    scanf("%d", &n);
    values = malloc(n * sizeof(int));
    printf("Vector:\n");
    for (i = 0; i < n; i++) {
      values[i] = rand() % 10;
      printf("%d ", values[i]);
    }
    printf("\n");
  }
  
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int local_n, *local_values;
  local_n = n / comm_sz;
  local_values = malloc(local_n * sizeof(int));

  MPI_Scatter(values, local_n, MPI_INT, local_values, local_n, MPI_INT, 0, MPI_COMM_WORLD);

  int local_total = 0, current_total = 0;
  int *local_prefix_sums = malloc(local_n * sizeof(int));
  for (i = 0; i < local_n; i++) {
    local_total += local_values[i];
    local_prefix_sums[i] = local_total;
  }

  MPI_Scan(&local_total, &current_total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  int previous_total = current_total - local_total;
  for (i = 0; i < local_n; i++) {
    local_prefix_sums[i] += previous_total;
  }

  int *prefix_sums;
  if (my_rank == 0) {
    prefix_sums = malloc(n * sizeof(int));
  }

  MPI_Gather(local_prefix_sums, local_n, MPI_INT, prefix_sums, local_n, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    printf("Prefix sums:\n");
    for (i = 0; i < n; i++) {
      printf("%d ", prefix_sums[i]);
    }
    printf("\n");
  }

  MPI_Finalize();
  return 0;
}

