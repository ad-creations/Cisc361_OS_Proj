#ifndef INPUT_H
#define INPUT_H
#include <fstream>
#include <iostream>
#include <vector>
#include <instruction.h>

using namespace std;

void print(System system, const vector<Instruction>& instructions);

void proccessVec(vector<string>& input, System* system, vector<Instruction>& instructions);

void readInput(string path, vector<Instruction>& instructions, System* system);

#endif