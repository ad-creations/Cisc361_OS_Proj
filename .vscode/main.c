#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "job.h"
#include "system.h"
#include "command.h"

#define MAX_FILE_SIZE 500

int current_time = 0;
int available_memory = 256;
int used_memory = 0;
int num_processes = 0;
int used_devices = 0;

int getInternalEventTime(struct System* s, int quantum, int time_passed)
{
    // this function gets the time of the next internal event(i.e. running a job on the CPU). this time
    // is then compared to the next instruction read time. internal events are prioritized.
    if (s->readyQueue == NULL)
    {
        // if ready_queue mt, we need to read a new instruction so internal_event_time needs to be > next_instruction_time so we set internal_event_time to be arbitrary large number
        return 9999999;
    }
    else
    {
        if (s->readyQueue->head->leftTime + quantum <= s->readyQueue->head->burstTime)
        {
            return time_passed + quantum;
        }
        else
        {
            // process will not finish quantum and we need to return curr time +remaining burstTime
            return time_passed + (s->readyQueue->head->burstTime - s->readyQueue->head->leftTime);
        }
    }
}

int main() {
    struct System* system = (struct System*)malloc(sizeof(struct System));

    system->holdQueue1 = newQueue(1);
    system->holdQueue2 = newQueue(2);
    system->readyQueue = newQueue(1);
    system->waitQueue = newQueue(2);
    system->leaveQueue = newQueue(2);

    char* file = (char*)malloc(sizeof(char) * MAX_FILE_SIZE);
    FILE* ptr = fopen("i0.txt", "r");

    if (ptr == NULL) {
        printf("File cannot be opened");
        exit(1);
    }

    while (fgets(file, MAX_FILE_SIZE, ptr) != NULL) {
        struct Command* command = parseCommand(file);

        switch (command->type) {
            case 'C': {
                printf("-----Configuring----\n");
                system->time = command->time;
                printf("Time: %d\n", system->time);

                system->totalMemory = command->memory;
                printf("Memory: %d\n", system->totalMemory);

                system->curMemory = system->totalMemory;
                printf("Curr Memory: %d\n", system->curMemory);

                system->totalDevice = command->devices;
                printf("Total Device: %d\n", system->totalDevice);

                system->curDevice = system->totalDevice;
                printf("Curr Device: %d\n", system->curDevice);

                system->timeQuantum = command->quantum;
                printf("Quantum: %d\n", system->timeQuantum);

                printf("Systems: total memory: %d\n", system->totalMemory);
                printf("Systems: quantum: %d\n", system->timeQuantum);
                printf("Systems: time: %d\n", system->time);

                free(command);
                break;
            }
            case 'A': {
                struct Job* job = newJob(command);
                if (job->needMemory > system->totalMemory || job->needDevice > system->totalDevice) {
                    printf("job is rejected, resource is not enough\n");
                }
                else if (system->curMemory >= job->needMemory) {
                    printf("adding job to ready queue\n");
                    pushQueue(system->readyQueue, job);
                    printf("Added Job %d\n", job->jobId);

                    used_memory += job->needMemory;
                    available_memory = system->totalMemory - used_memory;

                    printf("Used memory: %d\n", used_memory);
                }
                else {
                    if (job->priority == 1) {
                        printf("Adding %d to Hold Queue 1\n", job->jobId);
                        pushQueue(system->holdQueue1, job);
                        printf("Done Adding SJF!\n");
                    }
                    else {
                        printf("Adding %d to Hold Queue 2\n", job->jobId);
                        printf("Memory needed: %d, Available: %d,\n", job->needMemory, system->totalMemory - used_memory);
                        pushQueue(system->holdQueue2, job);
                        printf("Done adding FIFO!\n");
                    }
                    free(command);
                    break;
                }

                printAtTime(system, available_memory, used_devices);

                free(command);
                break;
            }
            case 'Q': {
                struct Job* job = newJob(command);
                if (job->jobId == system->running->jobId && (system->totalDevice + system->curDevice) <= system->running->needDevice) {
                    requestDevice(system, command, num_processes);
                    used_devices += job->needDevice;
                }
                free(command);
                break;
            }
            case 'L': {
                struct Job* job = newJob(command);
                if (job->jobId == system->running->jobId && (system->totalDevice + system->curDevice) <= system->running->holdDevice) {
                    releaseDevice(system, command);
                    used_devices -= job->needDevice;
                }
                free(command);
                break;
            }
            case 'D': {
                printAtTime(system, available_memory, used_devices);
                free(command);
                break;
            }
            default:
                printf("broken line");
                break;
        }
    }

    printf("outside while loop");
    fclose(ptr);
    free(system->holdQueue1);
    free(system->holdQueue2);
    free(system->readyQueue);
    free(system->waitQueue);
    free(system->leaveQueue);
    free(system);
    free(file);
    return 0;
}
