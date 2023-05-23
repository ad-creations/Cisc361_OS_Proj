#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <instruction.h>
#include <queue>

using namespace std;

void debug(string message);

struct Job
{
    int jobNumber;
    int arrival;
    int memoryRequirement;
    int devicesRequirement;
    int devicesRequesting;
    int devicesHeld;
    int burstTime;
    int origBTime;
    int priority;
};

struct cmpQ1
{
    bool operator()(struct Job a, struct Job b)
    {
        if (a.burstTime != b.burstTime)
        {
            return (a.burstTime < b.burstTime);
        }
        else
        {
            return (a.arrival < b.arrival);
        }
    }
};

struct cmpQ2
{
    bool operator()(struct Job a, struct Job b)
    {
        return (a.arrival < b.arrival);
    }
};


void handleJobArrival(struct Job job, priority_queue<struct Job, vector<struct Job>, cmpQ1> *holdQueue1, priority_queue<struct Job,
                      vector<struct Job>, cmpQ2> *holdQueue2, queue<struct Job> *readyQueue,System *system);

void handleDeviceRequest(DeviceRequest req, queue<struct Job> *waitQueue, queue<struct Job> *readyQueue, struct Job **CPU, System *system);

void handleDeviceRelease(DeviceRelease req, queue<struct Job> *waitQueue, queue<struct Job> *readyQueue, struct Job **CPU, System *system);

void handleDisplay(queue<struct Job> *waitQueue,priority_queue<struct Job, vector<struct Job>, cmpQ1> *holdQueue1,
                   priority_queue<struct Job, vector<struct Job>, cmpQ2> *holdQueue2,
                   queue<struct Job> *readyQueue, struct Job **CPU, System *system, vector<pair<struct Job,int>> *doneArr);

void handleProcessTermination(queue<struct Job> *waitQueue,priority_queue<struct Job, vector<struct Job>, cmpQ1> *holdQueue1,
                              priority_queue<struct Job, vector<struct Job>, cmpQ2> *holdQueue2,
                              queue<struct Job> *readyQueue, struct Job **CPU, System *system, vector<pair<struct Job,int>> *doneArr);

#endif
