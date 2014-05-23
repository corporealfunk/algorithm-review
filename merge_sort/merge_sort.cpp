#include <string>
#include <iostream>
#include "../utilities/utilities.h"
using namespace std;

int* merge_halves(int leftHalf[], int leftLength, int rightHalf[], int rightLength) {
  int totalLength = leftLength + rightLength;
  int *sortedArray = new int[totalLength]; // dynamic allocation so we can pass pointer back to caller
  int a = 0; //index into sortedArray
  int l = 0; //index into left array
  int r = 0; //index into right array

  while (l < leftLength && r < rightLength) {
    if (leftHalf[l] < rightHalf[r]) {
      //cout << "left wins:" << leftHalf[l] << " < " << rightHalf[r] << endl;
      sortedArray[a] = leftHalf[l];
      l++;
    } else if (rightHalf[r] < leftHalf[l]) {
      //cout << "right wins:" << rightHalf[r] << " < " << leftHalf[l] << endl;
      sortedArray[a] = rightHalf[r];
      r++;
    } else {
      //cout << "tie:" << rightHalf[r] << " == " << leftHalf[l] << endl;
      sortedArray[a] = leftHalf[l];
      l++;
    }
    //cout << "  sorted[" << a << "] == " << sortedArray[a] << endl;
    //cout << "  a/l/r: " << a << "/" << l << "/" << r << "/" << endl;
    a++;
  }
  //cout << "AFTER LOOP a/l/r: " << a << "/" << l << "/" << r << "/" << endl;

  if (l == leftLength) {
    //copy remainder from right half
    //cout << "copy remainder from right:" << endl;
    if (r < rightLength) {
      //cout << r << "-" << rightLength << endl;
      for (r; r < rightLength; r++) {
        sortedArray[a] = rightHalf[r];
        //cout << "   sorted[" << a << "] == " << sortedArray[a] << endl;
        a++;
      }
    }
  }

  if (r == rightLength) {
    //copy remainder from left half
    //cout << "copy remainder from left:" << endl;
    if (l < leftLength) {
      //cout << l << "-" << leftLength << endl;
      for (l; l < leftLength; l++) {
        sortedArray[a] = leftHalf[l];
        //cout << "   sorted[" << a << "] == " << sortedArray[a] << endl;
        a++;
      }
    }
  }
  return sortedArray;
}

int* merge_sort(int array[], int arrayLength) {
  // 0: check for base case, arrayLength == 1? if so, return
  // 1: break the array in two halves
  // 2: call merge_sort on each half, they return sorted halves
  // 3: call merge_halves(leftArray, rightArray)

  // 0: check for base case, arrayLength == 1? if so, return
  if (arrayLength <= 1) {
    return array;
  }

  // 1: break the array in two halves, copy into halves
  int breakPoint = arrayLength / 2;
  int leftLength = breakPoint;
  int leftHalf[leftLength];
  int rightLength = arrayLength - breakPoint;
  int rightHalf[rightLength];

  int i = 0;
  for (i = 0; i < leftLength; i++) {
    leftHalf[i] = array[i];
  }

  int x = 0;
  for (i = breakPoint; i < arrayLength; i++) {
    rightHalf[x] = array[i];
    x++;
  }

  // 2: call merge_sort on each half, they return pointers to sorted array halves
  int *leftSorted;
  leftSorted = merge_sort(leftHalf, leftLength);

  int *rightSorted;
  rightSorted = merge_sort(rightHalf, rightLength);

  // 3: call merge_halves(leftArray, rightArray)
  int *sortedArray;
  sortedArray = merge_halves(leftSorted, leftLength, rightSorted, rightLength);

  return sortedArray;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "please supply file name to read as first argument" << endl;
    exit(1);
  }

  string fileName = argv[1];
  int *numbers;
  int numbersLength = 0;
  try {
    numbers = readFile(fileName, numbersLength);
  } catch (string param){
    cerr << "Exception: " << param << endl;
    exit(1);
  }

  int *sorted;
  sorted = merge_sort(numbers, numbersLength);

//  printArray(sorted, numbersLength, "SORTED");

  return 0;
}
