#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

int* readFile(string fName, int &linesArrayLength) {
  ifstream inf(fName.c_str());

  if (!inf.is_open()) {
    throw "Cannot open " + fName;
  }

  //count lines in file
  int counter = 0;
  string sInput;
  while (getline(inf, sInput)) {
    counter++;
  }
  int *linesArray = new int[counter];

  //rewind, and re-read into new array
  counter = 0;
  inf.clear();
  inf.seekg(0);
  int nInput = 0;
  while (getline(inf, sInput)) {
    stringstream convert(sInput);
    convert >> nInput;
    linesArray[counter] = nInput;
    counter++;
  }
  inf.close();
  linesArrayLength = counter;
  return linesArray;
}

void printArray(int array[], int arrayLength, string label) {
  using namespace std;

  cout << label << endl;
  for (int i = 0; i < arrayLength; i++) {
    cout << array[i] << endl;
  }
}
