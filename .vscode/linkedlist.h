#include "proc.h"

typedef struct Job {
    int id;
    int mem;
    int device_num;
    int time;
} Job;

typedef struct Node {
    Proc data;
    struct Node* next;
} Node;

typedef struct LinkedListQueue {
    Node* front_node;
    int size_num;
} LinkedListQueue;

typedef struct FIFO_Queue {
    LinkedListQueue super;
} FIFO_Queue;

typedef struct SJF_Queue {
    LinkedListQueue super;
} SJF_Queue;

struct Node* create_node(void* data, struct Node* next);
LinkedListQueue init_queue();
int is_empty(struct LinkedListQueue* queue);
int size(struct LinkedListQueue* queue);
void push(struct LinkedListQueue* queue, void* data);
void pop(struct LinkedListQueue* queue);
void* front(struct LinkedListQueue* queue);
void print(struct LinkedListQueue* queue);
void clear(struct LinkedListQueue* queue);
void FIFO_Queue_push(FIFO_Queue* queue, void* data);
void SJF_Queue_push(SJF_Queue* queue, void* data);
