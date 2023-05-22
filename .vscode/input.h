#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"

typedef struct System {
    int currTime;
    int memory;
    int devices;
    int quantum;
} System;

typedef struct {
    int jobNumber;
    int memoryRequirement;
    int devicesRequirement;
    int burstTime;
    int priority;
} JobArrivalData;

typedef struct {
    int jobNumber;
    int deviceNumber;
} DeviceRequestData;

typedef struct {
    int jobNumber;
    int deviceNumber;
} DeviceReleaseData;

typedef struct Instruction {
    enum {
        JA,
        DReq,
        DRel,
        Display
    } type;
    int time;
    union {
        JobArrivalData jobArrival;
        DeviceRequestData deviceRequest;
        DeviceReleaseData deviceRelease;
    } data;
} Instruction;

void print(System system, const vector<Instruction>* instructions);

void proccessVec(vector<string>* input, System* system, vector<Instruction>* instructions);

void readInput(string path, vector<Instruction>* instructions, System* system);

#endif
