#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tr1/unordered_map>
#include <sys/resource.h>
using namespace std;

struct Node {
  int label;
  int discovered;
  vector< bool >* bitVector;
};

void printNode(Node& node) {
  vector< bool >::iterator bIt;

  for (bIt = node.bitVector->begin(); bIt != node.bitVector->end(); bIt++) {
    cout << *bIt;
  }

  cout << " = " << node.label << endl;
}

vector< int >& getBitMasks(int maskSize) {
  vector< int >* masks;

  masks = new vector< int >;

  // masks for 1 bit away
  bool bit = 0;
  for (int x = 0; x < maskSize; x++) {
    int mask = 0;

    for (int y = 0; y < maskSize; y++) {
      if (y != x) {
        bit = 0;
      } else {
        bit = 1;
      }
      mask = mask * 2 + (int)bit;
    }
    masks->push_back(mask);
  }

  // mask for 2 bits away
  int flipX = 0;
  int flipY = 0;
  for (int x = 0; x < maskSize; x++) {
    flipX = x;

    for (int y = x + 1; y < maskSize; y++) {
      int mask = 0;

      flipY = y;
      for (int j = 0; j < maskSize; j++) {
        if (flipY == j || flipX == j) {
          bit = 1;
        } else {
          bit = 0;
        }
        mask = mask * 2 + (int)bit;
      }
      masks->push_back(mask);
    }
  }

  return *masks;
}

void dfsNode(Node& node, tr1::unordered_map< long int, Node* >& nodes, const vector< int >& masks) {
//  cout << "----------" << endl;
//  printNode(node);

  node.discovered = true;

  // we need to create bit flips to get us 1 and 2 away
  int nodeToFind;

  for (int i = 0; i < masks.size(); i++) {
    // for each mask, we need to XOR (^) the mask and label
    nodeToFind = node.label ^ masks[i];

    if (nodes.count(nodeToFind) == 1 && !nodes[nodeToFind]->discovered) {
      dfsNode(*nodes[nodeToFind], nodes, masks);
    }
  }
}

int main() {
  // increase stack size:
  const rlim_t kStackSize = 16 * 1024 * 1024;
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

  cout << "Reading File..." << endl;
  while (getline(dataFile, line)) {
    istringstream iss(line);
    node = new Node();
    node->discovered = false;
    node->bitVector = new vector< bool >;
    node->bitVector->resize(numBits);
    node->label = 0;
    int i = 0;

    while (iss >> sBit) {
      if (sBit == "1") {
        (*node->bitVector)[i] = true;
      } else if (sBit == "0") {
        (*node->bitVector)[i] = false;
      }
      node->label = node->label * 2 + (int)(*node->bitVector)[i];
      i++;
    }

    // add to heap:
    nodes[node->label] = node;
  }
  dataFile.close();
  cout << "...done reading " << nodes.size() << " unique nodes" << endl;

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

  vector< int > masks;
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
