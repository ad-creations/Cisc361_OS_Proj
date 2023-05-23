#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Command {
    char type;      // Command type
    int time;       // Start/arrival time
    int jobId;      // "A" command
    int memory;     // "C" command "M"
    int devices;    // "C" command "S" and "Q"/"L" command "D" (request) //this is partially where the command input is getting a parameter type 0 error, no avaliable fix found
    int quantum;    // "C" command "Q"
    int runTime;    // "A" command R
    int priority;   // "A" command P
};

int parseParameter(char* line, char* type, int* num, int start);
struct Command* parseCommand(char* line);

#endif  /* COMMAND_H */
