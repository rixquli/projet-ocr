#ifndef IMAGE_H
#define IMAGE_H

#include "stb_image.h"
#include "stb_image_write.h"

// int *imtoar(char *filepath); // Image of letter to Array
void imtoletters(char *filename); // Creates PNGs of Letters in a given Image
char *artoim(int *array); // Array to Image
void im_to_grey(char *buffer, const char *filename); // Image to Grey Shades
// char *im_to_baw(char *original, int threshold); // Image to Black and White
void rotate(char *filename); // Rotate a given Image in a NEW Image


#endif // IMAGE_H