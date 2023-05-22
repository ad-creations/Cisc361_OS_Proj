#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const bool DEBUG = false;

void debug(const char *message) {
    if (DEBUG)
        printf("%s\n", message);
}

bool cmpJobArr(const pairJobInt a, const pairJobInt b) {
    return (a.first.arrival < b.first.arrival);
}

void handleJobArrival(Job *job, JobPriorityQueue *holdQueue1, JobPriorityQueue *holdQueue2, JobQueue *readyQueue,
                      System *system) {
    if (system->memory < job->memoryRequirement) {
        if (job->priority == 1) {
            priority_queue_push(holdQueue1->jobs, *job);
        } else {
            priority_queue_push(holdQueue2->jobs, *job);
        }
    } else {
        system->memory -= job->memoryRequirement;
        vector_push_back(&readyQueue->jobs, *job);
    }
}

bool bankersAlgo(int k, const vectorInt alloc, const vectorInt maxReq, const vectorInt need, int work) {
    vectorBool finish;
    vector_init(&finish, k, false);
    while (true) {
        bool flag = true;
        for (int i = 0; i < k + 1; ++i) {
            if (finish.data[i] == false && need.data[i] <= work) {
                work += alloc.data[i];
                finish.data[i] = true;
                flag = false;
            }
        }
        if (flag) {
            break;
        }
    }
    bool safe = true;
    for (int i = 0; i < k; ++i) {
        safe = safe && finish.data[i];
    }
    return safe;
}

void handleDeviceRequest(DeviceRequest req, JobQueue *waitQueue, JobQueue *readyQueue, Job *CPU, System *system) {
    int work = system->devices - req.deviceNumber;
    int k = vector_size(&readyQueue->jobs) + 1;
    JobQueue readyCopy;
    vector_pair_init(&readyCopy.jobs);
    vector_copy(&readyCopy.jobs, &readyQueue->jobs);

    vectorInt alloc, maxReq, need;
    vector_init(&alloc, k - 1, 0);
    vector_init(&maxReq, k - 1, 0);
    vector_init(&need, k - 1, 0);

    for (int i = 0; i < k - 1; ++i) {
        alloc.data[i] = readyCopy.jobs.data[i].devicesHeld;
        maxReq.data[i] = readyCopy.jobs.data[i].devicesRequirement;
        need.data[i] = maxReq.data[i] - alloc.data[i];
    }

    alloc.data[k - 1] = CPU->devicesHeld + req.deviceNumber;
    maxReq.data[k - 1] = CPU->devicesRequirement;
    need.data[k - 1] = CPU->devicesRequirement - (CPU->devicesHeld + req.deviceNumber);

    bool safe = bankersAlgo(k, alloc, maxReq, need, work);

    if (safe) {
        CPU->devicesHeld += req.deviceNumber;
        system->devices -= req.deviceNumber;
        vector_push_back(&readyQueue->jobs, *CPU);
    } else {
        CPU->devicesRequesting = req.deviceNumber;
        waitQueue_push(waitQueue, *CPU);
    }
    CPU = NULL;
}

void proccessWaitQueue(DeviceRelease req, JobQueue *waitQueue, JobQueue *readyQueue, Job *CPU, System *system) {
    JobQueue tmpWait;
    waitQueue_init(&tmpWait);
    queue_copy(&tmpWait, waitQueue);

    JobQueue newWaitQueue;
    waitQueue_init(&newWaitQueue);

    while (!queue_empty(&tmpWait)) {
        int work = system->devices - tmpWait.jobs.data[0].devicesRequesting;
        int k = vector_size(&readyQueue->jobs) + 1;
        JobQueue readyCopy;
        vector_pair_init(&readyCopy.jobs);
        vector_copy(&readyCopy.jobs, &readyQueue->jobs);

        vectorInt alloc, maxReq, need;
        vector_init(&alloc, k - 1, 0);
        vector_init(&maxReq, k - 1, 0);
        vector_init(&need, k - 1, 0);

        for (int i = 0; i < k - 1; ++i) {
            alloc.data[i] = readyCopy.jobs.data[i].devicesHeld;
            maxReq.data[i] = readyCopy.jobs.data[i].devicesRequirement;
            need.data[i] = maxReq.data[i] - alloc.data[i];
        }

        alloc.data[k - 1] = tmpWait.jobs.data[0].devicesHeld + tmpWait.jobs.data[0].devicesRequesting;
        maxReq.data[k - 1] = tmpWait.jobs.data[0].devicesRequirement;
        need.data[k - 1] = tmpWait.jobs.data[0].devicesRequirement - (tmpWait.jobs.data[0].devicesHeld +
                                                                       tmpWait.jobs.data[0].devicesRequesting);

        bool safe = bankersAlgo(k + 1, alloc, maxReq, need, work);

        if (safe) {
            system->devices -= tmpWait.jobs.data[0].devicesRequesting;
            tmpWait.jobs.data[0].devicesHeld += tmpWait.jobs.data[0].devicesRequesting;
            tmpWait.jobs.data[0].devicesRequesting = 0;
            vector_push_back(&readyQueue->jobs, tmpWait.jobs.data[0]);
        } else {
            waitQueue_push(&newWaitQueue, tmpWait.jobs.data[0]);
        }
        queue_pop(&tmpWait);
    }
    *waitQueue = newWaitQueue;
}

void handleDeviceRelease(DeviceRelease req, JobQueue *waitQueue, JobQueue *readyQueue, Job *CPU, System *system) {
    system->devices += req.deviceNumber;
    CPU->devicesHeld -= req.deviceNumber;
    readyQueue_push(readyQueue, *CPU);
    CPU = NULL;
    proccessWaitQueue(req, waitQueue, readyQueue, CPU, system);
}

void handleProcessTermination(JobQueue *waitQueue, JobPriorityQueue *holdQueue1, JobPriorityQueue *holdQueue2,
                              JobQueue *readyQueue, Job *CPU, System *system, vectorPairJobInt *doneArr) {
    DeviceRelease dRelease;
    dRelease.jobNumber = CPU->jobNumber;
    dRelease.deviceNumber = CPU->devicesHeld;
    system->devices += dRelease.deviceNumber;
    CPU->devicesHeld -= dRelease.deviceNumber;
    proccessWaitQueue(dRelease, waitQueue, readyQueue, CPU, system);
    system->memory += CPU->memoryRequirement;
    if (!priority_queue_empty(&holdQueue1->jobs)) {
        if (system->memory >= holdQueue1->jobs.data[0].memoryRequirement) {
            readyQueue_push(readyQueue, holdQueue1->jobs.data[0]);
            system->memory -= holdQueue1->jobs.data[0].memoryRequirement;
            priority_queue_pop(&holdQueue1->jobs);
        }
    } else if (!priority_queue_empty(&holdQueue2->jobs)) {
        if (system->memory >= holdQueue2->jobs.data[0].memoryRequirement) {
            readyQueue_push(readyQueue, holdQueue2->jobs.data[0]);
            system->memory -= holdQueue2->jobs.data[0].memoryRequirement;
            priority_queue_pop(&holdQueue2->jobs);
        }
    }
    vector_push_back(doneArr, (PairJobInt){*CPU, system->currTime});
    CPU = NULL;
}

void handleDisplay(JobQueue *waitQueue, JobPriorityQueue *holdQueue1, JobPriorityQueue *holdQueue2,
                   JobQueue *readyQueue, Job *CPU, System *system, vectorPairJobInt *doneArr) {
    double systemTurn = 0;
    vector_pair_sort(doneArr, cmpJobArr);
    printf("At time %d:\n", system->currTime);
    printf("Current Available Main Memory=%d\n", system->memory);
    printf("Current Devices=%d\nCompleted Jobs:\n", system->devices);
    printf("--------------------------------------------------------\n");
    printf("Job ID    Arrival Time    Finish Time    Turnaround Time\n");
    printf("========================================================\n");
    for (int i = 0; i < vector_size(doneArr); i++) {
        systemTurn += (double)(doneArr->data[i].second - doneArr->data[i].first.arrival);
        printf("   %d           %d               %d              %d\n", doneArr->data[i].first.jobNumber,
               doneArr->data[i].first.arrival, doneArr->data[i].second,
               doneArr->data[i].second - doneArr->data[i].first.arrival);
    }
    systemTurn = systemTurn / (double)vector_size(doneArr);
    printf("\n");
    printf("Hold Queue 1:\n-------------------------\nJob ID    Run Time\n=========================\n");
    JobPriorityQueue tmpHQ1;
    priority_queue_copy(&tmpHQ1, &holdQueue1->jobs);
    while (!priority_queue_empty(&tmpHQ1.jobs)) {
        printf("  %d         %d\n", tmpHQ1.jobs.data[0].jobNumber, tmpHQ1.jobs.data[0].burstTime);
        priority_queue_pop(&tmpHQ1.jobs);
    }

    printf("\n");
    printf("Hold Queue 2:\n-------------------------\nJob ID    Run Time\n=========================\n");
    JobPriorityQueue tmpHQ2;
    priority_queue_copy(&tmpHQ2, &holdQueue2->jobs);
    while (!priority_queue_empty(&tmpHQ2.jobs)) {
        printf("  %d         %d\n", tmpHQ2.jobs.data[0].jobNumber, tmpHQ2.jobs.data[0].burstTime);
        priority_queue_pop(&tmpHQ2.jobs);
    }

    printf("\n");
    printf("Ready Queue:\n----------------------------------\nJob ID    Run Time    Time Accrued\n==================================\n");
    JobQueue tmpRQ;
    queue_copy(&tmpRQ, readyQueue);
    while (!queue_empty(&tmpRQ)) {
        printf("  %d         %d             %d\n", tmpRQ.jobs.data[0].jobNumber, tmpRQ.jobs.data[0].burstTime,
               tmpRQ.jobs.data[0].origBTime - tmpRQ.jobs.data[0].burstTime);
        queue_pop(&tmpRQ);
    }

    printf("\n");
    printf("Process running on CPU:\n-----------------------------------\nJob ID    Time Accrued    Time Left\n===================================\n");
    if (CPU != NULL) {
        printf("   %d           %d             %d\n", CPU->jobNumber, CPU->origBTime - CPU->burstTime, CPU->burstTime);
    }
    printf("\n");

    printf("Wait Queue:\n----------------------------------\nJob ID    Run Time    Time Accrued\n==================================\n");
    JobQueue tmpWQ;
    queue_copy(&tmpWQ, waitQueue);
    while (!queue_empty(&tmpWQ)) {
        printf("  %d         %d             %d\n", tmpWQ.jobs.data[0].jobNumber, tmpWQ.jobs.data[0].burstTime,
               tmpWQ.jobs.data[0].origBTime - tmpWQ.jobs.data[0].burstTime);
        queue_pop(&tmpWQ);
    }
    printf("\n");

    if (system->currTime == 9999) {
        printf("System Turnaround Time: %f\n", systemTurn);
    }
}
