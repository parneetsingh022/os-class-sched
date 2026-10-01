/**
 * list data structure containing the tasks in the system
 */

#include "task.h"

struct task_node {
    Task *task;
    int remaining_bursts;
    struct task_node *next;
};

// insert and delete operations.
void insert(struct task_node **head, Task *task);
void delete(struct task_node **head, Task *task);
void traverse(struct task_node *head);
