#include <stdio.h>
#include <string.h>
#include "graph.h"

City cities[MAX];
int cityCount = 0;

int getCityIndex(const char* name) {
    for (int i = 0; i < cityCount; i++) {
        if (strcmp(cities[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

void addCity(const char* name) {
    if (getCityIndex(name) != -1) {
        printf("City '%s' already exists.\n", name);
        return;
    }
    if (cityCount >= MAX) {
        printf("City limit reached.\n");
        return;
    }
    strcpy(cities[cityCount].name, name);
    cityCount++;
    printf("City '%s' added successfully.\n", name);
}
