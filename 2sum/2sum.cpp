#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

/*
 * For a target:
 * if found in the vector, return the index of the element.
 * if not found, return how many elements are less than this element + 1.
 * if target is less than element 0 return 0.
 * the vector must be sorted.
 */
int ith_element(const vector< long long int >& integers, int start, int end, long long int target) {
  int midpoint = (end + start) / 2;

  // is that it?
  if (integers[midpoint] == target) {
    return midpoint;
  }

  if (midpoint == start && target < integers[midpoint]) {
    return start;
  }

  if (midpoint == start && target > integers[midpoint]) {
    return start + 1;
  }

  if (midpoint == end && target > integers[midpoint]) {
    return end;
  }

  if (target > integers[midpoint]) {
    return ith_element(integers, midpoint + 1, integers.size() - 1, target);
  } else if (target < integers[midpoint]) {
    return ith_element(integers, start, midpoint - 1, target);
  }
}

int main() {
  ifstream dataFile("integers.txt");
  string line;
  long long int element;
  vector< long long int > integers;
  set< long long int > t_values;

  while (getline(dataFile, line)) {
    istringstream iss(line);
    iss >> element;

    integers.push_back(element);
  }

  // sort all integers
  sort(integers.begin(), integers.end());
  dataFile.close();

  vector< long long int >::iterator it;

  long long int inspect, target, high, low;
  int lowIndex, highIndex;

  // loop all integers
  // for all integers, find out how many unique targets in set [-10000; 10000] inclusive
  // exist such that any pair of integers x + y = t
  for (int x = 0; x < integers.size(); x++) {
    inspect = integers[x];

    high =   10000 - inspect;
    low  =  -10000 - inspect;
    lowIndex = ith_element(integers, 0, integers.size() - 1, low);
    highIndex = ith_element(integers, 0, integers.size() - 1, high);

    for (int y = lowIndex; y <= highIndex; y++) {
      if (y != x) {
        target = inspect + integers[y];
        if (target <= 10000 && target >= -10000) {
          t_values.insert(target);
        }
      }
    }
  }

  cout << "FOUND: " << t_values.size() << endl;

  set< long long int >::iterator itS;

  return 1;
}
