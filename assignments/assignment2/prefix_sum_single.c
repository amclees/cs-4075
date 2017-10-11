#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(void)
{
  int my_rank, comm_sz;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  srand(time(NULL) + my_rank);

  int my_value = rand() % 10, my_prefix_sum;

  MPI_Scan(&my_value, &my_prefix_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  printf("Prefix sum is %d and value is %d on process %d\n", my_prefix_sum, my_value, my_rank);

  MPI_Finalize();
  return 0;
}

