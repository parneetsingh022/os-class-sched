#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"

// `task_node` is a linked list used to store tasks where are scheduled
// to run by the scheduler.
struct task_node {
  Task* task;
  struct task_node* next;
};

static void free_task_node(struct task_node* node) {
  if (node == NULL)
    return;
  
  free(node->task->name);
  free(node->task);
  free(node);
}

struct task_node* head = NULL;
struct task_node* tail = NULL;

void add(char *name, int priority, int burst)
{
    struct task_node *node = malloc(sizeof *node);
    if (node == NULL)
        goto memory_alloc_node_error;

    node->task = malloc(sizeof *node->task);
    if (node->task == NULL)
        goto memory_alloc_task_error;

    node->task->name = strdup(name);
    if (node->task->name == NULL)
        goto memory_alloc_name_error;

    node->task->priority = priority;
    node->task->burst = burst;
    node->next = NULL;

    // If this is the first task added to the list, both head and tail will be NULL.
    //
    // We only need to check if head is NULL because tail is guaranteed to be NULL
    // when the list is empty.
    if (head == NULL) {
      /// This is our firt task set tid to 0.
      node->task->tid = 0;
      head = node;
      tail = node;  
      return;
    }

    node->task->tid = tail->task->tid + 1;
    tail->next = node;
    tail = node;

    return;

memory_alloc_name_error:
    free(node->task);

memory_alloc_task_error:
    free(node);

memory_alloc_node_error:
    fprintf(stderr, "failed to allocate memory!\n");
    exit(1);
}

void schedule()
{

  while (head != NULL) {
    run(head->task, head->task->burst);        
    struct task_node* temp = head;
    head = head->next;

    free_task_node(temp);
  }

  tail = NULL;
  
  return;
}
