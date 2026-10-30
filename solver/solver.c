#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

#include "solver.h"

char getC(const Grid *grid, int x, int y,
          Direction dir, int a)
{
    int nx = x;
    int ny = y;

    switch (dir) {
        case g:  ny -= a; break;
        case d:  ny += a; break;
        case b:  nx += a; break;
        case h:  nx -= a; break;
        case hg: nx -= a; ny -= a; break;
        case hd: nx -= a; ny += a; break;
        case bg: nx += a; ny -= a; break;
        case bd: nx += a; ny += a; break;
        default: return '\0';
    }

    if (nx < 0 || nx >= grid->height ||
        ny < 0 || ny >= grid->width)
        return '\0';

    return grid->grille[nx][ny];
}


Solution *resolve(const Grid *grid, const char *mot)
{
    int ml = strlen(mot);

    if (ml == 0)
        return NULL;

    for (int x = 0; x < grid->height; x++) {
        for (int y = 0; y < grid->width; y++) {
            for (int dir = 0; dir < 8; dir++) {
                int pic = 0;
                char c;

                while (pic < ml &&
                       (c = getC(grid, x, y,
                                 (Direction)dir, pic)) != '\0' &&
                       c == toupper((unsigned char)mot[pic])) {
                    pic++;
                }

                if (pic == ml) {
                    Solution *sol = malloc(sizeof(*sol));

                    if (sol == NULL)
                        return NULL;

                    sol->x = x;
                    sol->y = y;
                    sol->l = ml;
                    sol->direction = (Direction)dir;

                    return sol;
                }
            }
        }
    }

    return NULL;
}

void printSolution(const Solution *sol)
{
    if (sol == NULL) {
        printf("Not found\n");
        return;
    }

    int endX = sol->x;
    int endY = sol->y;

    switch (sol->direction) {
        case g:  endY -= sol->l - 1; break;
        case d:  endY += sol->l - 1; break;
        case b:  endX += sol->l - 1; break;
        case h:  endX -= sol->l - 1; break;
        case hg: endX -= sol->l - 1;
                 endY -= sol->l - 1; break;
        case hd: endX -= sol->l - 1;
                 endY += sol->l - 1; break;
        case bg: endX += sol->l - 1;
                 endY -= sol->l - 1; break;
        case bd: endX += sol->l - 1;
                 endY += sol->l - 1; break;
    }

    printf("(%d, %d)(%d, %d)\n", sol->y, sol->x, endY,endX);
}

int loadGrid(Grid *grid, const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Erreur : impossible d'ouvrir %s\n", filename);
        return 0;
    }

    char *line = NULL;
    size_t capacity = 0;
    ssize_t length;

    grid->height = 0;
    grid->width = 0;
    grid->grille = NULL;

    while ((length = getline(&line, &capacity, file)) != -1) {
        if (length > 0 && line[length - 1] == '\n'){
            line[--length] = '\0';
        }

        if (length > 0 && line[length - 1] == '\r'){
            line[--length] = '\0';
        }

        if (length == 0){
            continue;
        }

        if (grid->height == 0) {
            grid->width = length;
        } else if ((int)length != grid->width) {
            printf("Erreur : grille non rectangulaire.\n");
            free(line);
            fclose(file);
            return 0;
        }

        char **newGrid = realloc(
            grid->grille,
            (grid->height + 1) * sizeof(char *)
        );

        if (newGrid == NULL) {
            free(line);
            fclose(file);
            return 0;
        }

        grid->grille = newGrid;
        grid->grille[grid->height] = malloc(length + 1);

        if (grid->grille[grid->height] == NULL) {
            free(line);
            fclose(file);
            return 0;
        }
        memcpy(grid->grille[grid->height], line, length + 1);
        grid->height++;
    }

    free(line);
    fclose(file);
    return grid->height > 0;
}

void freeGrid(Grid *grid)
{
    for (int i = 0; i < grid->height; i++){
        free(grid->grille[i]);
    }
    free(grid->grille);
    grid->grille = NULL;
    grid->height = 0;
    grid->width = 0;
}
