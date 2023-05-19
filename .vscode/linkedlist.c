#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

// Constructor
Job create_default_job() {
    Job default_job;
    default_job.jobId = -1;
    default_job.needMemory = -1;
    default_job.needDevice = -1;
    default_job.totalTime = -1;
    return default_job;
}

Job create_job_with_values(int id, int mem, int device_num, int time) {
    Job values_job;
    values_job.jobId = id;
    values_job.needMemory = mem;
    values_job.needDevice = device_num;
    values_job.totalTime = time;
    return values_job;
}

// Creates a new Node
Node* create_node(void* data, Node* next) {
    Node* new_node = (Node*) malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = next;
    return new_node;
}

// Initializes a new empty LinkedListQueue
LinkedListQueue* init_queue() {
    LinkedListQueue* queue = (LinkedListQueue*) malloc(sizeof(LinkedListQueue));
    queue->front_node = NULL;
    queue->size_num = 0;
    return queue;
}

// Returns true if the queue is empty
int is_empty(LinkedListQueue* queue) {
    return queue->size_num == 0;
}

// Returns the number of elements in the queue
int size(LinkedListQueue* queue) {
    return queue->size_num;
}

// Adds a new element to the back of the queue
void push(LinkedListQueue* queue, void* data) {
    Node* new_node = create_node(data, NULL);
    if (is_empty(queue)) {
        queue->front_node = new_node;
    } else {
        Node* back_node = queue->front_node;
        while (back_node->next != NULL) {
            back_node = back_node->next;
        }
        back_node->next = new_node;
    }
    queue->size_num++;
}

// Removes the first element from the queue
void pop(LinkedListQueue* queue) {
    if (!is_empty(queue)) {
        Node* old_front_node = queue->front_node;
        queue->front_node = queue->front_node->next;
        free(old_front_node->data);
        free(old_front_node);
        queue->size_num--;
    }
}

// Returns the first element in the queue
void* front(LinkedListQueue* queue) {
    if (!is_empty(queue)) {
        return queue->front_node->data;
    } else {
        return NULL;
    }
}

// Prints the contents of the queue
void print(LinkedListQueue* queue) {
    Node* current_node = queue->front_node;
    while (current_node != NULL) {
        printf("%d ", *(int*) current_node->data);
        current_node = current_node->next;
    }
    printf("\n");
}

// Deallocates all nodes in the queue
void clear(LinkedListQueue* queue) {
    while (!is_empty(queue)) {
        pop(queue);
    }
}

// Override FIFO push
void FIFO_Queue_push(FIFO_Queue* queue, void* data) {
    Node* new_node = create_node(data, NULL);
    push((LinkedListQueue*) queue, new_node);
}

// Override SJF push
void SJF_Queue_push(SJF_Queue* queue, void* data) {
    Node* new_node = create_node(data, NULL);
    if (is_empty((LinkedListQueue*) queue)) {
        queue->super.front_node = new_node;
    }
}

