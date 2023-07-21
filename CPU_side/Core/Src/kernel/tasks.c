
#include <kernel/tasks.h>

/* Tasks */
void task1(void) {
  while(1) {
    printf("1111\n");
    delayTask(16);
  }
}

void task2(void) {
  while(1) {
    printf("2222\n");
    delayTask(8);
  }
}

void task3(void) {
  while(1) {
    printf("3333\n");
    delayTask(4);
  }
}

void task4(void) {
  while(1) {
    printf("4444\n");
    delayTask(2);
  }
}
