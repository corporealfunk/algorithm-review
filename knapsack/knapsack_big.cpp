#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <tr1/unordered_map>
#include <vector>
#include <map>
using namespace std;

struct Item {
  int value;
  int weight;
};

struct Node {
  int i,
      x,
      value;
};

bool compareNodes(Node* a, Node* b) {
  if (a->i < b->i) {
    return true;
  } else if (a->i > b->i) {
    return false;
  } else if (a->i == b->i) {
    if (a->x < b->x) {
      return true;
    } else if (a->x > b->x) {
      return false;
    } else if (a->x == b->x) {
      return false;
    }
  }
}

string returnKey(int i, int x) {
  stringstream ii, xx;

  xx << i;
  string iStr = xx.str();

  ii << x;
  string xStr = ii.str();
  return iStr + "," + xStr;
}

void computeNode(int i, int x, tr1::unordered_map<string, Node* >& toCompute, Item** items) {
  // base case: weight is < 1
  // base case: i is < 1
  if (x < 1 || i < 0) {
    return;
  }

  // base case: we're already ready to compute
  if (toCompute.count(returnKey(i, x)) == 1) {
    return;
  }

//  cout << "computing: " << i << "," << x << endl;
  Node* node;
  node = new Node();
  node->i = i;
  node->x = x;
  node->value = 0;
  toCompute[returnKey(i, x)] = node;

  // to get the answer of this node's value, we need to get the answer of the two
  // items we could compare it to: [i - 1][x] and [i - 1][x - weight] + item value
  Item* item = items[i];

  computeNode(i - 1, x, toCompute, items);
  computeNode(i - 1, x - item->weight, toCompute, items);

  return;
}

int main() {
  ifstream dataFile("items2.txt");
  string line;
  int maxWeight, numItems;

  getline(dataFile, line);
  istringstream iss(line);
  iss >> maxWeight >> numItems;

  cout << "NumItems: " << numItems << ". Sack: " << maxWeight << endl;

  Item** items = 0;;
  items = new Item*[numItems];

  int value, weight;
  int i = 0;
  while (getline(dataFile, line))
  {
    istringstream iss(line);
    iss >> value >> weight;
    items[i] = new Item();
    items[i]->value = value;
    items[i]->weight = weight;
    i++;
  }
  dataFile.close();

  // recursivley compute from upper right down
  tr1::unordered_map< string, Node* > toCompute;
  computeNode(numItems - 1, maxWeight, toCompute, items);
  cout << "Must Compute " << toCompute.size() << " nodes in the matrix." << endl;

  // sort the nodes into a vector first:
  tr1::unordered_map< string, Node* >::iterator it;

  vector< Node* > nodesSorted;
  nodesSorted.resize(toCompute.size());
  i = 0;
  for (it = toCompute.begin(); it != toCompute.end(); it++) {
    nodesSorted[i] = it->second;
    i++;
  }

  sort(nodesSorted.begin(), nodesSorted.begin() + toCompute.size(), compareNodes);

  Node* node;
  Item* item;
  int valueIm1 = 0;
  int valueIm1XmW = 0;
  int x;

  // loop sorted nodes:
  vector< Node* >::iterator itSorted;
  for (itSorted = nodesSorted.begin(); itSorted != nodesSorted.end(); itSorted++) {
    node = *itSorted;
    item = items[node->i];

    i = node->i;
    x = node->x;

    // lookup [i-1][x] value:
    if (toCompute.count(returnKey(i - 1, x)) == 1) {
      valueIm1 = toCompute[returnKey(i - 1, x)]->value;
    } else {
      valueIm1 = 0;
    }

    if (item->weight > node->x) {
      node->value = valueIm1;
    } else {
      // lookup [i-1][x], compare to [i-1][x-item->weight] + item->value
      if (toCompute.count(returnKey(i - 1, x - item->weight)) == 1) {
        valueIm1XmW = toCompute[returnKey(node->i - 1, node->x - item->weight)]->value;
      } else {
        valueIm1XmW = 0;
      }
      node->value = max(valueIm1, valueIm1XmW + item->value);
    }
//    cout << "Computing: " << node->i << "," << node->x << endl;
//    cout << "     value: " << node->value << endl;
  }

  cout << "Max Value: " << toCompute[returnKey(numItems - 1, maxWeight)]->value << endl;
  return 1;
}
