#include "image.h"

#include <SDL.h>
#include <SDL_image.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358979323846 // If M_PI unavailable

// Creates blank image with white pixel -> returns NULL on failure
Image *image_create(int width, int height)
{
    if (width <= 0 || height <= 0 || (size_t)width > SIZE_MAX / (size_t)height)
        return NULL;

    Image *image = malloc(sizeof(*image));
    if (!image)
        return NULL;

    image->pixels = malloc((size_t)width * (size_t)height);
    if (!image->pixels)
    {
        free(image);
        return NULL;
    }
    
    image->width = width;
    image->height = height;
    memset(image->pixels, 255, (size_t)width * (size_t)height);
    return image;
}

// Frees image and pixels
void image_free(Image *image)
{
    if (image)
    {
        free(image->pixels);
        free(image);
    }
}


// Creates a copy of the image -> returns NULL on failure
Image *image_clone(const Image *image)
{
    Image *copy = image_create(image->width, image->height);
    if (!copy) return NULL;
    memcpy(copy->pixels, 
    image->pixels,
    (size_t)image->width * (size_t)image->height);
    return copy;
}

// Loads an image from a file and converts it to grayscale -> returns NULL on failure
Image *image_load(const char *path)
{
    SDL_Surface *loaded = IMG_Load(path);
    if (!loaded)
        return NULL;

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(
        loaded,
        SDL_PIXELFORMAT_RGBA32,
        0);
    SDL_FreeSurface(loaded);
    if (!surface)
        return NULL;

    Image *image = image_create(surface->w, surface->h);
    if (!image || SDL_LockSurface(surface) != 0)
    {
        image_free(image);
        SDL_FreeSurface(surface);
        return NULL;
    }
    for (int y = 0; y < image->height; y++)
    {
        Uint8 *row = (Uint8 *)surface->pixels + y * surface->pitch;
        for (int x = 0; x < image->width; x++)
        {
            Uint8 r, g, b;
            Uint32 pixel;
            memcpy(&pixel, row+x*4, sizeof(pixel));
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            int gray = (300*r+585*g+115*b) / 1000;
            // Eyes are more sensitive to green than red or blue
            image->pixels[(size_t)y*image->width+x] =
                (unsigned char)gray;
        }
    }
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);
    return image;
}

// Saves an image to a file in PNG format -> returns 1 on success and 0 on failure
int image_save(const Image *image, const char *path)
{
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        image->width,
        image->height,
        32,
        SDL_PIXELFORMAT_RGBA32);
    if (!surface)
        return 0;

    if (SDL_LockSurface(surface) != 0)
    {
        SDL_FreeSurface(surface);
        return 0;
    }

    for (int y = 0; y < image->height; y++)
    {
        Uint8 *row = (Uint8 *)surface->pixels + y * surface->pitch;
        for (int x = 0; x < image->width; x++)
        {
            Uint8 v = image->pixels[(size_t)y*image->width+x];
            Uint32 pixel = SDL_MapRGBA(surface->format, v, v, v, 255);
            memcpy(row+x*4, &pixel, sizeof(pixel));
        }
    }
    SDL_UnlockSurface(surface);
    int result = IMG_SavePNG(surface, path) == 0;
    SDL_FreeSurface(surface);
    return result;
}

// Applies a threshold to the image -> (pixels <= threshold become black and others become white)
void image_threshold(Image *image, int threshold)
{
    size_t count = (size_t)image->width * image->height;
    for (size_t i = 0; i < count; i++)
        image->pixels[i] = image->pixels[i] <= threshold ? 0 : 255;
}

int image_otsu(const Image *image)
{
    size_t hist[256] = {0};
    size_t total = (size_t)image->width * image->height;
    double sum = 0;

    for (size_t i = 0; i < total; i++)
        hist[image->pixels[i]]++; // Grayscale Image -> 1 value per pixel
    for (int i = 0; i < 256; i++)
        sum += (double)i*hist[i];

    size_t left_count = 0;
    double left_sum = 0;
    double best = -1;
    int threshold = 127;

    for (int i = 0; i < 255; i++)
    {
        left_count += hist[i];
        left_sum += (double)i*hist[i];
        if (!left_count || left_count == total) continue;
        double left_mean = left_sum/left_count;
        double right_mean = (sum-left_sum)/(total-left_count);
        double difference = left_mean-right_mean;
        double score = (double)left_count* 
                        (total-left_count)*
                        difference*difference;
        if (score > best)
        {
            best = score;
            threshold = i;
        }
    }
    return threshold;
}

// Rotates the image by a given angle in degrees -> returns NULL on failure
Image *image_rotate(const Image *image, double degrees)
{
    double angle = degrees * PI / 180.0;
    double c = cos(angle), s = sin(angle);
    double new_width = ceil(fabs(image->width*c) +
                            fabs(image->height*s)-1e-9);
    double new_height = ceil(fabs(image->height*c) +
                             fabs(image->width*s)-1e-9);
    if (new_width > INT_MAX || new_height>INT_MAX)
        return NULL;

    Image *output = image_create((int)new_width, (int)new_height);
    if (!output) return NULL;

    double old_x = (image->width-1) /2.0;
    double old_y = (image->height-1) /2.0;
    double new_x = (output->width-1) /2.0;
    double new_y = (output->height-1) /2.0;
    for (int y = 0; y < output->height; y++)
    {
        for (int x = 0; x < output->width; x++)
        {
            double dx = x - new_x, dy = y - new_y;
            int sx = (int)lround(c * dx + s * dy + old_x);
            int sy = (int)lround(-s * dx + c * dy + old_y);
            if (sx >= 0 && sx < image->width && sy >= 0 &&
                sy < image->height)
                {
                output->pixels[(size_t)y * output->width + x] =
                image->pixels[(size_t)sy * image->width + sx];
                }
        }
    }
    return output;
}

// Finds the best rotation angle for the image -> returns it in degrees
static double projection_score(const Image *image, double degrees)
{
    double radians = degrees * PI / 180.0;
    double c = cos(radians), s = sin(radians);
    int rows = (int)ceil(hypot(image->width, image->height)) + 3;

    int *counts = calloc((size_t)rows, sizeof(*counts));
    if (!counts) return -1;

    double cx = (image->width-1) /2.0;
    double cy = (image->height-1) /2.0;

    for (int y = 0; y < image->height; y++)
        for (int x = 0; x < image->width; x++)
            if (image->pixels[(size_t)y * image->width + x] == 0)
            {
                int row = (int)lround(s*(x-cx)+c*(y-cy)+rows/2.0);
                if (row >= 0 && row<rows)
                    counts[row]++;
            }

    double score = 0;
    for (int i = 1; i<rows; i++)
    {
        double delta = (double)counts[i] - counts[i-1];
        score += delta*delta;
    }

    free(counts);
    return score;
}

double image_find_rotation(const Image *image, double max_angle,
                           double step)
{
    double best_score = -1, best_angle = 0;
    int steps = (int)floor(2 *max_angle/step);

    for (int i = 0; i <= steps; i++)
    {
        double angle = -max_angle + i*step;
        double score = projection_score(image, angle);
        if (score>best_score)
        {
            best_score = score;
            best_angle = angle;
        }
    }
    return best_angle;
}

// Extracts a rectangular area from the image and resizes it to 28x28 -> returns NULL on failure
Image *image_letter(const Image *image, int x, int y, int width, int height)
{
    if (x < 0 || y < 0 || width <= 0 || height <= 0 ||
        x > image->width - width || y > image->height - height)
        return NULL;

    int left = x + width, right = x-1, top = y+height, bottom = y-1;
    for (int py = y; py < y+height; py++)
        for (int px = x; px < x+width; px++)
            if (image->pixels[(size_t)py * image->width+px] == 0)
            {
                if (px<left) left = px;
                if (px>right) right = px;
                if (py<top) top = py;
                if (py>bottom) bottom = py;
            }
    if (right < left) return NULL;

    int w = right-left+1, h = bottom-top+1;
    double scale = fmin(20.0 / w, 20.0 / h);
    int nw = (int)fmax(1, round(w * scale));
    int nh = (int)fmax(1, round(h * scale));

    Image *output = image_create(28, 28);
    if (!output) return NULL;

    int ox = (28-nw)/2, oy = (28-nh)/2;
    for (int py = 0; py < nh; py++)
        for (int px = 0; px < nw; px++)
        {
            int sx = left+px * w/nw;
            int sy = top+py * h/nh;
            output->pixels[(size_t)(oy + py)*28+ox+px] =
                image->pixels[(size_t)sy*image->width+sx];
        }
    return output;
}
