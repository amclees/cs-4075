#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

int main(void)
{
  int my_rank, comm_sz;

  int i, j;
  int data_count, bin_count, local_data_count;
  int *bin_counts, *local_bin_counts;
  double *data, *bin_maxes, *local_data;
  double min_meas, max_meas;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  if (my_rank == 0) {
    printf("Enter data_count, min_meas, max_meas, bin_count seperated by spaces:\n");
    scanf("%d %lf %lf %d", &data_count, &min_meas, &max_meas, &bin_count);
    bin_maxes = malloc(bin_count * sizeof(double));
    double bin_size =  (max_meas - min_meas) / bin_count;
    for (i = 0; i < bin_count; i++) {
      bin_maxes[i] = min_meas + (bin_size * (i + 1));
    }

    data = malloc(data_count * sizeof(double));
    printf("Enter the data:\n");
    for (i = 0; i < data_count; i++) {
      scanf("%lf", data + (i * sizeof(double)));
    }
  }

  MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&min_meas, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&max_meas, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&bin_count, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  if (my_rank != 0) {
    bin_maxes = malloc(bin_count * sizeof(double));
  }
  MPI_Bcast(bin_maxes, bin_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  local_data_count = comm_sz / data_count;
  local_data = malloc(local_data_count * sizeof(double));
  MPI_Scatter(data, local_data_count, MPI_DOUBLE, local_data, local_data_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  local_bin_counts = malloc(bin_count * sizeof(int));
  for (i = 0; i < bin_count; i++) {
    local_bin_counts[i] = 0;
  }

  for (i = 0; i < local_data_count; i++) {
    double value = local_data[i];
    // TODO: Fix
    for (j = 0; j < bin_count; j++) {
      if (value < bin_maxes[j]) {
        local_bin_counts[i]++;
        break;
      }
    }
  }

  if (my_rank == 0) {
    bin_counts = malloc(bin_count * sizeof(int));
  }
  MPI_Reduce(local_bin_counts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if (my_rank == 0) {
    printf("bin_maxes:\n");
    for (i = 0; i < bin_count; i++) {
      printf("%lf ", bin_maxes[i]);
    }

    printf("\nbin_counts:\n");
    for (i = 0; i < bin_count; i++) {
      printf("%d ", bin_counts[i]);
    }
    printf("\n");
  }

  // TODO: Free arrays

  MPI_Finalize();
  return 0;
}

