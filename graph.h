#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

#define MAX 100
#define NAME_LEN 30

// City structure
typedef struct City {
    char name[NAME_LEN];
} City;

// Adjacency list node (edge)
typedef struct Node {
    int cityIndex;
    int distance;
    struct Node* next;
} Node;

/* globals */
extern City cities[MAX];
extern int cityCount;
extern Node* adjList[MAX];

/* graph functions */
int getCityIndex(const char* name);
void addCity(const char* name);
void addRoad(const char* city1, const char* city2, int distance);
void displayGraph();

/* deletion */
void deleteRoad(const char* city1, const char* city2);
void deleteCity(const char* name);

/* utilities */
int getEdgeDistance(int u, int v);
void displayAdjacencyMatrix(void);
void getGraphStats(int* totalCities, int* totalRoads, double* avgDistance, int* mostConnectedIndex);

/* persistence */
void saveGraphToFile(const char* filename);
void loadGraphFromFile(const char* filename);

/* route report */
void saveRouteReport(const char* method, int parent[], int endIdx, int totalDistance);

//helper

void suggestCity(const char* partial);

#endif
