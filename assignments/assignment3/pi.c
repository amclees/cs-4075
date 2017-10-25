#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

double random_double()
{
  double base = (double) rand() / RAND_MAX;
  return (base * 2) - 1;
}

int main()
{
  int my_rank, comm_sz;
  int i, tosses, local_tosses, in_circle, local_in_circle = 0;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  srand(time(NULL) + my_rank);

  if (my_rank == 0) {
    printf("Enter number of tosses:\n");
    scanf("%d", &tosses);
  }
  
  MPI_Bcast(&tosses, 1, MPI_INT, 0, MPI_COMM_WORLD);
  local_tosses = tosses / comm_sz;

  double x, y, distance_squared;
  for (i = 0; i < local_tosses; i++) {
    x = random_double();
    y = random_double();
    distance_squared = (x * x) + (y * y);
    if (distance_squared <= 1) {
      local_in_circle++;
    }
  }

  MPI_Reduce(&local_in_circle, &in_circle, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    printf("Estimate of pi: %lf\n", 4.0 * (((double) in_circle) / ((double) tosses)));
  }

  MPI_Finalize();
  return 0;
}
