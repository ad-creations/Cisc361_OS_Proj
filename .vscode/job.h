#define SJF_Queue 1
#define FIFO_Queue 2

#include "command.h"

#ifndef JOB_H
#define JOB_H

struct Job {
    int jobId;
    int priority;
    int needMemory;
    int needDevice;
    int holdDevice;      
    int requestDevice; 
    int totalTime;
    int burstTime;
    int leftTime;
    int arrivalTime;
    int leaveTime;
    struct Job* next;
};

struct Queue {
    struct Job* head;
    struct Job* tail;
    int queueType;   // 1: SJF, 2: FIFO
    int size;
};

// Create a new job from a command
struct Job* newJob(struct Command* c);

// Display details of a job based on the queue type
void displayJob(struct Job* j, int type);

// Create a new queue of jobs
struct Queue* newQueue(int type);

// Push a job to the queue based on the queue type
void pushQueue(struct Queue* q, struct Job* j);

// Remove the first job from the queue
struct Job* popQueue(struct Queue* q);

// Check if the queue is empty
int emptyQueue(struct Queue* q);

// Display details of a job based on the queue type
void displayJob(struct Job* j, int type);

// Print the contents of the queue
void printQueue(struct Queue* queue);

#endif
