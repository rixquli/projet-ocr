#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

typedef struct dataset {
    float **data;
    char *expected_output;
    size_t rows;
    size_t allocated_rows;
} dataset;

FILE *open_file(const char *filename);
void close_file(FILE *file);
char *read_line(FILE *file);
dataset *load_dataset_from_csv(const char *filename);
dataset *load_dataset_from_batch_csv(const char **filename, size_t num_files);
void free_dataset(dataset *dataset);
void allocate_more(dataset *dataset);

#endif
