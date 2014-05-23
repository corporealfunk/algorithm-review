#include <string>
#include <iostream>
#include <algorithm>
#include "../utilities/utilities.h"
using namespace std;

/**
 * choose random number within inclusive range of start/end
 */
int choose_pivot(int partitionStartIndex, int partitionEndIndex) {
  int range = partitionEndIndex - partitionStartIndex + 1;
  return rand() % range + partitionStartIndex;
}

/**
 * partition chooses a pivot element in the start-to-end range and working
 * within that range, moves the pivot element to its correctly ordered
 * position while moving anything less than the pivot to the left in the
 * array and anything larger than the pivot to the right
 */
int partition(int array[], int partitionStartIndex, int partitionEndIndex) {
  int pivotIndex = choose_pivot(partitionStartIndex, partitionEndIndex);
  int i = partitionStartIndex + 1;

  // swap pivot to the front of the sub array
  swap(array[pivotIndex], array[partitionStartIndex]);

  // loop through range
  for (int j = partitionStartIndex + 1; j <= partitionEndIndex; j++) {
    // if element is less than the pivot, increment i
    // and swap the element with i
    if (array[j] < array[partitionStartIndex]) {
      swap(array[i], array[j]);
      i++;
    }
  }

  // swap the pivot into its correct place
  swap(array[partitionStartIndex], array[i - 1]);
  return i - 1;
}

void quick_sort(int array[], int sortStartIndex, int sortEndIndex) {
  // "base case": the range is 0 or negative in length
  if (sortStartIndex >= sortEndIndex) {
    return;
  }

  int partitionIndex = partition(array, sortStartIndex, sortEndIndex);

  // sort less-than-pivot numbers
  quick_sort(array, sortStartIndex, partitionIndex - 1);

  // sort bigger-than-pivot numbers
  quick_sort(array, partitionIndex + 1, sortEndIndex);
}

int main(int argc, char** argv) {
  srand(time(0));
  if (argc < 2) {
    cout << "please supply file name to read as first argument" << endl;
    exit(1);
  }

  string fileName = argv[1];
  int *numbers;
  int numbersLength = 0;
  try {
    numbers = readFile(fileName, numbersLength);
  } catch (string param) {
    cerr << "Exception: " << param << endl;
    exit(1);
  }

  quick_sort(numbers, 0, numbersLength - 1);

  printArray(numbers, numbersLength, "QUICK SORTED");

  return 0;
}
