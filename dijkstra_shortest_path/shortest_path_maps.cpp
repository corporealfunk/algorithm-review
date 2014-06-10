#include <string>
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
  map< int, Edge* > edges;

  public:
    Node(int labelInit) {
      label = labelInit;
    }

    int getLabel() {
      return label;
    }

    const map< int, Edge* >& getEdges() {
      return edges;
    }

    void pushEdge(Edge* edge) {
      if (edges[edge->getHead()] == 0) {
        edges[edge->getHead()] = edge;
      }
    }

    void eraseEdge(int head) {
      delete edges[head];
      edges.erase(head);
    }
};

int main() {
  ifstream dataFile("edges.txt");

  map< int, Node* > nodes;
  map< int, int> nodesShortestDistance;
  map< int, vector<int> > nodesIncomingEdges;

  string line;
  string tuple;
  int tail, head, length;
  Node* lastNode;

  while (getline(dataFile, line)) {
    istringstream iss(line);

    iss >> tail;

    nodes[tail] = new Node(tail);
    lastNode = nodes[tail];

    while (iss >> tuple) {
      sscanf(tuple.c_str(), "%d%*c%d", &head, &length);

      // push new edge onto the node
      lastNode->pushEdge(new Edge(head, length));

      // track incoming edges
      nodesIncomingEdges[head].push_back(tail);
    }
  }
  dataFile.close();

  cout << "Nodes: " << nodes.size() << endl;

  // initialize
  int startNode = 1;
  nodesShortestDistance[startNode] = 0;

  // invalidate any incoming edges to the start node, those edges should never be considered
  for (int x = 0; x < nodesIncomingEdges[startNode].size(); x++) {
//    cout << "Invalidate starting node edge " << nodesIncomingEdges[startNode][x] << "->" << startNode << endl;

    nodes[nodesIncomingEdges[startNode][x]]->eraseEdge(startNode);
  }

  // loop all valid outgoing edges for nodes in nodesShortestDistance
  Node* currentNode;

  int currentScore, minimumScore;
  Node* winningNode = 0;
  map< int, Edge* > edgesToExplore;

  map< int, int >::iterator it;
  map< int, Edge* >::iterator itEdge;
  map< int, vector< int > >::iterator itIncomingNode;

  while (nodesShortestDistance.size() < nodes.size()) {
//    cout << "---------------------" << endl;

    minimumScore = -1;
    winningNode = 0;
    currentScore = -1;

    for (it = nodesShortestDistance.begin(); it != nodesShortestDistance.end(); it++) {
      currentNode = nodes[it->first];
//      cout << "node in x to explore: " << currentNode->getLabel() << endl;
      edgesToExplore = currentNode->getEdges();

      for (itEdge = edgesToExplore.begin(); itEdge != edgesToExplore.end(); itEdge++) {
        currentScore = it->second + itEdge->second->getLength();

        if (minimumScore == -1) {
          minimumScore = currentScore;
          winningNode = nodes[itEdge->first];
        }

//        cout << " -> head : " << itEdge->first << "\tl: " << itEdge->second->getLength() << "\ts: " << currentScore <<  endl;

        if (currentScore < minimumScore) {
          minimumScore = currentScore;
          winningNode = nodes[itEdge->first];
        }
      }
    }

//    cout << "Lowest score: " << minimumScore << ", node " << winningNode->getLabel() << endl;

    // now we need to add the winning node to X, with the score:
    nodesShortestDistance[winningNode->getLabel()] = minimumScore;

    // then we need to invalidate all incoming edges to the winning node, they should never be considered
    for (int x = 0; x < nodesIncomingEdges[winningNode->getLabel()].size(); x++) {
//      cout << "Invalidate winning node edge " << nodesIncomingEdges[winningNode->getLabel()][x] << "->" << winningNode->getLabel() << endl;

      nodes[nodesIncomingEdges[winningNode->getLabel()][x]]->eraseEdge(winningNode->getLabel());
    }
  }

  cout << "Node / Distance from node " << startNode << ":" << endl;
  for (it = nodesShortestDistance.begin(); it != nodesShortestDistance.end(); it++) {
    cout << it->first << " / " << it->second << endl;
  }


  return 1;
}
