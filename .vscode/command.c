#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

int parseParameter(char* line, char* type, int* num, int start) {
    int i = start;

    // Skip whitespace in file
    while (line[i] == ' ') {
        i++;
    }

    // Check for end of string or newline
    if (line[i] == '\0' || line[i] == '\n') {
        return -1;
    }

    // Set type to current character
    *type = line[i];

    i += 2;

    // Store parsed integer value
    *num = atoi(line + i);

    // Increment index while not encountering whitespace, end of string, or newline
    while (line[i] != ' ' && line[i] != '\0' && line[i] != '\n') {
        i++;
    }
    return i;
}

Command* parseCommand(char* line) {
    // Allocate memory for new command struct
    Command* cmd = (Command*) malloc(sizeof(Command));

    if (cmd == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }

    // Set type to the first character in the string
    cmd->type = line[0];

    // Parse through line, saving the time it takes to move through
    cmd->time = atoi(line + 2);

    // Start at whitespace after '#' assigned to the first command
    int i = 3;

    char param_type;
    int param_value;
    while (i != -1 && line[i] != '\n' && line[i] != '\0') {
        // Start at the 3rd position in the string to look for additional params
        i = parseParameter(line, &param_type, &param_value, i);
        switch (param_type) {
            case 'M':
                cmd->memory = param_value;
                break;
            case 'S':
                cmd->devices = param_value;
                break;
            case 'Q':
                cmd->quantum = param_value;
                break;
            case 'J':
                cmd->jobId = param_value;
                break;
            case 'D':
                cmd->devices = param_value;
                break;
            case 'P':
                cmd->priority = param_value;
                break;
            case 'R':
                cmd->runTime = param_value;
                break;
            default:
                fprintf(stderr, "Error: invalid parameter type %c\n", param_type);
                exit(1);
        }
    }

    return cmd;
}
