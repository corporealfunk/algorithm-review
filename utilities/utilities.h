#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
using namespace std;

int* readFile(string fName, int &linesArrayLength);
void printArray(int array[], int arrayLength, string label);
void printSubArray(int array[], int startindex, int endIndex, string label);

#endif
