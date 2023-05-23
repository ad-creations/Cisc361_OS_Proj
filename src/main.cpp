/** @file main.cpp
 * Main driver file for the Operating systems project.
 */
// The previous block is needed in every file for which you want to generate documentation

#include <vector>
#include <queue>
#include <instruction.h>
#include <input.h>
#include <utils.h>

using namespace std;

int main(int argc, char *argv[])
{
    vector<Instruction> instructions;
    System system;
    /* Choose an input file: */
    // readInput("../input/test1.txt", instructions, &system);
    // readInput("../input/i0.txt", instructions, &system);
    // readInput("../input/i1.txt", instructions, &system);
    readInput("../input/i2.txt", instructions, &system);
    // print(system, instructions);
    // return 0;
    // #####################################################
    int totalMemory = system.memory;
    int totalDevices = system.devices;

    queue<Job> readyQueue;
    queue<Job> waitQueue;
    vector<pair<Job, int>> doneArr;
    priority_queue<struct Job, vector<struct Job>, cmpQ1> holdQueue1;
    priority_queue<struct Job, vector<struct Job>, cmpQ2> holdQueue2;

    Job *CPU = nullptr;

    // Main driver.
    int timeOfNextInput;
    int timeOfNextInternalEvent = system.quantum;
    long unsigned int instructionIdx = 0;
    long currQuantum = system.quantum;
    while (true)
    {
        debug("\n\nTop of loop with currTime = " + std::to_string(system.currTime));
        // If there is nothing on the CPU and there are jobs on the ready queue, then place on the CPU.
        if (CPU == nullptr && !readyQueue.empty())
        {
            CPU = &readyQueue.front();
            currQuantum = system.quantum;
            debug("Adding " + std::to_string(CPU->jobNumber) + " to the CPU");
            readyQueue.pop();
        }

        bool instructionIsInternal = false;
        // The time of next input should be infinite if there are none left,
        // otherwise it is the time of next instruction - current time.
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
            debug("There are no next inputs, nextInput is INT16_MAX");
            timeOfNextInput = INT16_MAX - 1;
        }

        // The time of next internal event will normally be the time quantum
        // unless the quantum will finish the burst time of the process, then it is the remaining time.
        if (CPU != nullptr && CPU->burstTime < currQuantum)
        {

            timeOfNextInternalEvent = CPU->burstTime;
            debug("Internal event is set to the CPU's burst time");
        }
        else if (CPU == nullptr) // If the CPU has nothing on it at this point, then the READY QUEUE must be empty, there is nothing to work on
        {                        // and there should be no internal events.
            timeOfNextInternalEvent = INT16_MAX;
            debug("Nothing on the CPU, internal event set to INT_MAX");
        }
        else
        {
            timeOfNextInternalEvent = currQuantum;
            debug("Internal event is set to the current quantum of " + std::to_string(currQuantum));
        }

        debug("Time till nextInput = " + std::to_string(timeOfNextInput));
        debug("Time till nextInternal = " + std::to_string(timeOfNextInternalEvent));
        if (((timeOfNextInput == timeOfNextInternalEvent) && instructionIsInternal))
        {
            debug("time of next input == time of next internal event and instruction is internal");
        }
        // if ((timeOfNextInput < timeOfNextInternalEvent) || ((timeOfNextInput == timeOfNextInternalEvent) && instructionIsInternal))
        if ((timeOfNextInput < timeOfNextInternalEvent))
        {
            // We will handle the instruction first
            // Jump the current time to the next input.
            system.currTime += timeOfNextInput;
            currQuantum -= timeOfNextInput;
            debug("Jumping to the next instruction, currTime = " + std::to_string(system.currTime));
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
                    debug("handling job arrival, job number " + std::to_string(tmp.jobNumber));
                    handleJobArrival(tmp, holdQueue1, holdQueue2, readyQueue, &system);
                }
                break;
            case DRel:
                if (instructions[instructionIdx].data.deviceRelease.jobNumber == CPU->jobNumber &&
                    (instructions[instructionIdx].data.deviceRelease.deviceNumber) <= CPU->devicesHeld)
                {
                    debug("handling device release on job number " + to_string(instructions[instructionIdx].data.deviceRelease.jobNumber));
                    handleDeviceRelease(instructions[instructionIdx].data.deviceRelease, waitQueue, readyQueue, CPU, &system);
                    // Reset the time quantum
                    // Job on CPU should have been moved to the ready queue and CPU is now nullptr.
                    currQuantum = system.quantum;
                }
                break;
            case DReq:
                if (instructions[instructionIdx].data.deviceRequest.jobNumber == CPU->jobNumber &&
                    (instructions[instructionIdx].data.deviceRequest.deviceNumber + CPU->devicesHeld) <= CPU->devicesRequirement)
                {
                    debug("handling device request on job number " + to_string(instructions[instructionIdx].data.deviceRequest.jobNumber));
                    handleDeviceRequest(instructions[instructionIdx].data.deviceRequest, waitQueue, readyQueue, CPU, &system);
                    // Reset the time quantum
                    // Job on CPU should have been moved to either the ready queue or wait queue and CPU should now be nullptr.
                    currQuantum = system.quantum;
                }
                break;
            case Display:
                handleDisplay(waitQueue, holdQueue1, holdQueue2, readyQueue, CPU, &system, doneArr);
                break;
            default:
                cout << "Erroneous type value of event!" << endl;
            }
            // Update the instructionIdx, we are now looking forward to the next instruction.
            instructionIdx += 1;
        }
        else
        {
            // We are handling the internal event first (this includes the case that they are at the same time).
            // Jump forward to the end of the quantum.
            system.currTime += timeOfNextInternalEvent;
            debug("Jumping to the end of the quantum, currTime = " + to_string(system.currTime));
            if (CPU != nullptr)
            {
                CPU->burstTime -= timeOfNextInternalEvent;
            }
            // The job on the CPU has been completed. Terminate and add something new to
            if (CPU != nullptr && CPU->burstTime == 0)
            {
                debug("Job on CPU is terminating, jobNumber = " + to_string(CPU->jobNumber));
                handleProcessTermination(waitQueue, holdQueue1, holdQueue2, readyQueue, CPU, &system, doneArr);
                // At this point, the wait queue and both hold queues have been proccessed.
                // And the job on the CPU should have been moved to the doneArr and
                // CPU should be nullptr.
            }
            else
            { // Job on the CPU has been worked on, but is being switched off due to receiving is quantum.
                debug("Job on CPU has been worked on, moving now to the back of the readyQueue, jobNumber = " + to_string(CPU->jobNumber));
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