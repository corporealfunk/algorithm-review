#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Edge {
  int x;
  int y;
};

int randomNumber(int start, int end) {
  int range = end - start + 1;
  return rand() % range + start;
}

void printEdges(vector<Edge>& edges) {
  for (int i = 0; i < edges.size(); i++) {
    cout << "EDGE: " << edges[i].x << "-" << edges[i].y << endl;
  }
}

void mergeEdge(vector<Edge>& edges, vector<int>& vertices, Edge& randomEdge) {
  // loop all edges, if the edge.x is equal to our random edge x, change it y
  // (essentially, we are removing randomEdge.x vertice and changing it to y)
  int verticeToRemove = randomEdge.x;
  for (int i = 0; i < edges.size(); i++) {
    if (edges[i].x == verticeToRemove) {
      edges[i].x = randomEdge.y;
    }

    if (edges[i].y == verticeToRemove) {
      edges[i].y = randomEdge.y;
    }
  }

  // kill self loops:
  vector<Edge>::iterator it;
  for(it = edges.begin(); it != edges.end();) {
    if ((*it).x == (*it).y) {
      it = edges.erase(it);
    } else {
      it++;
    }
  }

  // find the verticeToRemove in vertices and erase it:
  for (int i = 0; i < vertices.size(); i++) {
    if (vertices[i] == verticeToRemove) {
      vertices.erase(vertices.begin() + i);
    }
  }
}

int findMinimumCut(vector<Edge> edges, vector<int> vertices) {
  // 1) pick an edge to merge:
  int edgeEnd;
  int randomIndex;

  // do this until we only have two vertices left
  do {
    edgeEnd = edges.size() - 1;
    Edge randomEdge;

    randomIndex = randomNumber(0, edgeEnd);
    randomEdge = edges[randomIndex];
    //cout << "Random Edge[" << randomIndex << "]: " << randomEdge.x << "-" << randomEdge.y << endl;

    // 2) merge the edge:
    //    - examine x, any edge terminating in x must now terminate in y:
    mergeEdge(edges, vertices, randomEdge);

  } while(vertices.size() > 2);
  //printEdges(edges);
  //cout << "     inside edge size: " << edges.size() << endl;
  return edges.size();
}

int main() {
  srand(time(0));
  ifstream dataFile("edges.txt");
  string line;

  vector<Edge> edges;
  vector<int> vertices;

  int label, y;

  int edgeCounter = 0;
  while (getline(dataFile, line)) {
    istringstream iss(line);

    iss >> label;
    vertices.push_back(label);
    while (iss >> y) {
      if (y < label) {
        edges.push_back(Edge());
        edges[edgeCounter].x = label;
        edges[edgeCounter].y = y;
        edgeCounter++;
      }
    }
  }
  dataFile.close();
  cout << "EDGES: " << edges.size() << endl;

  //printEdges(edges);

  int currentCut = 0;
  int minimumCut = edges.size();
  int numberTrails = pow((double)vertices.size(), 2) * (log((double)vertices.size()));
  cout << "Trails: " << numberTrails << endl;
  for (int i = 0; i < numberTrails; i++) {
    currentCut = findMinimumCut(edges, vertices);
    if (currentCut < minimumCut) {
      minimumCut = currentCut;
    }
    //cout << "Trial " << i << ": " << currentCut << endl;
    //cout << "Curent Min: " << minimumCut << endl;
    if (i % 1000 == 0) {
      cout << "Trail: " << i << ". Cur Min: " << minimumCut << endl;
    }
  }
  cout << "Min Cut: " << minimumCut << endl;

  return 1;
}
