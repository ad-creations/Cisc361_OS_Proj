#include "job.h"
#include "command.h"

// Define the System struct
typedef struct System System;

// Declare the System struct
struct System {
    int time;               // Current time
    int totalMemory;        // Total memory capacity
    int curMemory;          // Current memory usage
    int totalDevice;        // Total device capacity
    int curDevice;          // Current device usage
    int timeQuantum;        // Time quantum for scheduling
    struct Queue* holdQueue1;   // Queue for holding jobs
    struct Queue* holdQueue2;   // Queue for holding jobs
    struct Queue* waitQueue;    // Queue for waiting jobs
    struct Queue* readyQueue;   // Queue for ready jobs
    struct Queue* leaveQueue;   // Queue for completed jobs
    struct Job* running;    // Currently running job
    int startTime;          // Start time of the system
};

#ifndef SYSTEM_H
#define SYSTEM_H

// Function to create a new System
struct System* newSystem(struct Command* c);

// Function to determine the next event in the system
int nextEvent(struct System* s, struct Command* c);

// Function to handle arrival of a job in the system
void arriveJob(struct System* s, struct Job* j);

// Function to schedule jobs in the ready queue
void scheduleQueue(struct System* s);

// Function to move jobs out of the hold queues
void moveOutHold(struct System* s);

// Function to move a job from the ready queue to running state
void moveReadyToRunning(struct System* s);

// Function to handle completion of a job
void jobComplete(struct System* s);

// Function to move a job from the wait queue to the ready queue
void moveWaitToReady(struct System* s);

// Function to move a running job to the ready queue
void moveRunningToReady(struct System* s);

// Function to move a running job to the wait queue
void moveRunningToWait(struct System* s);

// Function to request devices for a job
void requestDevice(struct System* s, struct Command* c, int numProcesses);

// Function to release devices held by a job
void releaseDevice(struct System* s, struct Command* c);

// Function to perform the banker's algorithm for resource allocation
int bankers(struct System* s, struct Command* c, int numProcesses);

// Function to print the current status of the scheduler at a given time
int printAtTime(struct System* s, int availableMem, int availableDevices);

#endif
