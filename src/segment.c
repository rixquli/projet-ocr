#include "segment.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Group consecutive positions whose projection passes the threshold.
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

int segment_grid(const Image *image, const char *directory)
{
    int xs[512], ys[512];
    int nx = find_lines(image, 1, xs, 512);
    int ny = find_lines(image, 0, ys, 512);
    if (nx < 6 || ny < 6)
    {
        fprintf(stderr, "Grid lines not found. Try another threshold.\n");
        return 0;
    }
    printf("Grid candidate: %d rows, %d columns\n", ny-1, nx-1);
    int failed = 0;
    for (int row = 0; row < ny-1; row++)
        for (int col = 0; col < nx-1; col++)
        {
            int margin_x = (xs[col+1] - xs[col]) / 10;
            int margin_y = (ys[row+1] - ys[row]) / 10;
            if (margin_x < 2) margin_x = 2;
            if (margin_y < 2) margin_y = 2;
            Rect box = {xs[col] + margin_x, ys[row] + margin_y,
                        xs[col+1] - xs[col]-2 * margin_x,
                        ys[row+1] - ys[row]-2 * margin_y};
            char name[64];
            snprintf(name, sizeof(name), "grid_%03d_%03d", row, col);
            if (!write_letter(image, box, directory, name))
            {
                fprintf(stderr, "Could not extract %s\n", name);
                failed++;
            }
        }
    return failed == 0;
}

static int compare_rects(const void *a, const void *b)
{
    const Rect *left = a, *right = b;
    if (left->y != right->y)
        return left->y < right->y ? -1 : 1;
    return left->x < right->x ? -1 : left->x > right->x;
}

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
