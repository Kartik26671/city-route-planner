#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "graph.h"        
#include "ui.h"           // <-- for UI output (route header etc.)
#include "algorithms.h"   


#define INF INT_MAX

// ---------- BFS Shortest Path ----------
void bfsShortestPath(const char* start, const char* end) {
    int s = getCityIndex(start);
    int e = getCityIndex(end);
    if (s == -1 || e == -1) {
        printf("Invalid city name.\n");
        return;
    }

    int visited[MAX] = {0}, parent[MAX];
    for (int i = 0; i < MAX; i++) parent[i] = -1;

    int queue[MAX], front = 0, rear = 0;
    visited[s] = 1;
    queue[rear++] = s;

    while (front < rear) {
        int curr = queue[front++];
        Node* temp = adjList[curr];
        while (temp) {
            int next = temp->cityIndex;
            if (!visited[next]) {
                visited[next] = 1;
                parent[next] = curr;
                queue[rear++] = next;
            }
            temp = temp->next;
        }
    }

    if (!visited[e]) {
        printf("No path found from %s to %s.\n", start, end);
        return;
    }

    int path[MAX], pathLen = 0;
    for (int v = e; v != -1; v = parent[v]) path[pathLen++] = v;

    printf("\nShortest Path (Least Hops - BFS):\n");
    displayRoute(path, pathLen);
    showRouteSummary(path, pathLen);
}

// ---------- Dijkstra Shortest Path ----------
void dijkstraShortestPath(const char* start, const char* end) {
    int s = getCityIndex(start);
    int e = getCityIndex(end);
    if (s == -1 || e == -1) {
        printf("Invalid city name.\n");
        return;
    }

    int dist[MAX], visited[MAX] = {0}, parent[MAX];
    for (int i = 0; i < cityCount; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[s] = 0;

    for (int i = 0; i < cityCount - 1; i++) {
        int minDist = INF, u = -1;
        for (int j = 0; j < cityCount; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        if (u == -1) break;
        visited[u] = 1;

        Node* temp = adjList[u];
        while (temp) {
            int v = temp->cityIndex;
            if (!visited[v] && dist[u] + temp->distance < dist[v]) {
                dist[v] = dist[u] + temp->distance;
                parent[v] = u;
            }
            temp = temp->next;
        }
    }

    if (dist[e] == INF) {
        printf("No route from %s to %s.\n", start, end);
        return;
    }

    int path[MAX], pathLen = 0;
    for (int v = e; v != -1; v = parent[v]) path[pathLen++] = v;

    printf("\nShortest Path (Dijkstra - Weighted):\n");
    displayRoute(path, pathLen);
    showRouteSummary(path, pathLen);
}

// ---------- Unified Route Display ----------
void displayRoute(int path[], int pathLen) {
    if (pathLen <= 0) {
        printf("No route available.\n");
        return;
    }

    printf("Route: ");
    for (int i = pathLen - 1; i >= 0; i--) {
        printf("%s", cities[path[i]].name);
        if (i > 0) printf(" ---> ");
    }
    printf("\n");

    int totalDist = 0;
    for (int i = pathLen - 1; i > 0; i--) {
        Node* temp = adjList[path[i]];
        while (temp) {
            if (temp->cityIndex == path[i - 1]) {
                totalDist += temp->distance;
                break;
            }
            temp = temp->next;
        }
    }

    printf("Total Distance: %d km\n", totalDist);
    printf("=============================\n");
}



//route summary

void showRouteSummary(const int path[], int pathLen) {
    if (pathLen <= 0) {
        printf("No route to display.\n");
        return;
    }

    int totalDistance = 0;
    int totalLegs = pathLen - 1;

    for (int i = 0; i < pathLen - 1; i++) {
        Node* temp = adjList[path[i]];
        while (temp) {
            if (temp->cityIndex == path[i + 1]) {
                totalDistance += temp->distance;
                break;
            }
            temp = temp->next;
        }
    }

    double avgDistance = (totalLegs > 0) ? (double)totalDistance / totalLegs : 0.0;

    printf("\n========= ROUTE SUMMARY =========\n");
    printf("Total Stops (including start & end): %d\n", pathLen);
    printf("Number of Legs (roads travelled): %d\n", totalLegs);
    printf("Total Distance: %d km\n", totalDistance);
    printf("Average Distance per Leg: %.2f km\n", avgDistance);
    printf("=================================\n");
}
