#ifndef SOLVER_H
#define SOLVER_H

#include "config.h"

typedef enum {
    g, d, b, h, hg, hd, bg, bd
} Direction;

typedef struct {
    int x;
    int y;
    int l;
    char* word;
    Direction direction;
} Solution;

typedef struct {
    int x;
    int y;
    int l;
    float probas[OUTPUT_DIM];
} Word;

typedef struct {
    int height;
    int width;
    float (*score)[8];
} ScoreGrid;

typedef struct {
    float (*grille)[OUTPUT_DIM];
    int height;
    int width;
} Grid;

float *getC(const Grid *grid, int x, int y, Direction dir, int a);
ScoreGrid *get_score_grid_for(float (*word)[OUTPUT_DIM], int word_size, Grid *grid);
Solution *get_solution(Grid *grid, float (*word)[OUTPUT_DIM], int word_size);
void freeGrid(Grid *grid);

#endif
