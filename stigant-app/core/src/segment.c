#include "segment.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Finds the lines in the image by projecting the pixels onto the x or y axis
static int find_lines(const Image *image, int vertical, int *lines, int max)
{
    int length = vertical ? image->width : image->height;
    int other = vertical ? image->height : image->width;

    int *projection = calloc((size_t)length, sizeof(*projection));
    if (!projection) return -1;

    int peak = 0;
    for (int p = 0; p < length; p++)
    {
        for (int q = 0; q < other; q++)
        {
            int x = vertical ? p : q;
            int y = vertical ? q : p;
            if (image->pixels[(size_t)y * image->width + x] == 0)
                projection[p]++;
        }
        if (projection[p] > peak)
            peak = projection[p];
    }

    int limit = peak * 3 / 5;
    if (limit < 20) limit = 20;

    int count = 0;
    for (int p = 0; p < length; p++)
    {
        if (projection[p] < limit)
            continue;
        int start = p;
        while (p+1 < length && projection[p+1] >= limit)
            p++;
        if (count == max)
        {
            free(projection);
            return -1;
        }
        lines[count++] = (start+p)/2;
    }
    free(projection);
    return count;
}

// Saves a letter from the image to a file in PNG format -> returns 1 on success and 0 on failure
static int write_letter(const Image *image, Rect box, const char *directory,
                        const char *name)
{
    Image *letter = image_letter(image, box.x, box.y, box.width, box.height);
    if (!letter) return 0;

    char path[1024];
    int length = snprintf(path, sizeof(path), "%s/%s.png", directory, name);
    int ok = length > 0 && (size_t)length < sizeof(path) &&
             image_save(letter, path);
    image_free(letter);
    return ok;
}

// Frees the memory used by a LetterGrid
void letter_grid_free(LetterGrid *grid)
{
    if (!grid)
        return;
    if (grid->letters)
    {
        for (int row = 0; row < grid->rows; row++)
        {
            if (grid->letters[row])
            {
                for (int col = 0; col < grid->columns; col++)
                    image_free(grid->letters[row][col]);
                free(grid->letters[row]);
            }
        }
        free(grid->letters);
    }
    free(grid);
}

// Segments the image into a grid of letters -> returns NULL on failure
LetterGrid *segment_grid(const Image *image)
{
    int xs[512], ys[512];
    int nx = find_lines(image, 1, xs, 512);
    int ny = find_lines(image, 0, ys, 512);
    if (nx < 6 || ny < 6)
    {
        fprintf(stderr, "Grid lines not found. Try another threshold.\n");
        return NULL;
    }

    LetterGrid *grid = calloc(1, sizeof(*grid));
    if (!grid)
        return NULL;
    grid->rows = ny - 1;
    grid->columns = nx - 1;
    grid->letters = calloc((size_t)grid->rows, sizeof(*grid->letters));
    if (!grid->letters)
    {
        letter_grid_free(grid);
        return NULL;
    }
    for (int row = 0; row < grid->rows; row++)
    {
        grid->letters[row] = calloc((size_t)grid->columns,
                                    sizeof(*grid->letters[row]));
        if (!grid->letters[row])
        {
            letter_grid_free(grid);
            return NULL;
        }
        for (int col = 0; col < grid->columns; col++)
        {
            int margin_x = (xs[col + 1] - xs[col]) / 10;
            int margin_y = (ys[row + 1] - ys[row]) / 10;
            if (margin_x < 2) margin_x = 2;
            if (margin_y < 2) margin_y = 2;
            Rect box = {xs[col] + margin_x, ys[row] + margin_y,
                        xs[col + 1] - xs[col] - 2 * margin_x,
                        ys[row + 1] - ys[row] - 2 * margin_y};
            grid->letters[row][col] = image_letter(image, box.x, box.y,
                                                   box.width, box.height);
            if (!grid->letters[row][col])
            {
                fprintf(stderr, "Could not extract cell (%d, %d).\n",
                        row, col);
                letter_grid_free(grid);
                return NULL;
            }
        }
    }
    return grid;
}

// Saves the letters in the grid to files in PNG format -> returns 1 on success and 0 on failure
int letter_grid_save(const LetterGrid *grid, const char *directory)
{
    for (int row = 0; row < grid->rows; row++)
        for (int col = 0; col < grid->columns; col++)
        {
            char path[1024];
            int length = snprintf(path, sizeof(path), "%s/grid_%03d_%03d.png",
                                  directory, row, col);
            if (length < 0 || (size_t)length >= sizeof(path) ||
                !image_save(grid->letters[row][col], path))
            {
                fprintf(stderr, "Could not save cell (%d, %d).\n", row, col);
                return 0;
            }
        }
    return 1;
}

// Compares two rectangles for sorting by top-left corner
static int compare_rects(const void *a, const void *b)
{
    const Rect *left = a, *right = b;
    if (left->y != right->y)
        return left->y < right->y ? -1 : 1;
    return left->x < right->x ? -1 : left->x > right->x;
}

// Segments the image into connected components -> returns 1 on success and 0 on failure
int segment_components(const Image *image, Rect area, const char *directory)
{
    if (area.x < 0 || area.y < 0 || area.width <= 0 || area.height <= 0 ||
        area.x > image->width - area.width ||
        area.y > image->height - area.height)
        return 0;
    size_t count = (size_t)area.width * area.height;
    if (count > SIZE_MAX / sizeof(size_t) ||
        count > SIZE_MAX / sizeof(Rect))
        return 0;
    unsigned char *seen = calloc(count, 1);
    size_t *queue = malloc(count * sizeof(*queue));
    Rect *boxes = malloc(count * sizeof(*boxes));
    if (!seen || !queue || !boxes)
    {
        free(seen); free(queue); free(boxes);
        return 0;
    }
    size_t box_count = 0;
    for (size_t start = 0; start < count; start++)
    {
        int x = (int)(start % area.width), y = (int)(start / area.width);
        if (seen[start] || image->pixels[(size_t)(area.y+y) *
                                        image->width+area.x+x] != 0)
            continue;

        size_t head = 0, tail = 0;
        queue[tail++] = start;
        seen[start] = 1;

        int minx = x, maxx = x, miny = y, maxy = y;
        while (head < tail)
        {
            size_t current = queue[head++];
            int cx = (int)(current % area.width);
            int cy = (int)(current / area.width);
            if (cx<minx) minx = cx;
            if (cx>maxx) maxx = cx;
            if (cy<miny) miny = cy;
            if (cy>maxy) maxy = cy;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                {
                    int xx = cx + dx, yy = cy + dy;
                    if (xx < 0 || yy < 0 || xx >= area.width ||
                        yy >= area.height)
                        continue;
                    size_t index = (size_t)yy * area.width + xx;
                    if (!seen[index] &&
                        image->pixels[(size_t)(area.y + yy) * image->width +
                                      area.x + xx] == 0)
                    {
                        seen[index] = 1;
                        queue[tail++] = index;
                    }
                }
        }
        if (tail >= 5 && maxy-miny >= 4 && maxx-minx >= 1)
            boxes[box_count++] = (Rect){area.x+minx, area.y+miny,
                                        maxx-minx+1, maxy-miny+1};
    }
    qsort(boxes, box_count, sizeof(*boxes), compare_rects);
    int ok = 1;
    for (size_t i = 0; i < box_count; i++)
    {
        char name[64];
        snprintf(name, sizeof(name), "component_%04zu_x%d_y%d", i,
                 boxes[i].x, boxes[i].y);
        if (!write_letter(image, boxes[i], directory, name))
            ok = 0;
    }
    printf("Saved %zu components (check them visually).\n", box_count);
    free(seen); free(queue); free(boxes);
    return ok && box_count > 0;
}
