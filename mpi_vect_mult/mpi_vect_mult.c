#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
    MPI_Comm comm);
void Check_for_error(int local_ok, char fname[], char message[], 
    MPI_Comm comm);
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
    int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[], 
    int my_rank, MPI_Comm comm);
double Par_dot_product(double local_vec1[], double local_vec2[], 
    int local_n, MPI_Comm comm);
void Par_vector_scalar_mult(double local_vec[], double scalar, 
    double local_result[], int local_n);

int main(void) {
  int n, local_n;
  double *local_vec1, *local_vec2;
  double scalar;
  double *local_scalar_mult1, *local_scalar_mult2;
  double dot_product;
  int comm_sz, my_rank;
  MPI_Comm comm;
  comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  /* Print input data */
  Read_n(&n, &local_n, my_rank, comm_sz, comm);
  Read_data(local_vec1, local_vec2, &scalar, local_n, my_rank, comm_sz, comm);
  Print_vector(local_vec1, local_n, n, "Local vector 1", my_rank, comm);
  Print_vector(local_vec2, local_n, n, "Local vector 2", my_rank, comm);

  /* Print results */

  /* Compute and print dot product */

  /* Compute scalar multiplication and print out result */


  free(local_scalar_mult2);
  free(local_scalar_mult1);
  free(local_vec2);
  free(local_vec1);

  MPI_Finalize();
  return 0;
}

/*-------------------------------------------------------------------*/
void Check_for_error(
    int       local_ok   /* in */, 
    char      fname[]    /* in */,
    char      message[]  /* in */, 
    MPI_Comm  comm       /* in */) {
  int ok;

  MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
  if (ok == 0) {
    int my_rank;
    MPI_Comm_rank(comm, &my_rank);
    if (my_rank == 0) {
      fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, 
          message);
      fflush(stderr);
    }
    MPI_Finalize();
    exit(-1);
  }
}  /* Check_for_error */


/*-------------------------------------------------------------------*/
void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
    MPI_Comm comm) {
  if (my_rank == 0) {
    scanf("%d", n_p);
    *local_n_p = *n_p / comm_sz;
  }
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
  MPI_Bcast(local_n_p, 1, MPI_INT, 0, comm);
}  /* Read_n */


/*-------------------------------------------------------------------*/
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
    int local_n, int my_rank, int comm_sz, MPI_Comm comm) {
  double* a = NULL;
  int i;
  if (my_rank == 0){
    printf("What is the scalar?\n");
    scanf("%lf", scalar_p);
  }

  MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm);

  if (my_rank == 0){
    a = malloc(local_n * comm_sz * sizeof(double));
    printf("Enter the first vector\n");
    for (i = 0; i < local_n * comm_sz; i++) 
      scanf("%lf", &a[i]);
    MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
    printf("Enter the second vector\n");
    for (i = 0; i < local_n * comm_sz; i++) 
      scanf("%lf", &a[i]);
    MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
    free(a);
  } else {
    MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
    MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
  }
}  /* Read_data */

/*-------------------------------------------------------------------*/
void Print_vector(double local_vec[], int local_n, int n, char title[], 
    int my_rank, MPI_Comm comm) {
  double* a = NULL;
  int i;

  if (my_rank == 0) {
    a = malloc(n * sizeof(double));
    free(a);
  } else {
    // TODO: Gather here
    MPI_Gather(local_vec, local_n, MPI_DOUBLE, );
  }

}  /* Print_vector */


/*-------------------------------------------------------------------*/
double Par_dot_product(double local_vec1[], double local_vec2[], 
    int local_n, MPI_Comm comm) {
}  /* Par_dot_product */


/*-------------------------------------------------------------------*/
void Par_vector_scalar_mult(double local_vec[], double scalar, 
    double local_result[], int local_n) {
}  /* Par_vector_scalar_mult */
