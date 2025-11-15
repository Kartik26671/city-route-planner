#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "graph.h"
#include "algorithms.h"

#define SAVE_FILE "city_data.txt"

/* ---------------------------------------------------------
   Safe line input (removes newline, avoids crashes)
--------------------------------------------------------- */
void readLine(char* buf, int size) {
    if (!fgets(buf, size, stdin)) { 
        buf[0] = '\0'; 
        return; 
    }
    buf[strcspn(buf, "\n")] = '\0'; // remove newline
}

/* ---------------------------------------------------------
   MAIN FUNCTION
--------------------------------------------------------- */
int main() {

    int choice;
    char city1[NAME_LEN], city2[NAME_LEN];
    int dist;

    clearScreen();
    printHeader();

    /* -----------------------------------------------------
       Load previous graph data from file (if it exists)
    ----------------------------------------------------- */
    loadGraphFromFile(SAVE_FILE);
    printSuccess("Previous data loaded successfully (if file existed).");

    /* -----------------------------------------------------
       MAIN MENU LOOP
    ----------------------------------------------------- */
    while (1) {
        printMainMenu();


        

        /* Validate numeric input */
        if (scanf("%d", &choice) != 1) {
            printError("Invalid input! Enter a number.");
            while (getchar() != '\n'); // clear buffer
            continue;
        }
        getchar(); // remove leftover newline

        /* -------------------------------------------------
           Handle menu choices
        ------------------------------------------------- */
        switch (choice) {

            case 1: // Add City
                printf("Enter city name: ");
                readLine(city1, sizeof(city1));

                addCity(city1);
                printSuccess("City added successfully!");
                break;

            case 2: // Add Road
                printf("Enter first city: ");
                readLine(city1, sizeof(city1));

                printf("Enter second city: ");
                readLine(city2, sizeof(city2));

                printf("Enter distance (km): ");
                if (scanf("%d", &dist) != 1) {
                    printError("Invalid distance value!");
                    while (getchar() != '\n');
                    break;
                }
                getchar(); // clear newline

                addRoad(city1, city2, dist);
                printSuccess("Road added successfully!");
                break;

            case 3: // Display graph (list)
                displayGraph();
                break;

            case 4: // Display adjacency matrix
                displayAdjacencyMatrix();
                break;

            case 5: { // Graph statistics
                int totalCities, totalRoads, mostIdx;
                double avgDist;

                getGraphStats(&totalCities, &totalRoads, &avgDist, &mostIdx);

                printf("\n========== GRAPH SUMMARY ==========\n");
                printf("Total Cities       : %d\n", totalCities);
                printf("Total Roads        : %d\n", totalRoads);
                printf("Average Distance   : %.2f km\n", avgDist);

                if (mostIdx >= 0)
                    printf("Most Connected City: %s\n", cities[mostIdx].name);

                printf("===================================\n");
                break;
            }

            case 6: // Delete Road
                printf("Enter first city: ");
                readLine(city1, sizeof(city1));

                printf("Enter second city: ");
                readLine(city2, sizeof(city2));

                deleteRoad(city1, city2);
                printWarning("Road deleted (if it existed).");
                break;

            case 7: // Delete City
                printf("Enter city name: ");
                readLine(city1, sizeof(city1));

                deleteCity(city1);
                printWarning("City deleted (if it existed).");
                break;

            case 8: // BFS shortest path
                printf("Start city: ");
                readLine(city1, sizeof(city1));
                suggestCity(city1);

                printf("Destination city: ");
                readLine(city2, sizeof(city2));
                suggestCity(city2);

                bfsShortestPath(city1, city2);
                break;

            case 9: // Dijkstra shortest path
                printf("Start city: ");
                readLine(city1, sizeof(city1));
                suggestCity(city1);

                printf("Destination city: ");
                readLine(city2, sizeof(city2));
                suggestCity(city2);

                dijkstraShortestPath(city1, city2);
                break;

            case 10: // Auto-suggest city names
                printf("Enter partial city name: ");
                readLine(city1, sizeof(city1));
                suggestCity(city1);
                break;

            case 11: // Save and Exit
                saveGraphToFile(SAVE_FILE);
                printSuccess("Data saved successfully. Goodbye!");
                return 0;

            default:
                printError("Invalid choice! Please select a valid option.");
        }

        printFooter(); // Pause + continue
    }

    return 0;
}
