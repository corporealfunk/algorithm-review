#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set> // debug
using namespace std;

void printDebug(set< int >& all, vector< int>& min, vector< int> max, int median, int element) {
  cout << "-----------------" << endl;
  int i = all.size();
  cout << "Iteration: " << i << endl;
  cout << "Element: " << element << endl;
  cout << "MIN HEAP: ";
  vector< int >::iterator itI;
  for(itI = min.begin(); itI != min.end(); itI++) {
    cout << *itI << " ";
  }

  cout << endl << "MAX HEAP: ";
  for(itI = max.begin(); itI != max.end(); itI++) {
    cout << *itI << " ";
  }

  cout << endl;

  cout << "SET: ";

  set< int >::iterator it;
  for(it = all.begin(); it != all.end(); it++) {
    cout << *it << " ";
  }
  cout << endl << "MEDIAN: " << median << endl;
}

int main() {
  ifstream dataFile("integers.txt");

  set< int > all; // debugging
  vector< int > min; // the smaller numbers, a max heap
  vector< int > max; // the bigger numbers, a min heap
  vector< int > medians;

  string line;
  int element;

  int i = 0;
  int medianTotal = 0;
  while (getline(dataFile, line)) {
    istringstream iss(line);
    iss >> element;

    all.insert(element);
    // if medians are empty, we're at the start
    if (medians.size() == 0) {
      // push onto min
      min.push_back(element);

      // it is the median right now
      medians.push_back(element);
      medianTotal += medians.back();
//      printDebug(all, min, max, medians.back(), element);
      continue;
    }

    // we have seen at least one element as we examine this one
    // we have to make some decisions:
    // is the element greater or less than the max element of the min heap?

    if (element <= min[0]) {
      min.push_back(element);
      push_heap(min.begin(), min.end());
    } else {
      max.push_back(element);
      push_heap(max.begin(), max.end(), greater<int>());
    }

    // We may have to rebalance the heaps, if the small heap
    // is more than 1 bigger than the max heap, pop from small, push onto max:
    // and visa-versa
    if (min.size() > max.size() + 1) {
      // pop from min:
      pop_heap(min.begin(), min.end());

      // push it onto max:
      max.push_back(min.back());
      push_heap(max.begin(), max.end(), greater<int>());

      // remove it from the back of the min container:
      min.pop_back();
    } else if (max.size() > min.size() + 1) {
      // pop from max:
      pop_heap(max.begin(), max.end(), greater<int>());

      // push it onto min:
      min.push_back(max.back());
      push_heap(min.begin(), min.end());

      // remove it from the back of the max container:
      max.pop_back();
    }

    // calculate min:
    // what iteration are we on:
    i = medians.size() + 1;

    if (i % 2 == 0) {
      // even, choose the smaller heap
      medians.push_back(min[0]);
    } else {
      // odd, choose the bigger heap
      if (min.size() >= max.size()) {
        medians.push_back(min[0]);
      } else {
        medians.push_back(max[0]);
      }
    }

    medianTotal += medians.back();
//    printDebug(all, min, max, medians.back(), element);
  }
  dataFile.close();

  cout << "MedianTotal: " << medianTotal << endl;
  cout << "MedianTotal % 10000: " << medianTotal % 10000 << endl;

  return 1;
}
