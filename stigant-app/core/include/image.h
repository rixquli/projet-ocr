#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>

// width: left to right / height: top to bottom
typedef struct
{
    int width;
    int height;
    unsigned char *pixels;
} Image;

Image *image_create(int width, int height);
void image_free(Image *image);
Image *image_load(const char *path);
int image_save(const Image *image, const char *path);
Image *image_clone(const Image *image);
void image_threshold(Image *image, int threshold);
int image_otsu(const Image *image);
Image *image_rotate(const Image *image, double degrees);
double image_find_rotation(const Image *image, double max_angle, double step);
Image *image_letter(const Image *image, int x, int y, int width, int height);

#endif
