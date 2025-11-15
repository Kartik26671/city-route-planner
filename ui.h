#ifndef UI_H
#define UI_H

void clearScreen();
void slowPrint(const char *text, int delay);
void printHeader();
void printMainMenu();
void printSuccess(const char *msg);
void printError(const char *msg);
void printWarning(const char *msg);
void printLoading(const char *msg);
void printRouteHeader();
void printFooter();

#endif
