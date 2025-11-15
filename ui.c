#include <windows.h> // For Sleep()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "graph.h"

// ANSI color codes
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"

extern int cityCount;
extern City cities[];


// Clear terminal screen
void clearScreen()
{
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

// Typing animation effect
void slowPrint(const char *text, int delay)
{
  while (*text)
  {
    putchar(*text++);
    fflush(stdout);
    Sleep(delay); // delay in milliseconds
  }
}

// Top header for every menu page
void printHeader()
{
  clearScreen();
  printf(CYAN "==============================================\n" RESET);
  printf(GREEN "        CITY ROUTE PLANNER & NAVIGATION\n" RESET);
  printf(CYAN "==============================================\n\n" RESET);
}




void printMainMenu()
{
    printHeader();

    // Access global variables directly
    printf(CYAN "Current Cities (%d): " RESET, cityCount);

    if (cityCount == 0)
    {
        printf("No cities added yet.\n\n");
    }
    else
    {
        for (int i = 0; i < cityCount; i++)
        {
            printf("%s", cities[i].name);
            if (i < cityCount - 1)
                printf(", ");
        }
        printf("\n\n");
    }

    // Print the menu options
    printf(YELLOW "1. Add City\n" RESET);
    printf(YELLOW "2. Add Road\n" RESET);
    printf(YELLOW "3. Display Graph\n" RESET);
    printf(YELLOW "4. Display Adjacency Matrix\n" RESET);
    printf(YELLOW "5. Graph Summary (Stats)\n" RESET);
    printf(YELLOW "6. Delete Road\n" RESET);
    printf(YELLOW "7. Delete City\n" RESET);
    printf(YELLOW "8. BFS Shortest Path\n" RESET);
    printf(YELLOW "9. Dijkstra Shortest Path\n" RESET);
    printf(YELLOW "10. Suggest City\n" RESET);
    printf(RED    "11. Save & Exit\n\n" RESET);

    printf(CYAN "Enter your choice: " RESET);
}


// Success message (green checkmark)
void printSuccess(const char *msg)
{
  printf(GREEN "\n %s\n" RESET, msg);
}

// Error message (red cross)
void printError(const char *msg)
{
  printf(RED "\n %s\n" RESET, msg);
}

// Warning message (yellow triangle)
void printWarning(const char *msg)
{
  printf(YELLOW "\n %s\n" RESET, msg);
}

// Loading animation ("Processing...")
void printLoading(const char *msg)
{
  printf(BLUE "%s" RESET, msg);
  fflush(stdout);

  for (int i = 0; i < 3; i++)
  {
    printf(".");
    fflush(stdout);
    Sleep(300); // 300 ms
  }
  printf("\n");
}

// Route heading for path output
void printRouteHeader()
{
  printf(MAGENTA "\n======= ROUTE INFORMATION =======\n\n" RESET);
}

// Footer pause
void printFooter()
{
  printf(CYAN "\n==============================================\n" RESET);
  printf(BLUE "Press Enter to continue..." RESET);
  getchar(); // Wait for user
}
