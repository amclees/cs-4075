#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

long thread_count;
pthread_mutex_t mutex;
char** messages;

void* Thread_prod_con(void* rank);

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

int main(int argc, char* argv[]) {
  pthread_t* thread_handles;

  Get_args(argc, argv);
  thread_handles = malloc(sizeof(pthread_t) * thread_count);
  pthread_mutex_init(&mutex, NULL);
  messages = malloc(sizeof(char*) * thread_count);
  printf("Making %ld threads\n", thread_count);
  long i;
  for (i = 0; i < thread_count; i++) {
    messages[i] = NULL;
  }
  for (i = 0; i < thread_count; i++) {
    long* index = malloc(sizeof(long));
    *index = i;
    pthread_create(&thread_handles[i], NULL, Thread_prod_con, index);
  }
  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  return 0;
}

void* Thread_prod_con(void* rank) {
  long my_rank = *((long*) rank);
  long dest = (my_rank + 1) % thread_count;
  int received = 0;
  int sent = 0;
  while (!sent || !received) {
    pthread_mutex_lock(&mutex);
    if (!sent && messages[dest] == NULL) {
      char* message = malloc(sizeof(char) * 100);
      sprintf(message, "Thread %ld sent this message to thread %ld\n", my_rank, dest);
      messages[dest] = message;
      sent = 1;
    }
    if (!received && messages[my_rank] != NULL) {
      printf("%ld got message: %s", my_rank, messages[my_rank]);
      received = 1;
    }
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

void Get_args(int argc, char* argv[]) {
  if (argc != 2) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0) Usage(argv[0]);
}


void Usage(char* prog_name) {
  fprintf(stderr, "usage: %s <k>\n", prog_name);
  fprintf(stderr, "   k should be the number of threads\n");
  exit(0);
}
