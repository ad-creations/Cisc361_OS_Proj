#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS 100

typedef struct {
    int jobNumber;
    int memoryRequirement;
    int devicesRequirement;
    int burstTime;
    int priority;
} JobArrival;

typedef struct {
    int jobNumber;
    int deviceNumber;
} DeviceRequest, DeviceRelease;

typedef enum {
    JA,
    DReq,
    DRel,
    Display
} InstructionType;

typedef struct {
    InstructionType type;
    int time;
    union {
        JobArrival jobArrival;
        DeviceRequest deviceRequest;
        DeviceRelease deviceRelease;
    } data;
} Instruction;

typedef struct {
    int currTime;
    int memory;
    int devices;
    int quantum;
} System;

void print(const System *system, const Instruction *instructions, int numInstructions) {
    printf("System configuration: currTime = %d, memory = %d, quantum = %d\n",
           system->currTime, system->memory, system->quantum);

    for (int i = 0; i < numInstructions; i++) {
        switch (instructions[i].type) {
            case JA:
                printf("Job arrival: Time = %d, jobNumber = %d, memoryRequirement = %d, burstTime = %d, priority = %d\n",
                       instructions[i].time,
                       instructions[i].data.jobArrival.jobNumber,
                       instructions[i].data.jobArrival.memoryRequirement,
                       instructions[i].data.jobArrival.burstTime,
                       instructions[i].data.jobArrival.priority);
                break;
            case DReq:
                printf("Device request: Time = %d, jobNumber = %d, deviceNumber = %d\n",
                       instructions[i].time,
                       instructions[i].data.deviceRequest.jobNumber,
                       instructions[i].data.deviceRequest.deviceNumber);
                break;
            case DRel:
                printf("Device release: Time = %d, jobNumber = %d, deviceNumber = %d\n",
                       instructions[i].time,
                       instructions[i].data.deviceRelease.jobNumber,
                       instructions[i].data.deviceRelease.deviceNumber);
                break;
            case Display:
                printf("Display: Time = %d\n", instructions[i].time);
                break;
            default:
                printf("Unknown instruction!\n");
                break;
        }
    }
}

void processVec(char **input, System *system, Instruction *instructions, int *numInstructions) {
    Instruction curr;
    switch ((*input)[0]) {
        case 'C':
            system->currTime = atoi(input[1]);
            system->memory = atoi(input[2] + 2);
            system->devices = atoi(input[3] + 2);
            system->quantum = atoi(input[4] + 2);
            break;
        case 'A':
            curr.type = JA;
            curr.time = atoi(input[1]);
            curr.data.jobArrival.jobNumber = atoi(input[2] + 2);
            curr.data.jobArrival.memoryRequirement = atoi(input[3] + 2);
            curr.data.jobArrival.devicesRequirement = atoi(input[4] + 2);
            curr.data.jobArrival.burstTime = atoi(input[5] + 2);
            curr.data.jobArrival.priority = atoi(input[6] + 2);
            instructions[(*numInstructions)++] = curr;
            break;
        case 'Q':
            curr.type = DReq;
            curr.time = atoi(input[1]);
            curr.data.deviceRequest.jobNumber = atoi(input[2] + 2);
            curr.data.deviceRequest.deviceNumber = atoi(input[3] + 2);
            instructions[(*numInstructions)++] = curr;
            break;
        case 'L':
            curr.type = DRel;
            curr.time = atoi(input[1]);
            curr.data.deviceRelease.jobNumber = atoi(input[2] + 2);
            curr.data.deviceRelease.deviceNumber = atoi(input[3] + 2);
            instructions[(*numInstructions)++] = curr;
            break;
        case 'D':
            curr.type = Display;
            curr.time = atoi(input[1]);
            instructions[(*numInstructions)++] = curr;
            break;
        default:
            // do nothing
            printf("Bad input\n");
            break;
    }
    input[0] = "";
}

void readInput(const char *path, Instruction *instructions, System *system, int *numInstructions) {
    FILE *inputFile = fopen(path, "r");
    if (inputFile) {
        char c;
        char input[100][100];
        int inputIndex = 0;
        int i = 0;
        while ((c = fgetc(inputFile)) != EOF) {
            switch (c) {
                case ' ':
                    inputIndex++;
                    i = 0;
                    break;
                case '\n':
                    processVec(input, system, instructions, numInstructions);
                    inputIndex = 0;
                    break;
                case '\r':
                    continue;
                default:
                    input[inputIndex][i++] = c;
                    break;
            }
        }
        if (inputIndex != 0) {
            processVec(input, system, instructions, numInstructions);
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open file\n");
    }
}

int main() {
    System system;
    Instruction instructions[MAX_INSTRUCTIONS];
    int numInstructions = 0;

    readInput("input.txt", instructions, &system, &numInstructions);
    print(&system, instructions, numInstructions);

    return 0;
}
