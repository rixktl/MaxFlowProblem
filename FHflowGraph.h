// ---- FHflowGraph.h follows ----

#ifndef FHflowGraph_H
#define FHflowGraph_H

#include <limits.h>
#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <stack>
#include <iostream>
#include <functional>
using namespace std;

template <class Object, typename CostType>
class FHflowVertex {
   typedef FHflowVertex<Object, CostType>* VertPtr;
   typedef map<VertPtr, CostType> EdgePairList;

public:
   static int nSortKey;
   static stack<int> keyStack;
   static enum { SORT_BY_DATA, SORT_BY_DIST } eSortType;
   static bool setNSortType(int whichType);
   static void pushSortKey() { keyStack.push(nSortKey); }
   static void popSortKey() { nSortKey = keyStack.top(); keyStack.pop(); }

   static int const INFINITY_FH = INT_MAX; // Defined in limits.h
   EdgePairList flowAdjList;
   EdgePairList resAdjList;
   Object data;
   CostType dist;
   VertPtr nextInPath; // Used for client-specific info

   FHflowVertex(const Object & x = Object());

   void addToFlowAdjList(VertPtr neighbor, CostType cost);
   void addToResAdjList(VertPtr neighbor, CostType cost);
   bool operator<(const FHflowVertex<Object, CostType> & rhs) const;
   const FHflowVertex<Object, CostType> & operator=
   (const FHflowVertex<Object, CostType> & rhs);
   void showFlowAdjList();
   void showResAdjList();
};

// Static const initializations for Vertex begins --------------
template <class Object, typename CostType>
int FHflowVertex<Object, CostType>::nSortKey
= FHflowVertex<Object, CostType>::SORT_BY_DATA;

template <class Object, typename CostType>
stack<int> FHflowVertex<Object, CostType>::keyStack;
// Static const initializations for Vertex ends ----------------

template <class Object, typename CostType>
bool FHflowVertex<Object, CostType>::setNSortType(int whichType) {
   switch (whichType) {
      case SORT_BY_DATA:
      case SORT_BY_DIST:
         nSortKey = whichType;
         return true;
      default:
         return false;
   }
}

template <class Object, typename CostType>
FHflowVertex<Object, CostType>::FHflowVertex(const Object & x)
: data(x), dist((CostType)INFINITY_FH),
nextInPath(NULL) {
   // Nothing to do
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::addToFlowAdjList
(FHflowVertex<Object, CostType> *neighbor, CostType cost) {
   flowAdjList[neighbor] = cost;
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::addToResAdjList
(FHflowVertex<Object, CostType> *neighbor, CostType cost) {
   resAdjList[neighbor] = cost;
}

template <class Object, typename CostType>
bool FHflowVertex<Object, CostType>::operator<
(const FHflowVertex<Object, CostType> & rhs) const {
   switch (nSortKey) {
      case SORT_BY_DIST:
         return (dist < rhs.dist);
      case SORT_BY_DATA:
         return (data < rhs.data);
      default:
         return false;
   }
}

template <class Object, typename CostType>
const FHflowVertex<Object, CostType> & FHflowVertex<Object, CostType>::operator=
(const FHflowVertex<Object, CostType> & rhs) {
   flowAdjList = rhs.flowAdjList;
   resAdjList = rhs.resAdjList;
   data = rhs.data;
   dist = rhs.dist;
   nextInPath = rhs.nextInPath;;
   return *this;
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::showFlowAdjList() {
   typename EdgePairList::iterator iter;

   cout << "Adj Flow List for " << data << ": ";
   for (iter = flowAdjList.begin(); iter != flowAdjList.end(); ++iter)
      cout << iter->first->data << "(" << iter->second << ") ";
   cout << endl;
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::showResAdjList() {
   typename EdgePairList::iterator iter;

   cout << "Adj Res List for " << data << ": ";
   for (iter = resAdjList.begin(); iter != resAdjList.end(); ++iter)
      cout << iter->first->data << "(" << iter->second << ") ";
   cout << endl;
}

template <class Object, typename CostType>
class FHflowGraph {
   // Internal typedefs to simplify syntax
   typedef FHflowVertex<Object, CostType> Vertex;
   typedef FHflowVertex<Object, CostType>* VertPtr;
   typedef map<VertPtr, CostType> EdgePairList;
   typedef set<VertPtr> VertPtrSet;
   typedef set<Vertex> VertexSet;

private:
   VertPtrSet vertPtrSet;
   VertexSet vertexSet;
   VertPtr startVertPtr, endVertPtr;

public:
   class EdgeNotFoundException {};
   FHflowGraph () {}
   void addEdge(const Object &source, const Object &dest, CostType cost);
   VertPtr addToVertexSet(const Object & object);
   void showFlowAdjTable();
   void showResAdjTable();
   void clear();
   bool setStartVert(const Object &x);
   bool setEndVert(const Object &x);
   CostType findMaxFlow();

private:
   VertPtr getVertexWithThisData(const Object & x);
   bool establishNextFlowPath();
   CostType getLimitingFlowOnResPath();
   CostType getCostOfResEdge(VertPtr src, VertPtr dst);
   bool adjustPathByCost(CostType cost);
   bool addCostToResEdge(VertPtr src, VertPtr dst, CostType cost);
   bool addCostToFlowEdge(VertPtr src, VertPtr dst, CostType cost);
};

template <class Object, typename CostType>
FHflowVertex<Object, CostType>* FHflowGraph<Object, CostType>::addToVertexSet
(const Object & object) {
   pair<typename VertexSet::iterator, bool> retVal;
   VertPtr vPtr;

   // Save sort key for client
   Vertex::pushSortKey();
   Vertex::setNSortType(Vertex::SORT_BY_DATA);

   // Build and insert vertex into master list
   retVal = vertexSet.insert(Vertex(object));

   // Get pointer to this vertex and put into vert pointer list
   vPtr = (VertPtr)&*retVal.first;
   vertPtrSet.insert(vPtr);

   Vertex::popSortKey(); // Restore client sort key
   return vPtr;
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::clear() {
   vertexSet.clear();
   vertPtrSet.clear();
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::addEdge(const Object &source,
                                        const Object &dest, CostType cost) {
   VertPtr src, dst;

   // Put both source and dest into vertex list(s) if not already there
   src = addToVertexSet(source);
   dst = addToVertexSet(dest);

   // Add dest to source's flow adjacency list with no cost
   src->addToFlowAdjList(dst, CostType());
   // Add dest to source's res adjacency list with actual cost
   src->addToResAdjList(dst, cost);
   // Add source to dest's res adjacency list with no cost (reverse edge)
   dst->addToResAdjList(src, CostType());
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::showFlowAdjTable() {
   typename VertPtrSet::iterator iter;

   cout << "---------- Flow Adjacency Table ----------\n";
   for (iter = vertPtrSet.begin(); iter != vertPtrSet.end(); ++iter)
      (*iter)->showFlowAdjList();
   cout << endl;
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::showResAdjTable() {
   typename VertPtrSet::iterator iter;

   cout << "---------- Residual Adjacency Table ----------\n";
   for (iter = vertPtrSet.begin(); iter != vertPtrSet.end(); ++iter)
      (*iter)->showResAdjList();
   cout << endl;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::setStartVert(const Object &x) {
   VertPtr ptr = getVertexWithThisData(x);
   if(ptr == NULL)
      return false;
   startVertPtr = ptr;
   return true;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::setEndVert(const Object &x) {
   VertPtr ptr = getVertexWithThisData(x);
   if(ptr == NULL)
      return false;
   endVertPtr = getVertexWithThisData(x);
   return true;
}

template <class Object, typename CostType>
FHflowVertex<Object, CostType>* FHflowGraph<Object, CostType>::getVertexWithThisData
(const Object & x) {
   typename VertexSet::iterator findResult;
   Vertex vert(x);

   Vertex::pushSortKey(); // Save client sort key
   Vertex::setNSortType(Vertex::SORT_BY_DATA);
   findResult = vertexSet.find(vert);
   Vertex::popSortKey(); // Restore client value

   if (findResult == vertexSet.end())
      return NULL;
   return  (VertPtr)&*findResult; // Address of vertex in set of originals
}

template <class Object, typename CostType>
CostType FHflowGraph<Object, CostType>::findMaxFlow() {
   while(establishNextFlowPath()) {
      CostType minCost = getLimitingFlowOnResPath();
      if(!adjustPathByCost(minCost) || minCost == CostType())
         break;
   }

   CostType totalCost = CostType();
   typename EdgePairList::iterator iter;
   for(iter = startVertPtr->flowAdjList.begin();
       iter != startVertPtr->flowAdjList.end(); iter++) {
      totalCost += iter->second;
   }

   return totalCost;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::establishNextFlowPath() {
   typename VertPtrSet::iterator vIter;
   typename EdgePairList::iterator edgePrIter;
   VertPtr wPtr, sPtr, vPtr;
   CostType costVW;
   queue<VertPtr> partiallyProcessedVerts;

   sPtr = startVertPtr;
   if (sPtr == NULL)
      return false;

   // Initialize the vertex list and place the starting vert in p_p_v queue
   for (vIter = vertPtrSet.begin(); vIter != vertPtrSet.end(); ++vIter) {
      (*vIter)->dist = Vertex::INFINITY_FH;
      (*vIter)->nextInPath = NULL;
   }

   sPtr->dist = 0;
   partiallyProcessedVerts.push(sPtr);

   while(!partiallyProcessedVerts.empty()) {
      vPtr = partiallyProcessedVerts.front();
      partiallyProcessedVerts.pop();
      if(vPtr == endVertPtr)
         return true;
      // For each vert adj to v, lower its dist to s if possible
      for (edgePrIter = vPtr->resAdjList.begin();
           edgePrIter != vPtr->resAdjList.end();
           edgePrIter++) {
         wPtr = edgePrIter->first;
         costVW = edgePrIter->second;
         if(costVW == CostType())
            continue;
         if (vPtr->dist + costVW < wPtr->dist) {
            wPtr->dist = vPtr->dist + costVW;
            wPtr->nextInPath = vPtr;

            // *wPtr now has improved distance, so add wPtr to p_p_v queue
            partiallyProcessedVerts.push(wPtr);
         }
      }
   }
   return false;
}

template <class Object, typename CostType>
CostType FHflowGraph<Object, CostType>::getLimitingFlowOnResPath() {
   if (startVertPtr == NULL || endVertPtr == NULL)
      return CostType();

   CostType minCost = getCostOfResEdge(endVertPtr->nextInPath, endVertPtr);
   for (VertPtr vp = endVertPtr; vp != startVertPtr; vp = vp->nextInPath) {
      CostType cost = getCostOfResEdge(vp->nextInPath, vp);
      minCost = (minCost > cost ? cost : minCost);
      if (vp->nextInPath == NULL) {
         throw EdgeNotFoundException();
      }
   }

   return minCost;
}

template <class Object, typename CostType>
CostType FHflowGraph<Object, CostType>::getCostOfResEdge(VertPtr src,
                                                         VertPtr dst) {
   if(src == NULL || dst == NULL) {
      throw EdgeNotFoundException();
   }

   typename EdgePairList::iterator iter;
   iter = src->resAdjList.find(dst);

   if(iter != src->resAdjList.end()) {
      return iter->second;
   } else {
      throw EdgeNotFoundException();
   }
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::adjustPathByCost(CostType cost) {
   if (startVertPtr == NULL || endVertPtr == NULL)
      return false;

   for (VertPtr vp = endVertPtr; vp != startVertPtr; vp = vp->nextInPath) {
      addCostToResEdge(vp->nextInPath, vp, -cost);
      addCostToResEdge(vp, vp->nextInPath, cost);
      addCostToFlowEdge(vp->nextInPath, vp, cost);
      if (vp->nextInPath == NULL) {
         return false;
      }
   }

   return true;
}


template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::addCostToResEdge(VertPtr src, VertPtr dst,
                                                     CostType cost) {
   if(src == NULL || dst == NULL)
      return false;

   typename EdgePairList::iterator iter;
   iter = src->resAdjList.find(dst);

   if(iter != src->resAdjList.end()) {
      iter->second += cost;
      return true;
   } else {
      return false;
   }
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::addCostToFlowEdge(VertPtr src, VertPtr dst,
                                                      CostType cost) {
   if(src == NULL || dst == NULL)
      return false;

   typename EdgePairList::iterator iter;
   iter = src->flowAdjList.find(dst);

   if(iter != src->flowAdjList.end()) {
      iter->second += cost;
      return true;
   } else {
      return addCostToFlowEdge(dst, src, cost);
   }
}

#endif
