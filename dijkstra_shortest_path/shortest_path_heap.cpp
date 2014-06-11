#include <string>
#include <limits>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Edge {
  int head;
  int length;

  public:
    Edge(int headInit, int lengthInit) {
      head = headInit;
      length = lengthInit;
    }

    int getHead() {
      return head;
    }

    int getLength() {
      return length;
    }
};

class Node {
  int label;
  int greedyScore;
  map< int, Edge* > edges;

  public:
    Node(int labelInit) {
      label = labelInit;
      greedyScore = std::numeric_limits<int>::max();
    }

    int getLabel() {
      return label;
    }

    int setGreedyScore(int score) {
      greedyScore = score;
    }

    int getGreedyScore() {
      return greedyScore;
    }

    const map< int, Edge* >& getEdges() {
      return edges;
    }

    void pushEdge(Edge* edge) {
      if (edges[edge->getHead()] == 0) {
        edges[edge->getHead()] = edge;
      }
    }
};

struct CompareNodes {
  bool operator()(Node*& rhs, Node*& lhs) {
    return lhs->getGreedyScore() < rhs->getGreedyScore();
  }
};

int main() {
  ifstream dataFile("edges.txt");

  vector< Node* > nodesRemaining;
  map< int,Node* > nodesProcessed;
  map< int,Node* > nodesRemainingPositionIndex; // label->pointer to Node with that label

  string line;
  string tuple;
  int tail, head, length;
  Node* lastNode;

  while (getline(dataFile, line)) {
    istringstream iss(line);

    iss >> tail;

    nodesRemaining.push_back(new Node(tail));
    lastNode = nodesRemaining.back();
    nodesRemainingPositionIndex[lastNode->getLabel()] = lastNode;

    while (iss >> tuple) {
      sscanf(tuple.c_str(), "%d%*c%d", &head, &length);

      // push new edge onto the node
      lastNode->pushEdge(new Edge(head, length));
    }
  }
  dataFile.close();

  cout << "Nodes: " << nodesRemaining.size() << endl;

  // initialize
  int startNodeLabel = 1;

  // we need to find the start node in our vector and move it out of the heap:
  vector< Node* >::iterator itNodes;
  Node* startNode = nodesRemainingPositionIndex[startNodeLabel];

  Node* workingNode;
  // the startNode greedyScore is 0:
  startNode->setGreedyScore(0);

  map< int, Edge* >::iterator itEdge;
  map< int, Node* >::iterator itNodesMap;
  map< int, Edge* > edgesToExplore;
  int greedyScore;
  Node* headNode;

  while (nodesRemaining.size() > 0) {
//    cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    // we need to heapify the nodes list, the keys are the nodes' greedyScore() (init'd to MAX_INT)
    make_heap(nodesRemaining.begin(), nodesRemaining.end(), CompareNodes());

    // pop lowest from front of heap
    pop_heap(nodesRemaining.begin(), nodesRemaining.end());
    workingNode = nodesRemaining.back();
    nodesRemaining.pop_back();

//    cout << "Working Node (winner): " << workingNode->getLabel() << endl;

    // place it into the nodesProcessed:
    nodesProcessed[workingNode->getLabel()] = workingNode;

    // for all edges with tail in processed and head in remaining, calc the greedyScore:
    for (itNodesMap = nodesProcessed.begin(); itNodesMap != nodesProcessed.end(); itNodesMap++) {
      edgesToExplore = itNodesMap->second->getEdges();
      for (itEdge = edgesToExplore.begin(); itEdge != edgesToExplore.end(); itEdge++) {
//        cout << "  examine edge: " << itNodesMap->second->getLabel() << "->" << itEdge->second->getHead() << endl;

        // only calculate the head node's greedyScore if it is not in nodesProcessed:
        if (nodesProcessed.count(itEdge->second->getHead()) == 0) {
          // to calculate greedy score, add the greedy score of the tail to the length of this edge:
          greedyScore = itNodesMap->second->getGreedyScore() + itEdge->second->getLength();
//          cout << "    -> calculate greedy score of node " << itEdge->second->getHead() << " = " << greedyScore << endl;

          headNode = nodesRemainingPositionIndex[itEdge->second->getHead()];

          // if the node already has a lower greedy score, don't overwrite it:
          if (greedyScore < headNode->getGreedyScore()) {
            headNode->setGreedyScore(greedyScore);
          }
        }
      }
    }
  }

  cout << "Node / Distance from node " << startNode << ":" << endl;
  for (itNodesMap = nodesProcessed.begin(); itNodesMap != nodesProcessed.end(); itNodesMap++) {
    cout << itNodesMap->first << " / " << itNodesMap->second->getGreedyScore() << endl;
  }

  return 1;
}
