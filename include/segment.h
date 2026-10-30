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

int segment_grid(const Image *image, const char *directory);
int segment_components(const Image *image, Rect area, const char *directory);

#endif
