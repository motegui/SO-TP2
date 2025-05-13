#ifndef COLORS_H
#define COLORS_H

typedef struct Color {
    char r, g, b;
} Color;

static Color RED = {0xFF, 0, 0};
static Color GREEN = {0, 0xFF, 0};
static Color BLUE = {0, 0, 0xFF};
static Color WHITE = {0xFF, 0xFF, 0xFF};
static Color YELLOW = {0xFF, 0xFF, 0};

#endif