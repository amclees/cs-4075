#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

const int MAX_THREADS = 1024;

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER; 

struct number {
  int value;
  struct number* next;
};

struct number_list {
  int length;
  struct number* first;
};

int member(struct number_list* list, int number) {
  struct number* current = list->first;
  while (1) {
    if (current == NULL) {
      return 0;
    }
    if (current->value == number) {
      return 1;
    }
    current = current->next;
  }
}

void insert(struct number_list* list, int number) {
  struct number* new = malloc(sizeof(number));
  new->value = number;
  new->next = list->first;
  list->first = new;
  list->length++;
}

int delete(struct number_list* list, int index) {
  if (index == 0) {
    // TODO: Special case
    return -1;
  }

  struct number* current = list->first;
  int current_index = 0;
  while (1) {
    if (current == NULL) {
      return -1;
    }
    if (current_index + 1 == index) {
      struct number* deleting = current->next;
      int return_value = deleting->value;
      current->next = deleting->next;
      free(deleting);
      list->length--;
      return return_value;
    }
    current = current->next;
  }
}

int thread_count;
int task_count;
struct number_list list;

void* run_tasks(void* to_run);

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

int main(int argc, char* argv[]) {
  long i;
  srand(time(NULL));

  list.first = NULL;
  list.length = 0;

  Get_args(argc, argv);

  pthread_rwlock_init(&rwlock, NULL);

  pthread_t* thread_handles;

  thread_handles = malloc(sizeof(pthread_t) * thread_count);
  printf("Making %d threads, %d tasks\n", thread_count, task_count);

  int local_task_count = task_count / thread_count;

  for (i = 0; i < thread_count; i++) {
    pthread_create(&thread_handles[i], NULL, run_tasks, &local_task_count);
  }

  for (i = 0; i < thread_count; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  printf("Finished\n");

  pthread_rwlock_destroy(&rwlock);

  return 0;
}

void* run_tasks(void* to_run) {
  int i;
  int running = *((int*) to_run);

  for (i = 0; i < running; i++) {
    int choice = rand() % 3;
    switch (choice) {
      case 0:
        pthread_rwlock_rdlock(&rwlock);
        int to_check = rand() % 10;
        int is_member = member(&list, to_check);
        if (is_member) {
          printf("%d is a member\n", to_check);
        } else {
          printf("%d is not a member\n", to_check);
        }
        break;
      case 1:
        pthread_rwlock_wrlock(&rwlock);
        if (list.length == 0) {
          printf("Tried to delete but list was empty\n");
          break;
        }
        int deleting = rand() % list.length;
        int deleted = delete(&list, deleting);
        printf("Deleted %d from index %d\n", deleted, deleting);
        break;
      case 2:
        pthread_rwlock_wrlock(&rwlock);
        int inserting = rand() % 10;
        printf("Inserting %d\n", inserting);
        insert(&list, inserting);;
        break;
    }
    pthread_rwlock_unlock(&rwlock);
  }

  return NULL;
}

void Get_args(int argc, char* argv[]) {
  if (argc != 3) Usage(argv[0]);
  thread_count = strtol(argv[1], NULL, 10);
  task_count = strtol(argv[2], NULL, 10);
  if (thread_count <= 0) Usage(argv[0]);
}


void Usage(char* prog_name) {
  fprintf(stderr, "usage: %s <a> <b>\n", prog_name);
  fprintf(stderr, "   a should be the number of threads\n");
  fprintf(stderr, "   b should be the number of tasks\n");
  exit(0);
}

