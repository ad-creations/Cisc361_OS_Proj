#include <fstream>
#include <iostream>
#include <vector>
#include <instruction.h>

using namespace std;

void print(System system, const vector<Instruction> &instructions)
{
    cout << "System configuration: currTime = " << system.currTime << ", memory = " << system.memory << ", quantum = " << system.quantum << endl;
    for (long unsigned int i = 0; i < instructions.size(); i++)
    {
        switch (instructions[i].type)
        {
        case JA:
            cout << "Job arrival: Time = " << instructions[i].time;
            cout << ", jobNumber = " << instructions[i].data.jobArrival.jobNumber;
            cout << ", memoryRequirement = " << instructions[i].data.jobArrival.memoryRequirement;
            cout << ", burstTime = " << instructions[i].data.jobArrival.burstTime;
            cout << ", priority = " << instructions[i].data.jobArrival.priority;
            cout << endl;
            break;
        case DReq:
            cout << "Device request: Time = " << instructions[i].time;
            cout << ", jobNumber = " << instructions[i].data.deviceRequest.jobNumber;
            cout << ", deviceNumber = " << instructions[i].data.deviceRequest.deviceNumber;
            cout << endl;
            break;
        case DRel:
            cout << "Device release: Time = " << instructions[i].time;
            cout << ", jobNumber = " << instructions[i].data.deviceRelease.jobNumber;
            cout << ", deviceNumber = " << instructions[i].data.deviceRelease.deviceNumber;
            cout << endl;
            break;
        case Display:
            cout << "Display: Time = " << instructions[i].time << endl;
            break;
        default:
            cout << "Unknown instruction!" << endl;
        }
    }
}

void proccessVec(vector<string> &input, System *system, vector<Instruction> &instructions)
{
    Instruction curr;
    switch (input[0][0])
    {
    case 'C':
        system->currTime = stoi(input[1]);
        system->memory = stoi(input[2].substr(2));
        system->devices = stoi(input[3].substr(2));
        system->quantum = stoi(input[4].substr(2));
        break;
    case 'A':
        curr.type = JA;
        curr.time = stoi(input[1]);
        curr.data.jobArrival.jobNumber = stoi(input[2].substr(2));
        curr.data.jobArrival.memoryRequirement = stoi(input[3].substr(2));
        curr.data.jobArrival.devicesRequirement = stoi(input[4].substr(2));
        curr.data.jobArrival.burstTime = stoi(input[5].substr(2));
        curr.data.jobArrival.priority = stoi(input[6].substr(2));
        instructions.push_back(curr);
        break;
    case 'Q':
        curr.type = DReq;
        curr.time = stoi(input[1]);
        curr.data.deviceRequest.jobNumber = stoi(input[2].substr(2));
        curr.data.deviceRequest.deviceNumber = stoi(input[3].substr(2));
        instructions.push_back(curr);
        break;
    case 'L':
        curr.type = DRel;
        curr.time = stoi(input[1]);
        curr.data.deviceRequest.jobNumber = stoi(input[2].substr(2));
        curr.data.deviceRequest.deviceNumber = stoi(input[3].substr(2));
        instructions.push_back(curr);
        break;
    case 'D':
        curr.type = Display;
        curr.time = stoi(input[1]);
        instructions.push_back(curr);
        break;
    default:
        // doing nothing
        cout << "Bad input" << endl;
    }
    input.clear();
    input.push_back("");
}

void readInput(string path, vector<Instruction> &instructions, System *system)
{
    ifstream inputFile(path);
    if (inputFile.is_open())
    {
        char c;
        vector<string> input;
        input.push_back("");

        int i = 0;
        while (inputFile.get(c))
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
        inputFile.close();
    }
    else
    {
        std::cerr << "Failed to open file" << std::endl;
    }
}