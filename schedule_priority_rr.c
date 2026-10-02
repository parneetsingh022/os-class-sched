#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "cpu.h"
#include "list.h"


// Each node stores a task, the amount of burst time still remaining,
// and a pointer to the next task in the ready queue.
struct task_node {
    Task *task;
    int remaining_bursts;
    struct task_node *next;
};

// Ready queue for round-robin scheduling.
// head points to the next task to run, and tail points to the last task in the queue.
struct task_node* head = NULL;
struct task_node* tail = NULL;

static void free_task_node(struct task_node* node) {
  // Release the memory for a finished task and its list node.
  if (node == NULL)
    return;
  
  free(node->task->name);
  free(node->task);
  free(node);
}


// The ready queue is maintained in descending priority order.
// A larger priority value means a higher priority.
// Tasks with the same priority remain in FIFO order so that
// round-robin scheduling is preserved among equal-priority tasks.
static void enqueue(struct task_node *node)
{
    if (node == NULL)
        return;


    // The node is being inserted into the queue, so clear its old
    // next pointer before determining its new position.
    node->next = NULL;

    // Empty queue
    if (head == NULL) {
        head = node;
        tail = node;
        return;
    }

    // New node has higher priority than the head.
    if (node->task->priority > head->task->priority) {
        node->next = head;
        head = node;
        return;
    }

    struct task_node *cur = head;

    // Find the position where the new node should go.
    //
    // We move past tasks with priority >= node's priority.
    // This is important because tasks with the same priority
    // remain in FIFO order for round-robin.
    while (cur->next != NULL &&
           cur->next->task->priority >= node->task->priority) {
        cur = cur->next;
    }

    node->next = cur->next;
    cur->next = node;

    // If inserted at the end, update tail.
    if (node->next == NULL) {
        tail = node;
    }
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
    // Create the linked-list node that will hold this task.
    struct task_node *node = malloc(sizeof *node);
    if (node == NULL)
        goto memory_alloc_node_error;

    // Allocate memory for the task data itself.
    node->task = malloc(sizeof *node->task);
    if (node->task == NULL)
        goto memory_alloc_task_error;

    // Copy the task name to ensure the string remains valid.
    node->task->name = strdup(name);
    if (node->task->name == NULL)
        goto memory_alloc_name_error;

    // Save task properties and initialize the remaining time for round-robin slices.
    node->task->priority = priority;
    node->task->burst = burst;
    node->remaining_bursts = burst;
    node->next = NULL;

    // Give each task a unique ID based on the current tail.
    if (head == NULL) {
      node->task->tid = 0;
    } else {
      node->task->tid = tail->task->tid + 1;
    }

    // Add the task to the ready queue.
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
  // Round-robin scheduling: each task gets a time slice, then goes back to the end of the queue
  // unless it has finished its total burst time.
  struct task_node *cur;
  while ((cur = dequeue()) != NULL) {
    // Use the smaller of the remaining burst time and the time quantum.
    int slice = cur->remaining_bursts < QUANTUM
                  ? cur->remaining_bursts
                  : QUANTUM;

    // Run the task for one time slice.
    run(cur->task, slice);        
    cur->remaining_bursts -= slice;

    // If the task is done, release its memory; otherwise, send it back to the end of the queue.
    if (cur->remaining_bursts == 0) {
      free_task_node(cur);
    } else {
      enqueue(cur);  
    }
  }
}
