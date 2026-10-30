#include <stdio.h>
#include <stdlib.h>

#include "solver.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Utilisation : %s <fichier_grille> <mot>",argv[0]);
        return 1;
    }

    Grid grid;
    if (!loadGrid(&grid, argv[1])){
        return 1;
    }

    Solution *sol = resolve(&grid, argv[2]);
    printSolution(sol);

    freeGrid(&grid);
    free(sol);
    return 0;
}
