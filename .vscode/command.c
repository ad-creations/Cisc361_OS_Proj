#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "command.h"

int parseParameter(char* line, char* type, int* num, int start) {
    int i = start;

    while (isspace(line[i])) {
        i++;
    }

    if (line[i] == '\0' || line[i] == '\n') {
        return -1;
    }

    *type = line[i];
    i += 2;

    *num = 0;
    while (!isspace(line[i]) && line[i] != '\0' && line[i] != '\n') {
        *num = (*num * 10) + (line[i] - '0');
        i++;
    }

    return i;
}

struct Command* parseCommand(char* line) {
    struct Command* cmd = (struct Command*)malloc(sizeof(struct Command));

    if (cmd == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }

    cmd->type = line[0];
    cmd->time = atoi(line + 2);
    int i = 3;

    char param_type;
    int param_value;
    while (line[i] != '\n' && line[i] != '\0') {
        int result = parseParameter(line, &param_type, &param_value, i);
        if (result == -1) {
            break;
        }

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

        i = result + 1; // Move to the next character after parsing a parameter
    }

    return cmd;
}
