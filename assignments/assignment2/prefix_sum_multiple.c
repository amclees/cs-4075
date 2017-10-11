#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(void)
{
  int i;
  int my_rank, comm_sz;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  srand(time(NULL) + my_rank);

  int count;
  if (my_rank == 0) {
    printf("Enter count:\n");
    scanf("%d", &count);
  }
  MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  int *values = malloc(count * sizeof(int)), *partial_values = malloc(count * sizeof(int));

  printf("Process %d has values:\n", my_rank);
  for (i = 0; i < count; i++) {
    values[i] = rand() % 10;
    printf("%d ", values[i]);
  }
  printf("\n");

  MPI_Scan(values, partial_values, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  printf("Process %d partial sum:\n", my_rank);
  for (i = 0; i < count; i++) {
    printf("%d ", partial_values[i]);
  }
  printf("\n");

  MPI_Finalize();
  return 0;
}

