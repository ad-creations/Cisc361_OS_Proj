#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "job.h"
#include "command.h"

struct System* newSystem(struct Command* c) {
    struct System* system = (struct System*)malloc(sizeof(struct System));

    system->time = 0;
    system->totalMemory = -1;
    system->curMemory = -1;
    system->totalDevice = -1;
    system->curDevice = -1;
    system->timeQuantum = -1;
    system->holdQueue1 = newQueue(1);
    system->holdQueue2 = newQueue(2);
    system->waitQueue = newQueue(-1);
    system->readyQueue = newQueue(-1);
    system->leaveQueue = newQueue(-1);
    system->running = NULL;
    system->startTime = 0;

    return system;
}

void arriveJob(struct System* s, struct Job* job) {
    if (job->needMemory > s->totalMemory || job->needDevice > s->totalDevice) {
        printf("Job %d is rejected, resource is not enough\n", job->jobId);
    } else if (s->curMemory >= job->needMemory) {
        printf("Adding job %d to ready queue\n", job->jobId);
        pushQueue(s->readyQueue, job);
        s->curMemory -= job->needMemory;
        printf("Added Job %d\n", job->jobId);
    } else {
        if (job->priority == 1) {
            printf("Adding job %d to Hold Queue 1\n", job->jobId);
            pushQueue(s->holdQueue1, job);
            printf("Done Adding SJF!\n");
        } else {
            printf("Adding job %d to Hold Queue 2\n", job->jobId);
            pushQueue(s->holdQueue2, job);
            printf("Done adding FIFO!\n");
        }
    }
}

void scheduleQueue(struct System* s) {
    while (s->holdQueue1->size != 0) {
        struct Job* j = popQueue(s->holdQueue1);
        pushQueue(s->readyQueue, j);
    }

    while (s->holdQueue2->size != 0) {
        struct Job* j = popQueue(s->holdQueue2);
        pushQueue(s->readyQueue, j);
    }
}

void moveOutHold(struct System* s) {
    while (s->holdQueue1->size != 0) {
        struct Job* j = popQueue(s->holdQueue1);
        pushQueue(s->leaveQueue, j);
    }

    while (s->holdQueue2->size != 0) {
        struct Job* j = popQueue(s->holdQueue2);
        pushQueue(s->leaveQueue, j);
    }
}

void moveReadyToRunning(struct System* s) {
    if (s->running == NULL) {
        return;
    }

    s->running->leftTime -= s->timeQuantum;

    if (s->running->leftTime <= 0) {
        s->running->leaveTime = s->time;
        pushQueue(s->leaveQueue, s->running);
        s->running = NULL;
    } else {
        pushQueue(s->readyQueue, s->running);
        s->running = NULL;
    }
}


void jobComplete(struct System* s) {
    if (s->running == NULL) {
        return;
    }

    pushQueue(s->leaveQueue, s->running);
    s->running = NULL;
}

void moveWaitToReady(struct System* s) {
    if (s->waitQueue->size != 0) {
        struct Job* j = popQueue(s->waitQueue);
        pushQueue(s->readyQueue, j);
    }
}

void moveRunningToReady(struct System* s) {
    if (s->running == NULL) {
        return;
    }

    pushQueue(s->readyQueue, s->running);
    s->running = NULL;
}

void moveRunningToWait(struct System* s) {
    if (s->running == NULL) {
        return;
    }

    pushQueue(s->waitQueue, s->running);
    s->running = NULL;
}

void requestDevice(struct System* s, struct Command* c, int numProcesses) {
    int safe = bankers(s, c, numProcesses);
    if (safe == 1) {
        c->devices += s->curDevice;
        s->curDevice -= c->devices;
        pushQueue(s->readyQueue, s->running);
    } else {
        pushQueue(s->waitQueue, s->running);
    }
}

void releaseDevice(struct System* s, struct Command* c) {
    s->totalDevice += c->devices;
    s->running->holdDevice -= c->devices;

    pushQueue(s->readyQueue, s->running);
    s->running = NULL;
}


int bankers(struct System* s, struct Command* c, int numProcesses) {
    int *allocated = malloc(numProcesses * sizeof(int));
    int *max = malloc(numProcesses * sizeof(int));
    int *need = malloc(numProcesses * sizeof(int));
    int *finish = malloc(numProcesses * sizeof(int));

    //set allocated, max, and need lists 
    struct Job* job = s->readyQueue->head;
    int totalAllocated =0;
    for(int i = 0; i < numProcesses; i++){
        allocated[i] = job->holdDevice;
        max[i] = job->requestDevice;
        need[i] = max[i] - allocated[i];
        totalAllocated +=  allocated[i];
        job = job -> next;
    }


    int availible = s->totalDevice - totalAllocated;
    
    int work = availible;

    for (int i = 0; i <= numProcesses; i++) {
        finish[i] = 0;
    }

    // Check if the requested resources can be granted
    for (int i = 0; i <= numProcesses; i++) {
        if (need[i] > work) {
            free(allocated);
            free(max);
            free(need);
            free(finish);

            return 0;
        }
    }

    // Check for a safe state
    for(int i = 0;i < numProcesses; i++){
        for(int j = 0;j<numProcesses;j++){
            if(finish[j] == 0 && need[j] <= work){
                work += allocated[j];
                finish[j] = 1;
            }
        }
    }

    for(int i = 0;i < numProcesses; i++){
        if(finish[i] == 0){
            printf("unsafe state");

            //frees memory before jumping out
            free(allocated);
            free(max);
            free(need);
            free(finish);

            return 1;
        }
    }

    free(allocated);
    free(max);
    free(need);
    free(finish);

    return 0;
}

#include <stdio.h>

// Function to print the current status of the scheduler at a given time
int printAtTime(struct System *s, int availableMem, int availableDevices)
{
    int sum = 0; // Sum of turnaround times
    int count = 0; // Number of completed jobs

    // Print current available memory and devices
    printf("At Time %d:\n", s->time);
    printf("Current Available Main Memory: %d\n", s->totalMemory - availableMem);
    printf("Current Devices: %d\n", s->totalDevice - availableDevices);
    printf("-------------------------------------------------\n");

    // Print completed jobs
    printf("Completed Jobs:\n");
    struct Job *job = s->leaveQueue->head;
    while (job != NULL)
    {
        sum += (job->leaveTime - job->arrivalTime);
        count++;
        printf("Job ID: %d\n", job->jobId);
        printf("Arrival Time: %d\n", job->arrivalTime);
        printf("Finish Time: %d\n", job->leaveTime);
        printf("Turn Around Time: %d\n", (job->leftTime - job->arrivalTime));
        printf("---------------------------------------\n");

        job = job->next;
    }
    printf("-------------------------------------------------\n");

    // Print jobs in Hold Queue 1
    printf("Hold Queue 1:\n");
    job = s->holdQueue1->head;
    while (job != NULL)
    {
        printf("Job ID: %d\n", job->jobId);
        printf("Run Time: %d\n", job->burstTime);
        printf("---------------------------------------\n");

        job = job->next;
    }
    printf("-------------------------------------------------\n");

    // Print jobs in Hold Queue 2
    printf("Hold Queue 2:\n");
    job = s->holdQueue2->head;
    while (job != NULL)
    {
        printf("Job ID: %d\n", job->jobId);
        printf("Run Time: %d\n", job->burstTime);
        printf("---------------------------------------\n");

        job = job->next;
    }
    printf("-------------------------------------------------\n");

    // Print jobs in Ready Queue
    printf("Ready Queue:\n");
    job = s->readyQueue->head;
    while (job != NULL)
    {
        printf("Job ID: %d\n", job->jobId);
        printf("Run Time: %d\n", job->burstTime);
        printf("Time Accrued: %d\n", job->totalTime);
        printf("---------------------------------------\n");

        job = job->next;
    }
    printf("-------------------------------------------------\n");

    // Print jobs in Wait Queue
    printf("Wait Queue:\n");
    job = s->waitQueue->head;
    while (job != NULL)
    {
        printf("Job ID: %d\n", job->jobId);
        printf("Run Time: %d\n", job->burstTime);
        printf("Time Accrued: %d\n", job->totalTime);
        printf("---------------------------------------\n");

        job = job->next;
    }
    printf("-------------------------------------------------\n");

    // Print job running on CPU
    printf("Running on CPU:\n");
    printf("-------------------------------------------------\n");

    // Calculate and print the average turnaround time
    float turnaround = 0.0;
    if (count > 0)
    {
        turnaround = (float)sum / count;
    }
    printf("System Turnaround Time: %.2f\n\n", turnaround);

    return 0;
}
