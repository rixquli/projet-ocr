#ifndef SOLVER_H
#define SOLVER_H

typedef enum {
    g, d, b, h, hg, hd, bg, bd
} Direction;

typedef struct {
    int x;
    int y;
    int l;
    Direction direction;
} Solution;

typedef struct {
    char **grille;
    int height;
    int width;
} Grid;

char getC(const Grid *grid, int x, int y, Direction dir, int a);
Solution *resolve(const Grid *grid, const char *mot);
void printSolution(const Solution *sol, const char *mot);
int loadGrid(Grid *grid, const char *filename);
void freeGrid(Grid *grid);

#endif
