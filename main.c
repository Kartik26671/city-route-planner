#include <stdio.h>
#include <string.h>
#include "graph.h"

int main() {
    char cityName[30];
    
    printf("Add cities to the route planner:\n");
    
    for (int i = 0; i < 3; i++) {  // Just testing 3 cities
        printf("Enter city name: ");
        fgets(cityName, sizeof(cityName), stdin);
        cityName[strcspn(cityName, "\n")] = 0;  // Remove newline
        
        addCity(cityName);
    }
    
    printf("\nCurrent cities added:\n");
    for (int i = 0; i < cityCount; i++) {
        printf("%d. %s\n", i + 1, cities[i].name);
    }
    
    return 0;
}
