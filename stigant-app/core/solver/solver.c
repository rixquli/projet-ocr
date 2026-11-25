
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <math.h>
#include <limits.h>

#include "solver.h"

float *getC(const Grid *grid, int x, int y,
            Direction dir, int a)
{
    int nx = x;
    int ny = y;

    switch (dir)
    {
    case g:
        ny -= a;
        break;
    case d:
        ny += a;
        break;
    case b:
        nx += a;
        break;
    case h:
        nx -= a;
        break;
    case hg:
        nx -= a;
        ny -= a;
        break;
    case hd:
        nx -= a;
        ny += a;
        break;
    case bg:
        nx += a;
        ny -= a;
        break;
    case bd:
        nx += a;
        ny += a;
        break;
    default:
        return '\0';
    }

    if (nx < 0 || nx >= grid->height ||
        ny < 0 || ny >= grid->width)
        return '\0';

    return grid->grille[nx * grid->width + ny];
}

int can_fit(const Grid *grid, int x, int y, Direction dir, int word_size)
{
    return getC(grid, x, y, dir, word_size - 1) != NULL;
}

// From a word and the grid return the max score which is the corresponding word
ScoreGrid *get_score_grid_for(float (*word)[OUTPUT_DIM], int word_size, Grid *grid)
{
    ScoreGrid *res = malloc(sizeof(ScoreGrid));
    res->height = grid->height;
res->width  = grid->width;
res->score  = malloc((size_t)grid->height * grid->width * sizeof(*res->score));

    if (word_size == 0)
        return NULL;
    for (int x = 0; x < grid->height; x++)
    {
        for (int y = 0; y < grid->width; y++)
        {
            for (int dir = 0; dir < 8; dir++)
            {
                int pic = 0;

                float total = 0;
                if(!can_fit(grid,x,y,dir,word_size))
                {
                    res->score[x * grid->width + y][dir] = -INFINITY;
                    continue;
                }
                while (pic < word_size)
                {
                    float sum = 0;

                    float *l = getC(grid, x, y, dir, pic);
                    if(!l) 
                        break;
                    for (size_t i = 0; i < 26; i++)
                    {
                        sum += l[i] * word[pic][i];
                    }
                    total += logf(sum + 0.00001); // to avoid log(0)
                    pic++;
                }
                res->score[x * grid->width + y][dir] = total;
            }
        }
    }

    return res;
}

Solution *get_solution(Grid *grid, float (*word)[OUTPUT_DIM], int word_size)
{
    ScoreGrid *score_grid = get_score_grid_for(word, word_size, grid);

    Solution *solution = malloc(sizeof(Solution));

    float best_score = -INFINITY ;

    for (int x = 0; x < grid->height; x++)
    {
        for (int y = 0; y < grid->width; y++)
        {
            for (int dir = 0; dir < 8; dir++)
            {
                if(!can_fit(grid,x,y,dir,word_size))
                {
                    continue;
                }
                float total = score_grid->score[x * grid->width + y][dir];
                if (total > best_score)
                {
                    best_score = total;
                    solution->x = x;
                    solution->y = y;
                    solution->direction = dir;
                }
            }
        }
    }

    if(best_score/word_size < -1.5)
    {
        printf("Warning: The matching score is low\n");
    }

    solution->word = calloc(word_size + 1, sizeof(char));
    for (size_t i = 0; i < word_size; i++)
    {
        float maxscore = -INFINITY ;
        int letter=0;
        float *proba_letter_grid = getC(grid, solution->x, solution->y, solution->direction, i);
        for (int j = 0; j < 26; j++)
        {
            float score = proba_letter_grid[j] * word[i][j];
            if(score > maxscore)
            {
                maxscore = score;
                letter = j;
            }
        }

        solution->word[i] = (char)letter + 'A';
    }
    solution->word[word_size] = '\0';

    free(score_grid->score);
    free(score_grid);

    return solution;
}

void freeGrid(Grid *grid)
{
    for (int i = 0; i < grid->height; i++)
    {
        free(grid->grille[i]);
    }
    free(grid->grille);
    grid->grille = NULL;
    grid->height = 0;
    grid->width = 0;
}
