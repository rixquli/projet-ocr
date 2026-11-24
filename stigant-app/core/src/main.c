#include "image.h"
#include "segment.h"

#include <SDL.h>
#include <SDL_image.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_int(const char *text, int *value)
{
    char *end;
    errno = 0;
    long number = strtol(text, &end, 10);
    if (errno || end == text || *end || number < INT_MIN || number > INT_MAX)
        return 0;
    *value = (int)number;
    return 1;
}

static int save_in(const Image *image, const char *directory, const char *name)
{
    char path[1024];
    int length = snprintf(path, sizeof(path), "%s/%s", directory, name);
    if (length <= 0 || (size_t)length >= sizeof(path) ||
        !image_save(image, path))
    {
        fprintf(stderr, "Cannot save %s: %s\n", name, IMG_GetError());
        return 0;
    }
    return 1;
}

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s INPUT OUTPUT_DIR [options]\n", program);
    fprintf(stderr, "  --threshold 0..255   Use a fixed threshold\n");
    fprintf(stderr, "  --rotate DEGREES     Rotate clockwise\n");
    fprintf(stderr, "  --auto               Estimate correction angle\n");
    fprintf(stderr, "  --grid               Extract a bordered grid\n");
    fprintf(stderr, "  --components X Y W H Extract components in a region\n");
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argv[0]);
        return 1;
    }
    int threshold = -1, automatic = 0, grid = 0, components = 0;
    double degrees = 0;
    Rect area = {0, 0, 0, 0};
    for (int i = 3; i < argc; i++)
    {
        if (!strcmp(argv[i], "--threshold") && i + 1 < argc)
        {
            if (!parse_int(argv[++i], &threshold) || threshold < 0 ||
                threshold > 255)
            {
                usage(argv[0]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--rotate") && i + 1 < argc)
        {
            char *end;
            errno = 0;
            degrees = strtod(argv[++i], &end);
            if (errno || end == argv[i] || *end || !isfinite(degrees) ||
                fabs(degrees) > 360)
            {
                usage(argv[0]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--auto"))
            automatic = 1;
        else if (!strcmp(argv[i], "--grid"))
            grid = 1;
        else if (!strcmp(argv[i], "--components") && i + 4 < argc)
        {
            if (!parse_int(argv[++i], &area.x) ||
                !parse_int(argv[++i], &area.y) ||
                !parse_int(argv[++i], &area.width) ||
                !parse_int(argv[++i], &area.height))
            {
                usage(argv[0]);
                return 1;
            }
            components = 1;
        }
        else
        {
            usage(argv[0]);
            return 1;
        }
    }
    if (SDL_Init(0) != 0 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "SDL initialization failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    Image *gray = image_load(argv[1]);
    if (!gray)
    {
        fprintf(stderr, "Cannot load %s: %s\n", argv[1], IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    int ok = save_in(gray, argv[2], "gray.png");
    if (threshold < 0)
        threshold = image_otsu(gray);
    printf("Threshold: %d\n", threshold);
    Image *binary = image_clone(gray);
    image_free(gray);
    if (!binary)
        ok = 0;
    if (ok)
    {
        image_threshold(binary, threshold);
        ok = save_in(binary, argv[2], "binary.png");
    }
    if (ok && automatic)
    {
        double coarse = image_find_rotation(binary, 30, 1);
        double best = image_find_rotation(binary, 30, 0.5);
        printf("Automatic correction: %.1f degrees (coarse %.1f)\n",
               best, coarse);
        degrees += best;
    }
    Image *rotated = NULL;
    if (ok)
    {
        rotated = image_rotate(binary, degrees);
        if (!rotated || !save_in(rotated, argv[2], "rotated.png"))
            ok = 0;
    }
    LetterGrid *letters = NULL;
    if (ok && grid)
    {
        letters = segment_grid(rotated);
        if (!letters)
            ok = 0;
        else
        {
            printf("Grid candidate: %d rows, %d columns\n",
                   letters->rows, letters->columns);
            if (!letter_grid_save(letters, argv[2]))
                ok = 0;
        }
    }
    if (ok && components && !segment_components(rotated, area, argv[2]))
        ok = 0;
    
    letter_grid_free(letters);
    image_free(rotated);
    image_free(binary);
    IMG_Quit();
    SDL_Quit();
    return ok ? 0 : 1;
}
