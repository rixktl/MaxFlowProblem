//
//  Assignment_9
//
// ---- main.cpp follows ----

#include <iostream>
#include <string>
using namespace std;
#include "FHflowGraph.h"

int main() {
   int finalFlow;
   FHflowGraph<string, int> myG;
   myG.addEdge("s","a", 3);
   myG.addEdge("s","b", 2);
   myG.addEdge("a","b", 1);
   myG.addEdge("a","c", 3);
   myG.addEdge("a","d", 4);
   myG.addEdge("b","d", 2);
   myG.addEdge("c","t", 2);
   myG.addEdge("d","t", 3);

   cout << "Original tables:\n";
   myG.showResAdjTable();
   myG.showFlowAdjTable();

   myG.setStartVert("s");
   myG.setEndVert("t");

   finalFlow = myG.findMaxFlow();
   cout << "Final flow: " << finalFlow << endl << endl;

   cout << "Tables after findMaxFlow:\n";
   myG.showResAdjTable();
   myG.showFlowAdjTable();

   FHflowGraph<string, int> myG2;
   myG2.addEdge("0", "1", 16);
   myG2.addEdge("0", "2", 13);
   myG2.addEdge("1", "3", 12);
   myG2.addEdge("1", "2", 10);
   myG2.addEdge("2", "1", 4);
   myG2.addEdge("2", "4", 14);
   myG2.addEdge("3", "2", 9);
   myG2.addEdge("3", "5", 20);
   myG2.addEdge("4", "3", 7);
   myG2.addEdge("4", "5", 4);

   myG2.setStartVert("0");
   myG2.setEndVert("5");

   cout << "Another graph with correct final flow of 23" << endl;
   finalFlow = myG2.findMaxFlow();
   cout << "Final flow: " << finalFlow << endl << endl;

   return 0;
}
