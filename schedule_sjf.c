
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"

// Linked list for the ready queue; tasks are kept in shortest-burst order.
struct task_node {
  Task* task;
  struct task_node* next;
};

static void free_task_node(struct task_node* node) {
  // Free one finished task's memory.
  if (node == NULL)
    return;
  
  free(node->task->name);
  free(node->task);
  free(node);
}

struct task_node* head = NULL;
struct task_node* tail = NULL;
int tid_count = 0;

static void insert_sorted_task(struct task_node* node)
{
  // Empty queue: this task becomes the first one.
  if (head == NULL) {
    head = node;
    tail = node;

    return;
  }

  // Put the new task at the front if it has the shortest burst.
  if(head->task->burst > node->task->burst) {
    node->next = head;
    head = node;
    return; 
  }

  struct task_node* cur = head;

  // Walk until we find the correct insertion point by burst length.
  while (cur->next && cur->next->task->burst <= node->task->burst){
    cur = cur->next;
  }

  // If it belongs at the end, update the tail pointer.
  if (cur == tail) {
    tail = node;
  }

  node->next = cur->next;
  cur->next = node;
}

void add(char *name, int priority, int burst)
{
    // Create and initialize the new task node.
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
    node->task->tid = tid_count;
    tid_count++;
    
    node->next = NULL;
    insert_sorted_task(node);
    

    return;

// Cleanup labels for failed memory allocations.
// If name allocation fails, free the task object first.
memory_alloc_name_error:
    free(node->task);

// If the task allocation fails, free the list node only.
memory_alloc_task_error:
    free(node);

// If the node allocation fails, print an error and exit.
memory_alloc_node_error:
    fprintf(stderr, "failed to allocate memory!\n");
    exit(1);
}

void schedule()
{
  // SJF runs the shortest burst first, then removes it and continues.
  while (head != NULL) {
    run(head->task, head->task->burst);        
    struct task_node* temp = head;
    head = head->next;

    free_task_node(temp);
  }

  tail = NULL;
  
  return;
}
