
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
int tid_count = 0;

static void insert_sorted_task(struct task_node* node)
{
  // if this is the first item we are adding.
  if (head == NULL) {
    head = node;
    tail = node;

    return;
  }

  // if new task takes less time then head
  if(head->task->burst > node->task->burst) {
    node->next = head;
    head = node;
    return; 
  }

  struct task_node* cur = head;

  while (cur->next && cur->next->task->burst <= node->task->burst){
    cur = cur->next;
  }

  // if this is the last task update the tail.
  if (cur == tail) {
    tail = node;
  }

  node->next = cur->next;
  cur->next = node;
}

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
    node->task->tid = tid_count;
    tid_count++;
    
    node->next = NULL;
    insert_sorted_task(node);
    

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
