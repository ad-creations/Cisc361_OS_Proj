#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Job.h"
#include "System.h"


#include "command.h"

//#define MAX_LINE_LENGTH 100

//Global Variables

int MAX_FILE_SIZE = 500;

//int time_slice = 4;
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

 int main(){
	struct System* system = (struct System*)malloc(sizeof(struct System));

	system->holdQueue1 = (struct Queue*)malloc(sizeof(struct Queue));
	system->holdQueue2 = (struct Queue*)malloc(sizeof(struct Queue));
	system->readyQueue = (struct Queue*)malloc(sizeof(struct Queue));
	system->waitQueue = (struct Queue*)malloc(sizeof(struct Queue));
	system->leaveQueue = (struct Queue*)malloc(sizeof(struct Queue));

	//reading input files function
	char *file = (char*)malloc(sizeof(char) * MAX_FILE_SIZE);
	
	//char file[MAX_FILE_SIZE];

	//opening file
	FILE* ptr = fopen("i0.txt", "r");


	if(ptr == NULL){
		printf("File cannot be opened");
		exit(1);
	}

	//reading through each line
	while (fgets(file, sizeof(file), ptr) != NULL){
		//call parsing functions and then push to queue
		struct Command* command = parseCommand(file);
		system = newSystem(command);

		switch (command->type){
			//System Config
			case 'C': {
				command = parseCommand(file);

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
				printf("Systems: time : %d\n", system->time);




				printf("Made system\n");
				

				//break;
			}
			//Job arrival
			case 'A': {
				command = parseCommand(file);

				//Debugging print statement
				//printf("Command %c\n", command->type);

				struct Job* job = newJob(command);
				//Checking if job needs more memory or devices that are available
				if(job->needMemory > system->totalMemory || job->needDevice > system->totalDevice){
					printf("job is rejected, resource is not enough\n");
				}
				//if there is enough available memory
				if(system->curMemory >= job->needMemory){
					printf("adding job to ready queue\n");

					//push job to readyqueue
					pushQueue(system->readyQueue, job);
					printf("Added Job %d\n", job->jobId);

					//Adding to used memory based on how much job needed
					used_memory = used_memory + job->needMemory;

					//taking away memory needed by job from available memory
					available_memory = system->totalMemory - job->needMemory;

					printf("Used memory: %d\n", used_memory);
				}
				else{
					if(job->priority == 1){
						//push job into hold queue 1 SJF 
						printf("Adding %d to Hold Queue 1\n", job->jobId);
						pushQueue(system->holdQueue1, job);

						printf("Done Adding SJF!\n");
					}
					else{
						//push job into hold queue 2 FIFO 
						printf("Adding %d to Hold Queue 2\n", job->jobId);
						printf("Memory needed: %d, Available: %d,\n", job->needMemory, system->totalMemory - used_memory);
						pushQueue(system->holdQueue2, job);

						printf("Done adding FIFO!\n");
					}
				}

				//To show that function works when uncommented despite the fact is should not be called here
				printAtTime(system, available_memory, system->totalDevice);

				//break;
			}
			//Request for Jobs
			case 'Q': {
				command = parseCommand(file);


				struct Job* job = newJob(command);
				//compare number of devices & number of devices to push that job into ready queue; else if # of devices = to need, running job pushed into waiting queue
				if(job->jobId == system->running->jobId && (system->totalDevice + system->curDevice) <= system->running->needDevice){
					//Going to request device based on bankers
					requestDevice(system, command, num_processes);

					//Adding to used devices based on how many devices job needs
					used_devices += job->needDevice;

				}

				//break;

			}
			//Release job
			case 'L': {
				command = parseCommand(file);

				struct Job* job = newJob(command);
				if (job->jobId == system->running->jobId && (system->totalDevice + system->curDevice) <= system->running->holdDevice){
					//Going to release devices
					releaseDevice(system, command);

					//Taking back devices job needed from amount used
					used_devices -= job->needDevice;
				}

				//break;
			}

			case 'D': {
				printAtTime(system, available_memory, used_devices);				
				//break;

			}

			default : 
			printf("broken line");
			return 0;

		 }
		free(command);

	}
	//printAtTime(system,system->curDevice, used_memory, system->time,current_time, system->totalMemory, system->totalDevice );

	printf("outside while loop");
	fclose(ptr);
	free(system->holdQueue1);
    free(system->holdQueue2);
    free(system->readyQueue);
    free(system->waitQueue);
    free(system->leaveQueue);
    free(system);
	return 0;
}