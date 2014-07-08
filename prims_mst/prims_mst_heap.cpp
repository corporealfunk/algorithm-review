#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

struct Edge {
  int cost;
  int tail;
  int head;
  bool explored;
};

struct Node {
  int label;
  int score;
  vector < Edge* > edges;
  bool explored;
};

bool compareNodes(Node* nodeL, Node* nodeR) {
  return (nodeL->score > nodeR->score);
}

void printNode(Node* node) {
  cout << "Node: " << node->label << " score: " << node->score << endl;
  cout << "  Edges: ";

  vector < Edge* >::iterator it;
  for (it = node->edges.begin(); it != node->edges.end(); it++) {
    cout << (*it)->tail << "->" << (*it)->head << " ";
  }
  cout << endl;
}

void calculateScore(Node* node, map < int, Node* > nodeMap) {
  // for each edge of this node that crosses the boundary
  // our score is the cost of the cheapest one

  vector < Edge* >::iterator it;
  Node* t;
  Node* h;
  for (it = node->edges.begin(); it != node->edges.end(); it++) {
    // if the edge is already explored, ignore it
    if (!(*it)->explored) {
      // this edge crosses the boundary if one side is explored and the other is not
      t = nodeMap[(*it)->tail];
      h = nodeMap[(*it)->head];
      if ((t->explored && !h->explored) || (h->explored && !t->explored)) {
        // this edge is in the running, it crosses the boundary
        if ((*it)->cost < node->score) {
          node->score = (*it)->cost;
        }
      }
    }
  }

}
void markExploredEdges(Node* node, map < int, Node* > nodeMap) {
  // for each edge of this node that has two explored nodes
  // mark as explored

  vector < Edge* >::iterator it;
  Node* t;
  Node* h;
  for (it = node->edges.begin(); it != node->edges.end(); it++) {
    // if the edge is already explored, ignore it
    if (!(*it)->explored) {
      // this edge is explored if both tail and head are explored
      t = nodeMap[(*it)->tail];
      h = nodeMap[(*it)->head];
      if (t->explored && h->explored) {
        // this edge is explored:
        (*it)->explored = true;
      }
    }
  }
}

int main() {
  ifstream dataFile("graph.txt");
  string line;
  int numEdges, numNodes;

  getline(dataFile, line);
  istringstream iss(line);

  iss >> numNodes >> numEdges;
  cout << "Nodes: " << numNodes << ", Edges: " << numEdges << endl;

  vector < Edge* > edges;
  vector < Node* > nodes;
  vector < Node* > nodesExplored;
  map < int, Node* > nodeMap;

  int head, tail, cost;
  while (getline(dataFile, line)) {
    istringstream iss(line);

    iss >> head >> tail >> cost;

    // create an edge:
    edges.push_back(new Edge());
    edges.back()->tail = tail;
    edges.back()->head = head;
    edges.back()->cost = cost;
    edges.back()->explored = false;

    // create the nodes if necesary:
    if (nodeMap.count(tail) == 0) {
      nodes.push_back(new Node());
      nodes.back()->score = std::numeric_limits<int>::max();
      nodes.back()->label = tail;
      nodes.back()->explored = false;
      nodeMap[tail] = nodes.back();
    }

    if (nodeMap.count(head) == 0) {
      nodes.push_back(new Node());
      nodes.back()->label = head;
      nodes.back()->score = std::numeric_limits<int>::max();
      nodes.back()->explored = false;
      nodeMap[head] = nodes.back();
    }

    // push the edge onto the tail and head:
    nodeMap[tail]->edges.push_back(edges.back());
    nodeMap[head]->edges.push_back(edges.back());
  }

  dataFile.close();

  Node* s;
  Node* t;

  int mstCost = 0;
  while (nodes.size() > 0) {
//    cout << "---------------" << endl;
    // get the vertice with lowest score:
    make_heap(nodes.begin(), nodes.end(), compareNodes);
//    cout << "first heap node:" << endl;
    s = nodes[0];

    // it is now explored, mark and move it into nodesExplored
    s->explored = true;

    // if we are not the first node, add it's score to the mstCost:
    if (nodesExplored.size() > 0) {
      mstCost += s->score;
    }

    nodesExplored.push_back(s);

    //remove from heap
    pop_heap(nodes.begin(), nodes.end());
    nodes.pop_back();

    // find any edges of this node that are now totally explored:
    markExploredEdges(s, nodeMap);

//    printNode(s);

    // now we need to re-calculate the score for any vertice attached to this
    // node
    vector< Edge* >::iterator it;
    vector< Edge* >::iterator nodeEdges;
    for (it = s->edges.begin(); it != s->edges.end(); it++) {
      // only recalc it if it is not node s
      // AND if it is not explored already
      t = nodeMap[(*it)->tail];
      if (t->label != s->label && !t->explored) {
//        cout << "Recalc vertice: " << (*it)->tail << endl;
        calculateScore(t, nodeMap);
//        printNode(t);
      }

      t = nodeMap[(*it)->head];
      if (t->label != s->label && !t->explored) {
//        cout << "Recalc vertice: " << (*it)->head << endl;
        calculateScore(t, nodeMap);
//        printNode(t);
      }
    }
  }

  cout << "------------" << endl;
  cout << "mst Cost: " << mstCost << endl;
  return 1;
}

// answer: 2624
