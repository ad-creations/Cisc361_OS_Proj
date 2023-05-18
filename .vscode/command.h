#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;     // start/arrival time
    char type;    // command type
    int jobId;    // "A" command
    int memory;   // "C" command "M"
    int devices;  // "C" command "S" and "Q"/"L" command "D"
    int quantum;  // "C" command "Q"
    int runTime;  // "A" command R
    int priority; // "A" command P
} Command;

const char COMMAND_TYPE_C = 'C';
const char COMMAND_TYPE_A = 'A';
const char COMMAND_TYPE_Q = 'Q';
const char COMMAND_TYPE_L = 'L';
const char COMMAND_TYPE_D = 'D';