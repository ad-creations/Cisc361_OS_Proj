#include <stdio.h>
#include <stdlib.h>
#include "job.h"
#include "command.h"
#include "system.h"

// Create a new job from a command
struct Job* newJob(struct Command* c) {
    struct Job* newJob = malloc(sizeof(struct Job));
    if (newJob == NULL) {
        return NULL;
    }
    newJob->next = NULL;
    newJob->jobId = c->jobId;
    newJob->priority = c->priority;
    newJob->burstTime = c->runTime;
    newJob->needMemory = c->memory;
    newJob->needDevice = c->devices;
    return newJob;
}

// Display details of a job
void displayJob(struct Job* j, int type) {
    switch (type) {
        case 1:
            printf("Type: SJF\n");
            break;
        case 2:
            printf("Type: FIFO\n");
            break;
        default:
            printf("Invalid\n");
            break;
    }
}

// Create a new queue of jobs
struct Queue* newQueue(int type) {
    struct Queue* newQueue = malloc(sizeof(struct Queue));
    if (newQueue == NULL) {
        return NULL;
    }
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->queueType = type;
    return newQueue;
}

// Push a job to the queue
void pushQueue(struct Queue* q, struct Job* j) {
    if (q->head == NULL) {
        q->head = j;
        q->tail = j;
    } else if (q->queueType == 1) { // SJF
        struct Job* curr = q->head;
        struct Job* prev = NULL;

        while (curr != NULL && j->burstTime >= curr->burstTime) {
            prev = curr;
            curr = curr->next;
        }

        if (prev == NULL) {
            j->next = curr;
            q->head = j;
        } else {
            j->next = curr;
            prev->next = j;
        }
    } else if (q->queueType == 2) { // FIFO
        q->tail->next = j;
        q->tail = j;
    }
}

// Remove the first job from the queue
struct Job* popQueue(struct Queue* q) {
    if (q == NULL || q->head == NULL) {
        return NULL;
    }

    struct Job* remove = q->head;
    q->head = q->head->next;
    remove->next = NULL;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    return remove;
}

// Check if the queue is empty
int emptyQueue(struct Queue* q) {
    return (q == NULL || q->head == NULL);
}

void printQueue(struct Queue* queue) {
    printf("---------printing queue------\n");
    struct Job* curr = queue->head;

    while (curr != NULL) {
        printf("JobNumber: %d with remaining %d\n", curr->jobId, curr->burstTime - curr->leftTime);
        curr = curr->next;
    }

    printf("----------------------------\n");
}
