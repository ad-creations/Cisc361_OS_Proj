#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "instruction.h"
#include "input.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    Instruction instructions[MAX_INSTRUCTIONS];
    System system;

    vector_init(&instructions);
    system.currTime = 0;

    /* Choose an input file: */
    readInput("../input.txt", instructions, &system);

    int totalMemory = system.memory;
    int totalDevices = system.devices;

    JobQueue readyQueue;
    JobQueue waitQueue;
    vector_init(&readyQueue.jobs);
    vector_init(&waitQueue.jobs);
    vector_pair_init(&doneArr);
    JobPriorityQueue holdQueue1;
    JobPriorityQueue holdQueue2;
    priority_queue_init(&holdQueue1.jobs, cmpQ1);
    priority_queue_init(&holdQueue2.jobs, cmpQ2);

    Job *CPU = NULL;

    int timeOfNextInput;
    int timeOfNextInternalEvent = system.quantum;
    size_t instructionIdx = 0;
    long currQuantum = system.quantum;
    while (true) {
        system.currTime += timeOfNextInput;
        currQuantum -= timeOfNextInput;

        if (CPU != NULL) {
            CPU->burstTime -= timeOfNextInput;
        }
        switch (instructions[instructionIdx].type) {
            case JA: {
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
                if (totalDevices >= tmp.devicesRequirement && totalMemory >= tmp.memoryRequirement) {
                    handleJobArrival(&tmp, &holdQueue1, &holdQueue2, &readyQueue, &system);
                }
                break;
            }
            case DRel:
                if (instructions[instructionIdx].data.deviceRelease.jobNumber == CPU->jobNumber &&
                    (instructions[instructionIdx].data.deviceRelease.deviceNumber) <= CPU->devicesHeld) {
                    handleDeviceRelease(&instructions[instructionIdx].data.deviceRelease, &waitQueue, &readyQueue, CPU, &system);
                    currQuantum = system.quantum;
                }
                break;
            case DReq:
                if (instructions[instructionIdx].data.deviceRequest.jobNumber == CPU->jobNumber &&
                    (instructions[instructionIdx].data.deviceRequest.deviceNumber + CPU->devicesHeld) <= CPU->devicesRequirement) {
                    handleDeviceRequest(&instructions[instructionIdx].data.deviceRequest, &waitQueue, &readyQueue, CPU, &system);
                    currQuantum = system.quantum;
                }
                break;
            case Display:
                handleDisplay(&waitQueue, &holdQueue1, &holdQueue2, &readyQueue, CPU, &system, &doneArr);
                break;
            default:
                printf("Erroneous type value of event!\n");
                break;
        }
        instructionIdx += 1;
        system.currTime += timeOfNextInternalEvent;

        if (CPU != NULL) {
            CPU->burstTime -= timeOfNextInternalEvent;
        }
        if (CPU != NULL && CPU->burstTime == 0) {
            handleProcessTermination(&waitQueue, &holdQueue1, &holdQueue2, &readyQueue, CPU, &system, &doneArr);
            currQuantum = system.quantum;
        } else {
            if (CPU != NULL) {
                vector_push_back(&readyQueue.jobs, *CPU);
            }
            CPU = NULL;
        }
        currQuantum = system.quantum;
        if (CPU == NULL && priority_queue_empty(&holdQueue1.jobs) && priority_queue_empty(&holdQueue2.jobs) &&
            vector_empty(&readyQueue.jobs) && vector_empty(&waitQueue.jobs) && instructionIdx >= vector_size(&instructions)) {
            break;
        }
    }
    return 0;
}
