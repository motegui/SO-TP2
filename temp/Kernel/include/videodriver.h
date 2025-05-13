#ifndef _VIDEODRIVER_H
#define _VIDEODRIVER_H

#include <colors.h>
#include <stdint.h>

void putPixel(char r, char g, char b, int x, int y);

void drawWhiteLine();

void drawRect(int x, int y, int width, int height, int color);

char getPixel(int x, int y);

char isSpaceEmpty(int x, int y);

void printChar(char c, int x, int y, Color color);

void printStringPlace(char * string, int x, int y, Color color);

void printString(char * string);

void printStringN(char * string, uint64_t length);

void printLn(char * string);

void moveOneLineUp();

void printStringColor(char * string, Color color);

void printStringNColor(char * buffer, uint64_t length, Color color);

void moveCursor();

void eraseCursor();

void clearScreen();

uint16_t getHeight();

uint16_t getWidth();

void toggleCursor();

void drawImage(const unsigned long int * image, int width, int height);

#endif