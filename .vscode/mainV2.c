#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <instruction.h>
#include <input.h>
#include <utils.h>
#include <iostream>


using namespace std;

int main(int argc, char *argv[])
{
    vector<Instruction> instructions;
    System system;
    readInput("../input/i0.txt", instructions, &system);

    int totalMemory = system.memory;
    int totalDevices = system.devices;

    queue<Job> readyQueue;
    queue<Job> waitQueue;
    vector<pair<Job, int>> doneArr;
    priority_queue<struct Job, vector<struct Job>, cmpQ1> holdQueue1;
    priority_queue<struct Job, vector<struct Job>, cmpQ2> holdQueue2;

    Job *CPU = NULL;

    int timeOfNextInput;
    int timeOfNextInternalEvent = system.quantum;
    long unsigned int instructionIdx = 0;
    long currQuantum = system.quantum;
    while (true)
    {
        printf("\n\nTop of loop with currTime = %d\n", system.currTime);
        if (CPU == NULL && !readyQueue.empty())
        {
            CPU = &readyQueue.front();
            currQuantum = system.quantum;
            printf("Adding %d to the CPU\n", CPU->jobNumber);
            readyQueue.pop();
        }

        bool instructionIsInternal = false;
        if (instructionIdx < instructions.size())
        {
            timeOfNextInput = instructions[instructionIdx].time - system.currTime;
            if (instructions[instructionIdx].type == DRel || instructions[instructionIdx].type == DReq)
            {
                instructionIsInternal = true;
            }
        }
        else
        {
            printf("There are no next inputs, nextInput is INT16_MAX\n");
            timeOfNextInput = INT16_MAX - 1;
        }


        if (CPU != NULL && CPU->burstTime < currQuantum)
        {

            timeOfNextInternalEvent = CPU->burstTime;
            printf("Internal event is set to the CPU's burst time\n");
        }
        else if (CPU == NULL) 
        {                       
            timeOfNextInternalEvent = INT16_MAX;
            printf("Nothing on the CPU, internal event set to INT_MAX\n");
        }
        else
        {
            timeOfNextInternalEvent = currQuantum;
            printf("Internal event is set to the current quantum of %d\n", currQuantum);
        }

        printf("Time till nextInput = %d\n", timeOfNextInput);
        printf("Time till nextInternal = %d\n", timeOfNextInternalEvent);
        if (((timeOfNextInput == timeOfNextInternalEvent) && instructionIsInternal))
        {
            printf("time of next input == time of next internal event and instruction is internal\n");
        }
        if ((timeOfNextInput < timeOfNextInternalEvent))
        {
        system.currTime += timeOfNextInput;
        currQuantum -= timeOfNextInput;
        printf("Jumping to the next instruction, currTime = %d\n", system.currTime);
        if (CPU != nullptr)
        {
            CPU->burstTime -= timeOfNextInput;
        }
        switch (instructions[instructionIdx].type)
        {
        case JA:
            Job tmp;
            tmp.jobNumber = instructions[instructionIdx].data.jobArrival.jobNumber;
            tmp.arrival = instructions[instructionIdx].time;
            tmp.memoryRequirement = instructions[instructionIdx].data.jobArrival.memoryRequirement;
            tmp.devicesRequirement = instructions[instructionIdx].data.jobArrival.devicesRequirement;
            tmp.devicesRequesting = 0;
            tmp.devicesHeld = 0;
            tmp.burstTime = instructions[instructionIdx].data.jobArrival.burstTime;
            tmp.origBTime = tmp.burstTime;
            tmp.priority = instructions[instructionIdx].data.jobArrival.priority;
            if (totalDevices >= tmp.devicesRequirement && totalMemory >= tmp.memoryRequirement)
            {
                printf("handling job arrival, job number %d\n", tmp.jobNumber);
                handleJobArrival(tmp, holdQueue1, holdQueue2, readyQueue, &system);
            }
            break;
        case DRel:
            if (instructions[instructionIdx].data.deviceRelease.jobNumber == CPU->jobNumber &&
                (instructions[instructionIdx].data.deviceRelease.deviceNumber) <= CPU->devicesHeld)
            {
                printf("handling device release on job number %s\n", to_string(instructions[instructionIdx].data.deviceRelease.jobNumber));
                handleDeviceRelease(instructions[instructionIdx].data.deviceRelease, waitQueue, readyQueue, CPU, &system);
   
                currQuantum = system.quantum;
            }
            break;
        case DReq:
            if (instructions[instructionIdx].data.deviceRequest.jobNumber == CPU->jobNumber &&
                (instructions[instructionIdx].data.deviceRequest.deviceNumber + CPU->devicesHeld) <= CPU->devicesRequirement)
            {
                printf("handling device request on job number %s\n", to_string(instructions[instructionIdx].data.deviceRequest.jobNumber));
                handleDeviceRequest(instructions[instructionIdx].data.deviceRequest, waitQueue, readyQueue, CPU, &system);
               
                currQuantum = system.quantum;
            }
            break;
        case Display:
            handleDisplay(waitQueue, holdQueue1, holdQueue2, readyQueue, CPU, &system, doneArr);
            break;
        default:
            printf("Erroneous type value of event!\n");
        }

        instructionIdx += 1;
    }
    else
    {

        system.currTime += timeOfNextInternalEvent;
        printf("Jumping to the end of the quantum, currTime = %d\n", system.currTime);
        if (CPU != nullptr)
        {
            CPU->burstTime -= timeOfNextInternalEvent;
        }

        if (CPU != nullptr && CPU->burstTime == 0)
        {
            printf("Job on CPU is terminating, jobNumber = %d\n", CPU->jobNumber);
            handleProcessTermination(waitQueue, holdQueue1, holdQueue2, readyQueue, CPU, &system, doneArr);
        }
        else
        { 
            printf("Job on CPU has been worked on, moving now to the back of the readyQueue, jobNumber = %d\n", CPU->jobNumber);
            if (CPU != nullptr)
            {
                readyQueue.push(*CPU);
            }
            CPU = nullptr;
        }
        currQuantum = system.quantum;
    }
    if (CPU == nullptr && holdQueue1.empty() && holdQueue2.empty() && readyQueue.empty() && waitQueue.empty() && instructionIdx >= instructions.size())
    {
        break;
    }
}
    return 0;
}
