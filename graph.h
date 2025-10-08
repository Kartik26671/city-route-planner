#ifndef GRAPH_H
#define GRAPH_H

#define MAX 100

typedef struct City {
    char name[30];
    // For now, no roads; that comes next phase
} City;

extern City cities[MAX];
extern int cityCount;

int getCityIndex(const char* name);
void addCity(const char* name);

#endif
