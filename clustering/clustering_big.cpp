#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tr1/unordered_map>
#include <algorithm>
#include <sys/resource.h>
using namespace std;

struct Node {
  int label;
  int discovered;
  vector< bool >* bitVector;
};

int decimalFromBoolVector(vector< bool >& bitVector) {
  vector< bool >::iterator bIt;

  int decimal = 0;
  for (bIt = bitVector.begin(); bIt != bitVector.end(); bIt++) {
    decimal = decimal * 2 + (int)*bIt;
  }

  return decimal;
}

void printNode(Node& node) {
  vector< bool >::iterator bIt;

  for (bIt = node.bitVector->begin(); bIt != node.bitVector->end(); bIt++) {
    cout << *bIt;
  }

  cout << " = " << node.label << endl;
}

vector< vector< bool > >& getBitMasks(int maskSize) {
  vector< vector< bool > >* masks;

  masks = new vector< vector< bool > >;

  // masks for 1 bit away
  for (int x = 0; x < maskSize; x++) {
    vector < bool > mask;
    mask.resize(maskSize);

    for (int y = 0; y < maskSize; y++) {
      if (y != x) {
        mask[y] = false;
      } else {
        mask[y] = true;
      }
    }
    masks->push_back(mask);
  }

  // mask for 2 bits away
  int flipX = 0;
  int flipY = 0;
  for (int x = 0; x < maskSize; x++) {
    flipX = x;

    for (int y = x + 1; y < maskSize; y++) {
      vector < bool > mask;
      mask.resize(maskSize);

      flipY = y;
      for (int j = 0; j < maskSize; j++) {
        if (flipY == j || flipX == j) {
          mask[j] = true;
        } else {
          mask[j] = false;
        }
      }
      masks->push_back(mask);
    }
  }

  return *masks;
}

void dfsNode(Node& node, tr1::unordered_map< long int, Node* >& nodes, vector< vector< bool > >& masks) {
  if (node.discovered) {
    return;
  }

//  cout << "----------" << endl;

  node.discovered = true;

  // we need to create bit flips to get us 1 and 2 away
//  printNode(node);

  int nodeToFind;
  vector< vector< bool > >::iterator masksIt;

  for (masksIt = masks.begin(); masksIt != masks.end(); masksIt++) {
    vector< bool > xorD;
    xorD.resize(node.bitVector->size());

    for (int x = 0; x < masksIt->size(); x++) {
      if ((*masksIt)[x] == (*node.bitVector)[x]) {
        xorD[x] = false;
      } else {
        xorD[x] = true;
      }
    }
    nodeToFind = decimalFromBoolVector(xorD);

    if (nodes.count(nodeToFind) == 1) {
      dfsNode(*nodes[nodeToFind], nodes, masks);
    }
  }
}

int main() {
  // increase stack size:
  const rlim_t kStackSize = 63 * 1024 * 1024;
  struct rlimit rl;
  int result;

  result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0)
  {
    if (rl.rlim_cur < kStackSize)
    {
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);
      if (result != 0)
      {
        fprintf(stderr, "setrlimit returned result = %d\n", result);
        return 0;
      }
    }
  }

  ifstream dataFile("clustering_big.txt");
  string line;
  int numNodes, numBits;

  getline(dataFile, line);
  istringstream iss(line);
  iss >> numNodes >> numBits;

  tr1::unordered_map< long int, Node* > nodes;

  string sBit;
  int i;

  Node* node;

  while (getline(dataFile, line)) {
    istringstream iss(line);
    node = new Node();
    node->discovered = false;
    node->bitVector = new vector< bool >;
    node->bitVector->resize(numBits);
    int i = 0;

    while (iss >> sBit) {
      if (sBit == "1") {
        (*node->bitVector)[i] = true;
      } else if (sBit == "0") {
        (*node->bitVector)[i] = false;
      }
      i++;
    }

    // convert the vector<bool> to decimal
    node->label = decimalFromBoolVector(*(node->bitVector));

    // add to heap:
    nodes[node->label] = node;
  }
  dataFile.close();

  // we need to find the number of connected components where all
  // members of a given component have edges with distance < 3

  // OUTER LOOP: look at each node
  // - has it been discovered? if so, move on
  // - if it has not been discovered, we need to find all
  //   node keys that have distance +/- 1, 2 from this node
  // - look up those node keys in the map, for each of those nodes
  //   add it to this cluster. then we need to search all those nodes
  //   in the same manner (DFS or BFS)

  tr1::unordered_map< long int, Node* >::iterator nodesIt;
  int clusters = 0;

  vector< vector< bool > > masks;
  masks = getBitMasks(node->bitVector->size());
  for (nodesIt = nodes.begin(); nodesIt != nodes.end(); nodesIt++) {
    node = nodesIt->second;
    if (node->discovered == false) {
      dfsNode(*node, nodes, masks);
      clusters++;
    }
  }

  cout << "# of clusters: " << clusters << endl;

  return 1;
}
