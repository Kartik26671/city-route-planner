#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "algorithms.h"

#define SAVE_FILE "city_data.txt"

void showMenu() {
    printf("\n===== CITY ROUTE PLANNER =====\n");
    printf("1. Add City\n");
    printf("2. Add Road\n");
    printf("3. Display Graph\n");
    printf("4. Display Adjacency Matrix\n");
    printf("5. Graph Summary (Stats)\n");
    printf("6. Delete Road\n");
    printf("7. Delete City\n");
    printf("8. Find Shortest Path (BFS)\n");
    printf("9. Find Shortest Path (Dijkstra)\n");
    printf("10. Suggest City (Auto-complete)\n");
    printf("11. Save & Exit\n");
    printf("==============================\n");
    printf("Enter your choice: ");
}

void readLine(char* buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}

int main() {
    int choice;
    char s1[NAME_LEN], s2[NAME_LEN];
    int dist;

    /* auto-load existing data */
    loadGraphFromFile(SAVE_FILE);
    printf("Data loaded (if file existed).\n");

    while (1) {
        showMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            while (getchar() != '\n');
            continue;
        }
        getchar(); /* consume newline */

        switch (choice) {
            case 1:
                printf("Enter city name: ");
                readLine(s1, sizeof(s1));
                addCity(s1);
                break;

            case 2:
                printf("Enter first city: ");
                readLine(s1, sizeof(s1));
                printf("Enter second city: ");
                readLine(s2, sizeof(s2));
                printf("Enter distance (km): ");
                if (scanf("%d", &dist) != 1) { 
                    printf("Invalid distance.\n"); 
                    while (getchar()!='\n'); 
                    break; 
                }
                getchar();
                addRoad(s1, s2, dist);
                break;

            case 3:
                displayGraph();
                break;

            case 4:
                displayAdjacencyMatrix();
                break;

            case 5: {
                int totalCities, totalRoads, mostIdx;
                double avgDist;
                getGraphStats(&totalCities, &totalRoads, &avgDist, &mostIdx);
                printf("\n=== Graph Summary ===\n");
                printf("Total Cities: %d\n", totalCities);
                printf("Total Roads: %d\n", totalRoads);
                printf("Average road distance (per direction entry): %.2f km\n", avgDist);
                if (mostIdx >= 0) 
                    printf("Most connected city: %s\n", cities[mostIdx].name);
                printf("=====================\n");
                break;
            }

            case 6:
                printf("Enter first city of road to delete: ");
                readLine(s1, sizeof(s1));
                printf("Enter second city: ");
                readLine(s2, sizeof(s2));
                deleteRoad(s1, s2);
                break;

            case 7:
                printf("Enter city name to delete: ");
                readLine(s1, sizeof(s1));
                deleteCity(s1);
                break;

            case 8:
                printf("Enter start city: ");
                readLine(s1, sizeof(s1));
                suggestCity(s1); // show possible matches

                printf("Enter destination city: ");
                readLine(s2, sizeof(s2));
                suggestCity(s2); // correct variable now

                bfsShortestPath(s1, s2);
                break;

            case 9:
                printf("Enter start city: ");
                readLine(s1, sizeof(s1));
                suggestCity(s1);

                printf("Enter destination city: ");
                readLine(s2, sizeof(s2));
                suggestCity(s2);

                dijkstraShortestPath(s1, s2);
                break;

            case 10:
                printf("Enter partial city name: ");
                readLine(s1, sizeof(s1));
                suggestCity(s1);
                break;

            case 11:
                saveGraphToFile(SAVE_FILE);
                printf("Data saved. Exiting.\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
