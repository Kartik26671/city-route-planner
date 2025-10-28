#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

City cities[MAX];
int cityCount = 0;
Node* adjList[MAX] = { NULL };

/* ------------------ basic helpers ------------------ */

int getCityIndex(const char* name) {
    for (int i = 0; i < cityCount; i++) {
        if (strcmp(cities[i].name, name) == 0) return i;
    }
    return -1;
}

/* Add a city */
void addCity(const char* name) {
    if (getCityIndex(name) != -1) {
        printf("City '%s' already exists.\n", name);
        return;
    }
    if (cityCount >= MAX) {
        printf("Cannot add more cities (limit reached).\n");
        return;
    }
    strncpy(cities[cityCount].name, name, NAME_LEN-1);
    cities[cityCount].name[NAME_LEN-1] = '\0';
    adjList[cityCount] = NULL;
    cityCount++;
    printf("City '%s' added successfully.\n", name);
}

/* Add a two-way road */
void addRoad(const char* city1, const char* city2, int distance) {
    int a = getCityIndex(city1);
    int b = getCityIndex(city2);
    if (a == -1 || b == -1) {
        printf("Error: One or both cities not found.\n");
        return;
    }
    if (a == b) {
        printf("Cannot add road from a city to itself.\n");
        return;
    }

    /* Prevent duplicate edge a->b */
    Node* t = adjList[a];
    while (t) {
        if (t->cityIndex == b) {
            printf("Road already exists between '%s' and '%s'.\n", city1, city2);
            return;
        }
        t = t->next;
    }

    Node* n1 = (Node*)malloc(sizeof(Node));
    n1->cityIndex = b;
    n1->distance = distance;
    n1->next = adjList[a];
    adjList[a] = n1;

    Node* n2 = (Node*)malloc(sizeof(Node));
    n2->cityIndex = a;
    n2->distance = distance;
    n2->next = adjList[b];
    adjList[b] = n2;

    printf("Road added between '%s' and '%s' (%d km)\n", city1, city2, distance);
}

/* Display all connections */
void displayGraph() {
    printf("\n===== CITY MAP =====\n");
    for (int i = 0; i < cityCount; i++) {
        printf("%s -> ", cities[i].name);
        Node* t = adjList[i];
        if (!t) printf("(no connections)");
        while (t) {
            printf("%s (%dkm)", cities[t->cityIndex].name, t->distance);
            t = t->next;
            if (t) printf(", ");
        }
        printf("\n");
    }
    printf("====================\n");
}

/* ------------------ deletion helpers ------------------ */

/* delete a single directed edge u->v from adjacency list */
static void deleteDirectedEdgeIndex(int u, int v) {
    Node* cur = adjList[u];
    Node* prev = NULL;
    while (cur) {
        if (cur->cityIndex == v) {
            if (prev) prev->next = cur->next;
            else adjList[u] = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

/* delete road both directions */
void deleteRoad(const char* city1, const char* city2) {
    int a = getCityIndex(city1);
    int b = getCityIndex(city2);
    if (a == -1 || b == -1) {
        printf("Error: One or both cities not found.\n");
        return;
    }
    deleteDirectedEdgeIndex(a, b);
    deleteDirectedEdgeIndex(b, a);
    printf("Road between '%s' and '%s' deleted (if existed).\n", city1, city2);
}

/* delete a city (and all edges). This will shift arrays to keep compact indices. */
void deleteCity(const char* name) {
    int idx = getCityIndex(name);
    if (idx == -1) {
        printf("City '%s' not found.\n", name);
        return;
    }

    /* free adjacency list of idx */
    Node* cur = adjList[idx];
    while (cur) {
        Node* next = cur->next;
        free(cur);
        cur = next;
    }

    /* remove incoming edges to idx from other lists */
    for (int i = 0; i < cityCount; i++) {
        if (i == idx) continue;
        deleteDirectedEdgeIndex(i, idx);
    }

    /* shift cities and adjList left to fill removed index */
    for (int i = idx; i < cityCount - 1; i++) {
        /* free current adjList[i] if any - we will reassign pointer */
        adjList[i] = adjList[i+1];
        cities[i] = cities[i+1];
    }
    /* last slot becomes NULL */
    adjList[cityCount - 1] = NULL;
    cityCount--;

    /* BUT after shifting, all adjacency nodes containing indices > idx must be decremented by 1 */
    for (int i = 0; i < cityCount; i++) {
        Node* p = adjList[i];
        while (p) {
            if (p->cityIndex > idx) p->cityIndex--;
            p = p->next;
        }
    }

    printf("City '%s' and its roads deleted successfully.\n", name);
}

/* ------------------ utility functions ------------------ */

/* get edge distance from u to v, -1 if not present */
int getEdgeDistance(int u, int v) {
    Node* t = adjList[u];
    while (t) {
        if (t->cityIndex == v) return t->distance;
        t = t->next;
    }
    return -1;
}

/* display adjacency matrix */
void displayAdjacencyMatrix(void) {
    int n = cityCount;
    if (n == 0) {
        printf("Graph is empty.\n");
        return;
    }

    /* prepare matrix */
    int matrix[MAX][MAX];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = 0;

    for (int i = 0; i < n; i++) {
        Node* t = adjList[i];
        while (t) {
            matrix[i][t->cityIndex] = t->distance;
            t = t->next;
        }
    }

    /* print header */
    printf("\nAdjacency Matrix (0 = no direct road):\n      ");
    for (int j = 0; j < n; j++) printf("%10s", cities[j].name);
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%-6s", cities[i].name);
        for (int j = 0; j < n; j++) {
            printf("%10d", matrix[i][j]);
        }
        printf("\n");
    }
}

/* graph stats: totalCities, totalRoads (undirected count), avgDistance, mostConnectedIndex */
void getGraphStats(int* totalCities, int* totalRoads, double* avgDistance, int* mostConnectedIndex) {
    *totalCities = cityCount;
    int roads = 0;
    int sumDist = 0;
    int mostConn = -1;
    int maxConn = -1;

    for (int i = 0; i < cityCount; i++) {
        int deg = 0;
        Node* t = adjList[i];
        while (t) {
            deg++;
            sumDist += t->distance;
            t = t->next;
        }
        if (deg > maxConn) {
            maxConn = deg;
            mostConn = i;
        }
        roads += deg;
    }
    /* each undirected road counted twice in adjacency lists */
    *totalRoads = roads / 2;
    if (roads > 0) *avgDistance = (double)sumDist / roads;
    else *avgDistance = 0.0;
    *mostConnectedIndex = mostConn;
}

/* ------------------ persistence (simple text) ------------------ */

void saveGraphToFile(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to open %s for writing.\n", filename);
        return;
    }

    fprintf(fp, "%d\n", cityCount);
    for (int i = 0; i < cityCount; i++) {
        fprintf(fp, "%s\n", cities[i].name);
    }

    /* write edges once (i < neighbor) */
    for (int i = 0; i < cityCount; i++) {
        Node* t = adjList[i];
        while (t) {
            if (i < t->cityIndex)
                fprintf(fp, "%d %d %d\n", i, t->cityIndex, t->distance);
            t = t->next;
        }
    }

    fclose(fp);
    printf("Graph saved to file '%s'.\n", filename);
}

void loadGraphFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        /* no file is fine */
        return;
    }

    /* clear current graph first */
    for (int i = 0; i < cityCount; i++) {
        Node* cur = adjList[i];
        while (cur) {
            Node* nx = cur->next;
            free(cur);
            cur = nx;
        }
        adjList[i] = NULL;
    }
    cityCount = 0;

    int n;
    if (fscanf(fp, "%d\n", &n) != 1) { fclose(fp); return; }
    char line[256];
    for (int i = 0; i < n && i < MAX; i++) {
        if (!fgets(line, sizeof(line), fp)) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(cities[i].name, line, NAME_LEN-1);
        cities[i].name[NAME_LEN-1] = '\0';
        adjList[i] = NULL;
        cityCount++;
    }

    int a,b,d;
    while (fscanf(fp, "%d %d %d", &a, &b, &d) == 3) {
        if (a < 0 || a >= cityCount || b < 0 || b >= cityCount) continue;
        Node* n1 = (Node*)malloc(sizeof(Node));
        n1->cityIndex = b; n1->distance = d; n1->next = adjList[a]; adjList[a] = n1;
        Node* n2 = (Node*)malloc(sizeof(Node));
        n2->cityIndex = a; n2->distance = d; n2->next = adjList[b]; adjList[b] = n2;
    }

    fclose(fp);
}

/* ------------------ route report saving ------------------ */

/* reconstruct path using parent[] and save to file */
void saveRouteReport(const char* method, int parent[], int endIdx, int totalDistance) {
    if (endIdx < 0 || endIdx >= cityCount) return;
    char pathNames[MAX][NAME_LEN];
    int pathLen = 0;
    for (int v = endIdx; v != -1; v = parent[v]) {
        strncpy(pathNames[pathLen], cities[v].name, NAME_LEN-1);
        pathNames[pathLen][NAME_LEN-1] = '\0';
        pathLen++;
    }

    /* reverse path to get start->...->end */
    FILE* fp = fopen("last_route.txt", "w");
    if (!fp) return;

    fprintf(fp, "Route Report\n");
    fprintf(fp, "Method: %s\n", method);
    fprintf(fp, "Path: ");
    for (int i = pathLen - 1; i >= 0; i--) {
        fprintf(fp, "%s", pathNames[i]);
        if (i > 0) fprintf(fp, " -> ");
    }
    fprintf(fp, "\nTotal Distance: %d km\n", totalDistance);
    fclose(fp);
    printf("Route saved to 'last_route.txt'.\n");
}

//helper
void suggestCity(const char* partial) {
    int found = 0;
    printf("Suggestions: ");
    for (int i = 0; i < cityCount; i++) {
        if (strncasecmp(cities[i].name, partial, strlen(partial)) == 0) {
            printf("%s ", cities[i].name);
            found = 1;
        }
    }
    if (!found) printf("No matches found.");
    printf("\n");
}

