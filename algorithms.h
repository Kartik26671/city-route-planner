#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"

void bfsShortestPath(const char* start, const char* end);
void dijkstraShortestPath(const char* start, const char* end);
void displayRoute(int path[], int pathLen);
void showRouteSummary(const int path[], int pathLen);


#endif
