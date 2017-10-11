#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
  srand(time(NULL));

  int n, i;
  printf("Enter n:\n");
  scanf("%d", &n);

  int* vector = malloc(n * sizeof(int));

  printf("Vector:\n");
  for (i = 0; i < n; i++) {
    vector[i] = rand() % 10;
    printf("%d ", vector[i]); 
  }
  printf("\n");

  printf("Prefix sum:\n%d", vector[0]);
  int sum = vector[0];
  for (i = 1; i < n; i++) {
    sum += vector[i];
    printf(", %d", sum);
  }
  printf("\n");

  return 0;
}
