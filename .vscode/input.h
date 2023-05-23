#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "instruction.h"

using namespace std;

void print(System system, const vector<Instruction>& instructions);

void proccessVec(vector<string>& input, System* system, vector<Instruction>& instructions);

void readInput(char* path, vector<Instruction>& instructions, System* system);

#endif
