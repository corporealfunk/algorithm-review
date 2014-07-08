#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Job {
  int weight;
  int length;
  float score;
  int completionTime;
};

bool jobCompare(Job jobL, Job jobR) {
  if (jobL.score != jobR.score) {
    return jobL.score < jobR.score;
  } else {
    return jobL.weight > jobR.weight;
  }
}

int main() {
  ifstream dataFile("jobs.txt");
  string line;
  int numJobs;

  getline(dataFile, line);
  istringstream iss(line);
  iss >> numJobs;

  Job* jobs = new Job[numJobs];

  int i = 0;
  while (getline(dataFile, line)) {
    istringstream iss(line);

    jobs[i] = Job();
    iss >> jobs[i].weight;
    iss >> jobs[i].length;
    jobs[i].score = (float)jobs[i].length / (float)jobs[i].weight;
    i++;
  }
  dataFile.close();

  // sort by score:
  sort(jobs, jobs + numJobs, jobCompare);

  // calculate completion times
  double weightedCompTimes = 0;
  int compTime = 0;
  for (int i = 0; i < numJobs; i++) {
    compTime += jobs[i].length;
    jobs[i].completionTime = compTime;
    weightedCompTimes += (jobs[i].completionTime * jobs[i].weight);
    cout << "JOB: l: " << jobs[i].length << " w: " << jobs[i].weight << " s: " << jobs[i].score << " ct: " << jobs[i].completionTime << " wct: " << jobs[i].completionTime * jobs[i].weight << endl;
  }

  cout << setprecision(20) << "Sum of Weighted Comp Times: " << weightedCompTimes << endl;
  return 1;
}
