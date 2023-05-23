#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "instruction.h"

using namespace std;

void print(System system, const vector<Instruction>& instructions)
{
    printf("System configuration: currTime = %d, memory = %d, quantum = %d\n", system.currTime, system.memory, system.quantum);
    for (unsigned int i = 0; i < instructions.size(); i++)
    {
        switch (instructions[i].type)
        {
        case JA:
            printf("Job arrival: Time = %d, jobNumber = %d, memoryRequirement = %d, burstTime = %d, priority = %d\n", instructions[i].time, instructions[i].data.jobArrival.jobNumber, instructions[i].data.jobArrival.memoryRequirement, instructions[i].data.jobArrival.burstTime, instructions[i].data.jobArrival.priority);
            break;
        case DReq:
            printf("Device request: Time = %d, jobNumber = %d, deviceNumber = %d\n", instructions[i].time, instructions[i].data.deviceRequest.jobNumber, instructions[i].data.deviceRequest.deviceNumber);
            break;
        case DRel:
            printf("Device release: Time = %d, jobNumber = %d, deviceNumber = %d\n", instructions[i].time, instructions[i].data.deviceRelease.jobNumber, instructions[i].data.deviceRelease.deviceNumber);
            break;
        case Display:
            printf("Display: Time = %d\n", instructions[i].time);
            break;
        default:
            printf("Unknown instruction!\n");
        }
    }
}

void proccessVec(vector<string>& input, System* system, vector<Instruction>& instructions)
{
    Instruction curr;
    switch (input[0][0])
    {
    case 'C':
        system->currTime = atoi(input[1].c_str());
        system->memory = atoi(input[2].substr(2).c_str());
        system->devices = atoi(input[3].substr(2).c_str());
        system->quantum = atoi(input[4].substr(2).c_str());
        break;
    case 'A':
        curr.type = JA;
        curr.time = atoi(input[1].c_str());
        curr.data.jobArrival.jobNumber = atoi(input[2].substr(2).c_str());
        curr.data.jobArrival.memoryRequirement = atoi(input[3].substr(2).c_str());
        curr.data.jobArrival.devicesRequirement = atoi(input[4].substr(2).c_str());
        curr.data.jobArrival.burstTime = atoi(input[5].substr(2).c_str());
        curr.data.jobArrival.priority = atoi(input[6].substr(2).c_str());
        instructions.push_back(curr);
        break;
    case 'Q':
        curr.type = DReq;
        curr.time = atoi(input[1].c_str());
        curr.data.deviceRequest.jobNumber = atoi(input[2].substr(2).c_str());
        curr.data.deviceRequest.deviceNumber = atoi(input[3].substr(2).c_str());
        instructions.push_back(curr);
        break;
    case 'L':
        curr.type = DRel;
        curr.time = atoi(input[1].c_str());
        curr.data.deviceRequest.jobNumber = atoi(input[2].substr(2).c_str());
        curr.data.deviceRequest.deviceNumber = atoi(input[3].substr(2).c_str());
        instructions.push_back(curr);
        break;
    case 'D':
        curr.type = Display;
        curr.time = atoi(input[1].c_str());
        instructions.push_back(curr);
        break;
    default:
        // doing nothing
        printf("Bad input\n");
    }
    input.clear();
    input.push_back("");
}

void readInput(char* path, vector<Instruction>& instructions, System* system)
{
    FILE* inputFile = fopen(path, "r");
    if (inputFile != NULL)
    {
        char c;
        vector<string> input;
        input.push_back("");

        int i = 0;
        while (fscanf(inputFile, "%c", &c) != EOF)
        {
            switch (c)
            {
            case ' ':
                input.push_back("");
                i += 1;
                break;
            case '\n':
                proccessVec(input, system, instructions);
                i = 0;
                break;
            case '\r':
                continue;
            default:
                input[i] += c;
            }
        }
        if (input.size() != 0)
        { // Handle end of file instead of newline char.
            proccessVec(input, system, instructions);
        }
        fclose(inputFile);
    }
    else
    {
        std::cerr << "Failed to open file" << std::endl;
    }
}
