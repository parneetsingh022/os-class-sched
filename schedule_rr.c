#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "cpu.h"
#include "list.h"

struct task_node* head = NULL;
struct task_node* tail = NULL;

static void free_task_node(struct task_node* node) {
  if (node == NULL)
    return;
  
  free(node->task->name);
  free(node->task);
  free(node);
}

static void enqueue(struct task_node *node)
{
  if (node == NULL)
    return;

  node->next = NULL;

  // If the queue is empty we set the node as its head.
  if (head == NULL) {
    head = node;
    tail = node;
    return;
  }

  tail->next = node;
  tail = node;
}

static struct task_node *dequeue()
{
  if (head == NULL)
    return NULL;  

  struct task_node *ret = head;
  head = head->next;

  if (head == NULL)
    tail = NULL;

  return ret;
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
    node->remaining_bursts = burst;
    node->next = NULL;
    if (head == NULL) {
      node->task->tid = 0;
    } else {
      node->task->tid = tail->task->tid + 1;
    }


    enqueue(node);

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
  struct task_node *cur;
  while ((cur = dequeue()) != NULL) {
    int slice = cur->remaining_bursts < QUANTUM
                  ? cur->remaining_bursts
                  : QUANTUM;

    run(cur->task, slice);        
    cur->remaining_bursts -= slice;

    if (cur->remaining_bursts == 0) {
      free_task_node(cur);
    } else {
      enqueue(cur);  
    }
  }
}
