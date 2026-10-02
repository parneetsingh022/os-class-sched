#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "cpu.h"

// Each node stores one task and a pointer to the next task
// in the priority-ordered ready queue.
struct task_node {
  Task* task;
  struct task_node* next;
};

/**
 * Frees the memory used by a task node.
 */
static void free_task_node(struct task_node* node) {
  if (node == NULL)
    return;

  free(node->task->name);
  free(node->task);
  free(node);
}

struct task_node* head = NULL;
struct task_node* tail = NULL;
int next_tid = 0;

/**
 * Adds a task to the ready queue based on its priority.
 * Higher priority tasks are placed before lower priority tasks.
 */
void add(char *name, int priority, int burst)
{

    // Allocate memory for the new task node.
    struct task_node *node = malloc(sizeof *node);
    if (node == NULL)
        goto memory_alloc_node_error;


    // Allocate memory for the Task stored inside the node.
    node->task = malloc(sizeof *node->task);
    if (node->task == NULL)
        goto memory_alloc_task_error;


    // Make a copy of the task name.
    node->task->name = strdup(name);
    if (node->task->name == NULL)
        goto memory_alloc_name_error;

    node->task->priority = priority;
    node->task->burst = burst;
    // Assign a unique ID to the task.
    node->task->tid = next_tid++;
    node->next = NULL;

    /// If the queue is empty, this becomes the first task.
    if (head == NULL) {
      head = node;
      tail = node;
      return;
    }

    struct task_node *cur = head;

    while(cur) {
      struct task_node *next = cur->next;
       // Insert the task before the next node if it has a higher priority.
      // If there is no next node, add it to the end of the queue.
      if (next == NULL || next->task->priority < node->task->priority) {
        cur->next = node;
        node->next = next;
        return;
      }

      cur = next;
    }

    return;

// Free any memory that was successfully allocated before the error.
memory_alloc_name_error:
    free(node->task);

memory_alloc_task_error:
    free(node);

memory_alloc_node_error:
    fprintf(stderr, "failed to allocate memory!\n");
    exit(1);
}

/**
 * Runs the tasks in the ready queue.
 * The highest priority task runs until it finishes.
 */
void schedule()
{
  // The queue is already sorted by priority, so the task at head
  // is always the highest-priority task.
  while (head != NULL) {
    run(head->task, head->task->burst);
    struct task_node* temp = head;
    head = head->next;

    // Free the memory used by the completed task.
    free_task_node(temp);
  }

  tail = NULL;

  return;
}
