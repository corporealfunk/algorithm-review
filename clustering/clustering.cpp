#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;


struct Node {
  int label;
  Node* leader;
};

struct Edge {
  int x;
  int y;
  int cost;
};

bool edgeCompare(const Edge* edgeL, const Edge* edgeR) {
  if (edgeL->cost == edgeR->cost) {
    return false;
  } else {
    return (edgeL->cost < edgeR->cost);
  }
}

int main() {
  ifstream dataFile("clustering1.txt");
  string line;
  int numNodes;

  getline(dataFile, line);
  istringstream iss(line);
  iss >> numNodes;

  map < int, Node* > nodes;
  vector < Edge* > edges;

  int x, y, cost;
  int numEdges = 0;
  while (getline(dataFile, line)) {
    istringstream iss(line);
    iss >> x >> y >> cost;

    edges.push_back(new Edge());
    edges.back()->x = x;
    edges.back()->y = y;
    edges.back()->cost = cost;

    if (nodes.count(x) == 0) {
      nodes[x] = new Node();
      nodes[x]->label = x;
      nodes[x]->leader = nodes[x];
    }

    if (nodes.count(y) == 0) {
      nodes[y] = new Node();
      nodes[y]->label = y;
      nodes[y]->leader = nodes[y];
    }
    numEdges++;
  }
  dataFile.close();

  sort(edges.begin(), edges.begin() + edges.size(), edgeCompare);

  int k = 4;

  int i = numNodes;
  int curCheapest = 0;
  Edge cheapestEdge;
  Node nodeX, nodeY;
  map< int, Node* >::iterator nodeIt;

  while (i > k) {
    // pick cheapest edge, merge the nodes to be in a single cluster
    cheapestEdge = *edges[curCheapest];
    nodeX = *nodes[cheapestEdge.x];
    nodeY = *nodes[cheapestEdge.y];

    // if nodes of edge are not in same cluster, merge clusters:
    if (nodeX.leader != nodeY.leader) {
      // dumbly always merge y's cluster into x for now:
      // iterate all nodes if they have Y's leader, make it x's:
      for (nodeIt = nodes.begin(); nodeIt != nodes.end(); nodeIt++) {
        if (nodeIt->second->leader == nodeY.leader) {
          nodeIt->second->leader = nodeX.leader;
        }
      }
      i--;
    } // done changing leaders

    curCheapest++;
  }

  // curCheapest edge might now be pointing the next cheapest edge,
  // but those nodes of the edges might already be in a cluster!
  // find the next curCheapest in different clusters
  for (curCheapest; curCheapest < edges.size(); curCheapest++) {
    cheapestEdge = *edges[curCheapest];
    nodeX = *nodes[cheapestEdge.x];
    nodeY = *nodes[cheapestEdge.y];

    if (nodeX.leader != nodeY.leader) {
      break;
    }
  }

  cout << "Cluster Spacing: " << edges[curCheapest]->cost << endl;

  return 1;
}
