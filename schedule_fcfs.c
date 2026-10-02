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
  // Free the memory used by one queued task entry.
  // This includes the task metadata, the task name string, and the list node itself.
  if (node == NULL)
    return;
  
  free(node->task->name);
  free(node->task);
  free(node);
}

// Queue pointers for the FIFO ready list.
// head points to the first task to run, and tail points to the most recently added task.
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

    // Store task details and initialize the list link.
    node->task->priority = priority;
    node->task->burst = burst;
    node->next = NULL;

    // If this is the first task added to the list, both head and tail will be NULL.
    // We only need to check head because tail is guaranteed to be NULL when the queue is empty.
    if (head == NULL) {
      node->task->tid = 0;
      head = node;
      tail = node;  
      return;
    }

    // Assign the next sequential task ID and append to the end of the ready queue.
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
  // FCFS (first-come, first-served) scheduling: repeatedly run the task at the front
  // of the queue until it has been completely processed, then remove it and continue.
  while (head != NULL) {
    // Run the task currently at the front of the queue for its full burst time.
    run(head->task, head->task->burst);        

    // Save the current node so we can remove it safely after advancing the queue.
    struct task_node* temp = head;
    head = head->next;
    free_task_node(temp);
  }
  tail = NULL;
  return;
}
