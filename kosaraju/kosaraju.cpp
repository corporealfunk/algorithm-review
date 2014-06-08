/*
 * TODO: this requires quite a bit of free memory in the heap for large data sets.
 * might be better to figure out a more efficient data storage structure than the
 * currently designed vector of Node pointers
 */
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

class Node {
  int label;
  bool explored;
  Node* leader;
  int finishingTime;

  vector<Node*> outgoingNodes;
  vector<Node*> incomingNodes;

  public:
  Node(int labelName) {
    explored = false;
    leader = 0;
    finishingTime = -1;
    label = labelName;
  }

  Node() {
    explored = false;
    leader = 0;
    finishingTime = -1;
    label = -1;
  }

  // getters and setters
  int getLabel() {
    return label;
  }

  void setLabel(int labelName) {
    label = labelName;
  }

  int getPosition() {
    return (label - 1);
  }

  Node* getLeader() {
    return leader;
  }

  void setLeader(Node* nodeLeader) {
    leader = nodeLeader;
  }

  bool isExplored() {
    return explored;
  }

  void markExplored() {
    explored = true;
  }

  void markUnExplored() {
    explored = false;
  }

  int getFinishingTime() {
    return finishingTime;
  }

  void setFinishingTime(int time) {
    finishingTime = time;
  }

  const vector<Node*>& getOutgoingNodes() {
    return outgoingNodes;
  }

  void pushOutgoingNode(Node* node) {
    outgoingNodes.push_back(node);
  }

  const vector<Node*>& getIncomingNodes() {
    return incomingNodes;
  }

  void pushIncomingNode(Node* node) {
    incomingNodes.push_back(node);
  }
};

void printNode(Node* node) {
  cout << node->getPosition() << ": " << ((node->isExplored()) ? "Y" : "N" ) << " - fT: " << node->getFinishingTime() << " - o: ";

  vector<Node*> outgoingNodes = node->getOutgoingNodes();
  vector<Node*> incomingNodes = node->getIncomingNodes();

  for (int x = 0; x < outgoingNodes.size(); x++) {
    cout << outgoingNodes[x]->getPosition() << " ";
  }

  cout << " i: ";

  for (int x = 0; x < incomingNodes.size(); x++) {
    cout << incomingNodes[x]->getPosition() << " ";
  }

  cout << endl;
}

void printNodes(vector<Node*>& nodes) {
  int length = nodes.size();
  for (int i = 0; i < length; i++) {
    if (nodes[i] != 0) {
      printNode(nodes[i]);
    }
  }
}

void dfsFirstPass(vector<Node*>& nodes, int nodeIndex, int& finishingTime, Node** finishingOrdered) {
  Node* startNode = nodes[nodeIndex];

  vector<Node*> nodesToExplore;

  // look at reverse direction of edges
  nodesToExplore = startNode->getIncomingNodes();

  nodes[nodeIndex]->markExplored();
  int length = nodesToExplore.size();
  for (int i = 0; i < length; i++) {
    if (!nodesToExplore[i]->isExplored()) {
      dfsFirstPass(nodes, nodesToExplore[i]->getPosition(), finishingTime, finishingOrdered);
      nodesToExplore[i]->markExplored();
    }
  }

  finishingTime++;
  nodes[nodeIndex]->setFinishingTime(finishingTime);
  finishingOrdered[finishingTime - 1] = nodes[nodeIndex];
}

void dfsSecondPass(vector<Node*>& nodes, int nodeIndex, Node* leader, map<int,int>& leaderBoard) {
  Node* startNode = nodes[nodeIndex];

  vector<Node*> nodesToExplore;

  // normal graph direction
  nodesToExplore = startNode->getOutgoingNodes();

  nodes[nodeIndex]->markExplored();
  for (int i = 0; i < nodesToExplore.size(); i++) {
    if (!nodesToExplore[i]->isExplored()) {
      dfsSecondPass(nodes, nodesToExplore[i]->getPosition(), leader, leaderBoard);
      nodesToExplore[i]->markExplored();
    }
  }

  nodes[nodeIndex]->setLeader(leader);
  leaderBoard[leader->getPosition()]++;
}

int main() {
  srand(time(0));

  ifstream dataFile("edges.txt");
  string line;

  vector<Node*> nodes;

  int tailLabel, headLabel, tailIndex, headIndex;
  int counter = 0;
  int newSize = 0;
  while (getline(dataFile, line)) {
    istringstream iss(line);
    iss >> tailLabel;
    iss >> headLabel;
    tailIndex = tailLabel - 1;
    headIndex = headLabel - 1;

    if (tailLabel > headLabel) {
      newSize = tailLabel;
    } else {
      newSize = headLabel;
    }

    if (newSize > nodes.size()) {
      cout << "Resizing Nodes to " << newSize << endl;
      nodes.resize(newSize);
    }

    // setup the tail if needed:
    if (nodes[tailIndex] == 0) {
      nodes[tailIndex] = new Node(tailLabel);
    }

    // setup the head if needed)
    if (nodes[headIndex] == 0) {
      nodes[headIndex] = new Node(headLabel);
    }

    // don't push self loops
    if (tailIndex != headIndex) {
      // push a pointer to the head in outgoing nodes of the tail:
      nodes[tailIndex]->pushOutgoingNode(nodes[headIndex]);

      // push a pointer to the tail in the incoming nodes of the head:
      nodes[headIndex]->pushIncomingNode(nodes[tailIndex]);
    }
  }

  //printNodes(nodes);

  // First Pass: run only on incoming nodes (reverse the graph)
  // compute the finishing order of every node
  // store the finishing order in an array so we can easily iterate through
  // the nodes in this order later
  Node** finishingOrdered = new Node*[nodes.size()];

  cout << "First Pass" << endl;
  int finishingTime = 0;
  for (int i = 0; i < nodes.size(); i++) {
    if (nodes[i] != 0) {
      if (!nodes[i]->isExplored()) {
        dfsFirstPass(nodes, i, finishingTime, finishingOrdered);
      }
    }
  }

  // reset the explored bool to false on all nodes:
  for (int i = 0; i < nodes.size(); i++) {
    if (nodes[i] != 0) {
      nodes[i]->markUnExplored();
    }
  }

  cout << "Second Pass" << endl;
  //printNodes(nodes);

  // Second Pass: loop in reverse finishing order. operate on outgoing nodes
  // (normal directed graph order). run dfs, keep track of leader nodes
  Node* leader = 0;
  map<int,int> leaderBoard;
  for (int i = nodes.size() - 1; i >= 0; i--) {
    if (finishingOrdered[i] != 0) {
      if (!finishingOrdered[i]->isExplored()) {
        leader = finishingOrdered[i];
        leaderBoard[leader->getLabel()] = 0;
        dfsSecondPass(nodes, leader->getPosition(), leader, leaderBoard);
      }
    }
  }

  // Get leader board to into leader values
  map<int,int>::iterator it;
  int* leaderValues = new int[leaderBoard.size()];
  int countLeaders = 0;
  for(it = leaderBoard.begin(); it != leaderBoard.end(); it++) {
    leaderValues[countLeaders] = it->second;
    countLeaders++;
  }

  sort(leaderValues, leaderValues + leaderBoard.size(), greater<int>());

  cout << "Top 5 Strongly Connected Component Node Couts:" << endl;
  int maxSize = min((double)5, (double)leaderBoard.size());
  for (int i = 0; i < maxSize; i++) {
    cout << leaderValues[i] << ",";
  }
  cout << endl;

  return 1;
}
