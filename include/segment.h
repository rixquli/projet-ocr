#ifndef SEGMENT_H
#define SEGMENT_H

#include "image.h"

typedef struct
{
    int x;
    int y;
    int width;
    int height;
} Rect;

typedef struct
{
    int rows;
    int columns;
    Image ***letters;
} LetterGrid;

LetterGrid *segment_grid(const Image *image);
int letter_grid_save(const LetterGrid *grid, const char *directory);
void letter_grid_free(LetterGrid *grid);
int segment_components(const Image *image, Rect area, const char *directory);

#endif
