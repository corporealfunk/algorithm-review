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
  int tail;
  int length;
  int procLength;

  public:
    Edge(int tailInit, int headInit, int lengthInit) {
      tail = tailInit;
      head = headInit;
      length = lengthInit;
      procLength = lengthInit;
    }

    int getHead() {
      return head;
    }

    int getTail() {
      return tail;
    }

    int getLength() {
      return length;
    }

    int getProcLength() {
      return procLength;
    }

    int setProcLength(int length) {
      procLength = length;
    }
};

class Node {
  int label;
  int greedyScore;
  int weight;
  map< int, Edge* > outgoingEdges;
  map< int, Edge* > incomingEdges;

  public:
    Node(int labelInit) {
      label = labelInit;
      greedyScore = std::numeric_limits<int>::max();
      weight = 0;
    }

    int getLabel() {
      return label;
    }

    int getWeight() {
      return weight;
    }

    int setWeight(int weightIn) {
      weight = weightIn;
    }

    int setGreedyScore(int score) {
      greedyScore = score;
    }

    int getGreedyScore() {
      return greedyScore;
    }

    const map< int, Edge* >& getOutgoingEdges() {
      return outgoingEdges;
    }

    void pushOutgoingEdge(Edge* edge) {
      if (outgoingEdges[edge->getHead()] == 0) {
        outgoingEdges[edge->getHead()] = edge;
      }
    }

    const map< int, Edge* >& getIncomingEdges() {
      return incomingEdges;
    }

    void pushIncomingEdge(Edge* edge) {
      if (incomingEdges[edge->getTail()] == 0) {
        incomingEdges[edge->getTail()] = edge;
      }
    }
};

struct CompareNodes {
  bool operator()(Node*& rhs, Node*& lhs) {
    return lhs->getGreedyScore() < rhs->getGreedyScore();
  }
};

int dijkstra(Node* startNode, vector< Node* >& nodes, map< int, Node* >& nodesMap) {
  vector< Node* >::iterator it;
  Node* node = 0;
  vector< Node* > nodesRemaining;
  map< int, Node* > nodesProcessed;

  // reset all greedy scores to max_int
  // push the nodes into nodesRemaining heap
  for (it = nodes.begin(); it != nodes.end(); it++) {
    node = *it;
    node->setGreedyScore(std::numeric_limits<int>::max());
    nodesRemaining.push_back(node);
  }

  // init: start node score is 0
  startNode->setGreedyScore(0);

  Node* workingNode = 0;

  map< int, Edge* >::iterator itEdge;
  map< int, Node* >::iterator itNodesMap;
  map< int, Edge* > edgesToExplore;
  int greedyScore;
  Node* headNode;

  int smallestScore = std::numeric_limits<int>::max();
  int actualScore = 0;

  while (nodesRemaining.size() > 0) {
    make_heap(nodesRemaining.begin(), nodesRemaining.end(), CompareNodes());

    // pop lowest from front of heap:
    pop_heap(nodesRemaining.begin(), nodesRemaining.end());
    workingNode = nodesRemaining.back();
    nodesRemaining.pop_back();

    // place it into processed:
    nodesProcessed[workingNode->getLabel()] = workingNode;

    // if our working Node's greedy score is max_int, don't process it
    if (workingNode->getGreedyScore() != std::numeric_limits<int>::max()) {
      // for all outgoing edges from workingNode:
      edgesToExplore = workingNode->getOutgoingEdges();
      for (itEdge = edgesToExplore.begin(); itEdge != edgesToExplore.end(); itEdge++) {
        // calculate the head's greedyScore if it is not in nodesProcessed:
        if (nodesProcessed.count(itEdge->second->getHead()) == 0) {
          // score is the working node's score + the edge length
          greedyScore = workingNode->getGreedyScore() + itEdge->second->getProcLength();

          // set the score on the head only if it is lower
          headNode = nodesMap[itEdge->second->getHead()];
          if (greedyScore < headNode->getGreedyScore()) {
            headNode->setGreedyScore(greedyScore);
          }
        }
      }
      actualScore = workingNode->getGreedyScore() - startNode->getWeight() + workingNode->getWeight();
      smallestScore = min(actualScore, smallestScore);
    }
  }

  return smallestScore;
}

bool bellmanFord(vector< Node* >& nodes, map< int, Node* >& nodesMap, vector< Edge*>& edges) {
  // setup our fake node for johnson's algo:
  Node* startNode = new Node(nodesMap.rbegin()->first + 1);
  Edge* edge = 0;
  Node* node = 0;

  // it has an edge to every node in nodes:
  vector< Node* >::iterator it;
 
  for (it = nodes.begin(); it != nodes.end(); it++) {
    // head node:
    node = *it;

    // new edge:
    edge = new Edge(startNode->getLabel(), node->getLabel(), 0);

    // outgoing from start node:
    startNode->pushOutgoingEdge(edge);

    // incoming to the head:
    node->pushIncomingEdge(edge);
  }

  // put it in our graph:
  nodesMap[startNode->getLabel()] = startNode;
  nodes.push_back(startNode);

  // put in first position:
  swap(nodes[0], nodes[nodes.size() - 1]);

  // init a 2D array n**2 in size, fill with 0:
  int** matrix = 0;
  matrix = new int*[nodes.size() + 1];
  for (int i = 0; i <= nodes.size(); i++) {
    matrix[i] = new int[nodes.size()];
    for (int j = 0; j < nodes.size(); j++) {
      matrix[i][j] = 0;
    }
  }

  // bellman ford:
  node = 0;
  int case1, case2, case2Test;
  map< int, Edge* > edgesToExplore;
  map< int, Edge* >::iterator itEdge;
  edge = 0;
  int jIndex = 0;
  for (int i = 0; i <= nodes.size(); i++) {
    for (int j = startNode->getLabel(); j >= 1; j--) {
      if (nodesMap.count(j) == 0) {
        cout << "NO NODE WITH LABEL: " << j << endl;
        throw;
      }

      node = nodesMap[j];
      jIndex = j - 1;

//      cout << "---------------" << endl;
//      cout << "SOLVING matrix: " << i << "," << jIndex << endl;

      if (i == 0) {
        // Base cases, no hops allowed:
        if (node->getLabel() == startNode->getLabel()) {
          // start node, path length is 0:
          matrix[i][jIndex] = 0;
        } else {
          // other node, no hops, set to int_max
          matrix[i][jIndex] = std::numeric_limits<int>::max();
        }
      } else {
        // at least 1 hop allowed:
        case1 = matrix[i - 1][jIndex];

        // for case2, we need the minimum of all the incoming
        // nodes edge costs added to their last hop value
        edgesToExplore = node->getIncomingEdges();
        case2 = std::numeric_limits<int>::max();
        for (itEdge = edgesToExplore.begin(); itEdge != edgesToExplore.end(); itEdge++) {
          edge = (*itEdge).second;

//          cout << "    exploring edge: " << edge->getTail() << "->" << edge->getHead() << ":" << edge->getLength() << endl;

          // if the edge's last hop value is int_max, don't even calculate it,
          // it was unreachable
//          cout << "    examining matrix: " << i - 1 << "," << edge->getTail() - 1 << ": " << matrix[i - 1][edge->getTail() - 1] << endl;
          if (matrix[i - 1][edge->getTail() - 1] != std::numeric_limits<int>::max()) {
            case2Test = matrix[i - 1][edge->getTail() - 1] + edge->getLength();
//            cout << "      case2Test (not max): " << case2Test << endl;
//            cout << "        edge length: " << edge->getLength() << endl;
          } else {
            case2Test = std::numeric_limits<int>::max();
//            cout << "      case2Test (max int): " << case2Test << endl;
          }
          case2 = min(case2, case2Test);
        }

        // the winner is our matrix value:
        matrix[i][jIndex] = min(case1, case2);
      }

//      cout << i << "," << jIndex << ": " << matrix[i][jIndex] << endl;
    }
  }

  // do we have a negative cycle? if our last two columns of the
  // matrix are not the same, then yes, we have a neg cycle
  bool negativeCycleFlag = false;

  int i = nodes.size();
  for (int j = 0; j < nodes.size(); j++) {
    if (matrix[i][j] != matrix[i - 1][j]) {
      negativeCycleFlag = true;
    }
  }

  // if no negative cycle, the shortest distance to each node
  // is the vertex weight located at i = n
  if (!negativeCycleFlag) {
    i = nodes.size();
    int nodeLabel = 0;
    for (int j = 0; j < nodes.size(); j++) {
      nodeLabel = j + 1;
      nodesMap[nodeLabel]->setWeight(matrix[i][j]);
    }
  }

  // for each edge, set a processed weight
  vector< Edge* >::iterator itVEdge;
  Node* head = 0;
  Node* tail = 0;
  for (itVEdge = edges.begin(); itVEdge != edges.end(); itVEdge++) {
    edge = *itVEdge;
    head = nodesMap[edge->getHead()];
    tail = nodesMap[edge->getTail()];

    edge->setProcLength(edge->getLength() + tail->getWeight() - head->getWeight());
  }

  // cleanup startNode:
  nodesMap.erase(startNode->getLabel());
  swap(nodes[0], nodes[nodes.size() - 1]);
  nodes.pop_back();
  delete(startNode);

  // clean up matrix:
  for (int i = 0; i <= nodes.size(); i++) {
    delete(matrix[i]);
  }
  delete matrix;

  return negativeCycleFlag;
}

int main() {
  ifstream dataFile("input.txt");
  string line;
  int numNodes, numEdges;

  getline(dataFile, line);
  istringstream iss(line);

  iss >> numNodes >> numEdges;

  cout << "Nodes: " << numNodes << " Edges: " << numEdges << endl;

  vector< Node* > nodes;
  vector< Edge* > edges;
  map< int, Node* > nodesMap;
  Node* tailNode = 0;
  Node* headNode = 0;
  Edge* edge = 0;

  int tail, head, length;
  while (getline(dataFile, line)) {
    istringstream iss(line);
    iss >> tail >> head >> length;

    edge = new Edge(tail, head, length);
    edges.push_back(edge);

    // push outgoing edge to the tail:
    if (nodesMap.count(tail) == 0) {
      tailNode = new Node(tail);

      nodes.push_back(tailNode);
      nodesMap[tailNode->getLabel()] = tailNode;
    } else {
      tailNode = nodesMap[tail];
    }

    tailNode->pushOutgoingEdge(edge);

    // push incoming edge to the head:
    if (nodesMap.count(head) == 0) {
      headNode = new Node(head);

      nodes.push_back(headNode);
      nodesMap[headNode->getLabel()] = headNode;
    } else {
      headNode = nodesMap[head];
    }

    headNode->pushIncomingEdge(edge);
  }
  dataFile.close();

  // Run Bellman-Ford with a fake startnode:
  cout << "Running Bellman-Ford..." << endl;
  bool negativeCycleFlag = bellmanFord(nodes, nodesMap, edges);

  cout << "Negative Cycle Detected?: " << negativeCycleFlag << endl;

  if (!negativeCycleFlag) {
    Node* node = 0;
    vector< Node* >::iterator it;
    int shortestPath;
    int allPairsShortest = std::numeric_limits<int>::max();

    for (it = nodes.begin(); it != nodes.end(); it++) {
      // for each node, run Dijkstra:
      node = *it;

      // compute short paths, get the smallest one here:
      shortestPath = dijkstra(node, nodes, nodesMap);
      allPairsShortest = min(allPairsShortest, shortestPath);
      cout << "Start Node " << node->getLabel() << " Shortest Path: " << shortestPath << endl;
    }

    cout << "Shortest Shortest: " << allPairsShortest << endl;
  }

  return 1;
}
