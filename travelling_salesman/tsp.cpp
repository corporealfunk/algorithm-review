#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <bitset>
#include <limits>
using namespace std;

struct Node {
  float x;
  float y;
  int label;
};

float distance(Node* node1, Node* node2) {
  float x = node1->x - node2->x;
  float y = node1->y - node2->y;
  float dist;

  dist = pow(x, 2) + pow(y, 2);
  dist = sqrt(dist);

  return dist;
}

void printMatrix(float** matrix, int s, int n) {
  for (int x = 0; x < s; x++) {
    for (int y = 0; y < n; y++) {
      cout << matrix[x][y] << "\t";
    }
    cout << endl;
  }
}

int main() {
  ifstream dataFile("input.txt");
  string line;
  int numNodes;

  getline(dataFile, line);
  istringstream iss(line);
  iss >> numNodes;

  float x, y;
  vector< Node* > nodes;
  int i = 0;

  nodes.resize(numNodes);
  while (getline(dataFile, line))
  {
    sscanf(line.c_str(), "%f %f", &x, &y);

    nodes[i] = new Node();
    nodes[i]->x = x;
    nodes[i]->y = y;
    nodes[i]->label = i;
    cout << x << "/" << y << endl;
    i++;
  }
  dataFile.close();

  cout << "Num Nodes: " << numNodes << endl;

  // setup the sub-sets:
  vector< bitset< 25 >* > subSets; // CHANGE
  map< int, vector< int > > subSetsBySize;
  map< int, int> subSetsByHash;

  bitset< 25 >* subSet; // CHANGE
  bitset< 25 > subSetJ; // CHANGE
  for (int j = 1; j < pow(2, numNodes); j++) {
    subSet = new bitset< 25 >; // CHANGE
    *subSet = j;
    if ((*subSet)[0] == 1) {
      subSets.push_back(subSet);
      subSetsByHash[j] = subSets.size() - 1;
      subSetsBySize[subSet->count()].push_back(subSets.size() - 1);
      cout << *subSet << endl;
    }
  }


  // init a 2D array subsets size in size, fill with 0:
  float** matrix = 0;
  matrix = new float*[subSets.size()];
  for (int s = 0; s < subSets.size(); s++) {
    matrix[s] = new float[nodes.size()];
    for (int j = 0; j < nodes.size(); j++) {
      // fill with base cases:
      if (s == 0 && j == 0) {
        matrix[s][j] = 0.0;
      } else {
        matrix[s][j] = std::numeric_limits<int>::max();
      }
    }
  }

  for (int m = 2; m <= numNodes; m++) {
    Node* nodeK = 0;
    Node* nodeJ = 0;
    cout << "----------------: SIZE : " << m << endl;

    // loop all subsets of size m the contain the start node 0
    for (int s = 0; s < subSetsBySize[m].size(); s++) {
      subSet = subSets[subSetsBySize[m][s]];
      for (int j = 1; j < subSet->size(); j++) {
        nodeJ = nodes[j];
        // if subset has j in it, examine it:
        if ((*subSet)[j] == 1) {
          // loop for every node (k) in the subset
          // k cannot be j
          subSetJ = *subSet; // copy
          subSetJ[j] = 0;

          float tryCost = 0.0;
          for (int k = 0; k < subSet->size(); k++) {
            nodeK = nodes[k];
            // if k != j and it's in the set, examine it
            if (k != j && (*subSet)[k] == 1) {
              // find the index of subSet - {j}
              if (matrix[subSetsByHash[subSetJ.to_ulong()]][k] != std::numeric_limits<int>::max()) {
                tryCost = matrix[subSetsByHash[subSetJ.to_ulong()]][k] + distance(nodeJ, nodeK);
                matrix[subSetsByHash[subSet->to_ulong()]][j] = min(tryCost, matrix[subSetsByHash[subSet->to_ulong()]][j]);
              }
            }
          }
        }
      }
    }
  }

  //printMatrix(matrix, subSets.size(), numNodes);

  // we just have to calc final hop:
  float minCost = std::numeric_limits<int>::max();
  float tryCost = 0;
  int lastS = subSets.size() - 1;
  Node* startNode = nodes[0];
  Node* nodeJ = 0;

  for (int j = 1; j < numNodes; j++) {
    nodeJ = nodes[j];
    tryCost = matrix[lastS][j] + distance(nodeJ, startNode);
    minCost = min(tryCost, minCost);
  }

  cout << "MIN COST TOUR: " << minCost << endl;

  return 1;
}
