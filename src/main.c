#include "../include/image.h"

int main()
{
    // Example usage of the functions
    const char *input_image = "../data/input.png";

    char *grey_image = im_to_grey(input_image);
    if (grey_image)
    {
        printf("Converted image to grey shades successfully.\n");
        free(grey_image);
    }
    else
    {
        printf("Failed to convert image to grey shades.\n");
    }

    int array[28 * 28] = { /* Fill with pixel values */ };
    char *output_image = artoim(array);
    if (output_image)
    {
        printf("Converted array to image successfully.\n");
        free(output_image);
    }
    else
    {
        printf("Failed to convert array to image.\n");
    }

    return 0;
}