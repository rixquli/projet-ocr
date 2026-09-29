#include "../include/image.h"

void im_to_grey(char *buffer, const char *filename)
{
    // Makes an array of Grey Shades from a given Image
    int width, height, channels;

    unsigned char *img = stbi_load(
        filename,
        &width,
        &height,
        &channels,
        0);

    if (!img)
    {
        printf("Impossible d'ouvrir l'image : %s\n", stbi_failure_reason());
        return;
    }

    int i = 0;

    for (int y=0; (y<height); y++)
    {
        for (int x=0; (x<width); x++)
        {
            // int index = (y width + x) * channels;
            int index = (y * width + x) * channels;

            int r = img[index + 0];
            int g = img[index + 1];
            int b = img[index + 2];

            buffer[i] = (r + g + b) / 3;
            i++;

        }
    }

    stbi_image_free(img);
}

char *artoim(int *array)
{
    // Convert a given Array to an Image in PNG format
    int width = 28; // Set the width of the image
    int height = 28; // Set the height of the image
    int channels = 1; // Set the number of channels (1 for grayscale)

    unsigned char *img = (unsigned char *)malloc(width * height * channels);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * width + x;
            img[index] = (unsigned char)array[index];
        }
    }

    // Save the image as a PNG file
    stbi_write_png("output.png", width, height, channels, img, width * channels);

    free(img);

    return "output.png";
}
