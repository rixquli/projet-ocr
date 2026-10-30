#include "csv.h"

FILE *open_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error while file %s\n", filename);
        exit(1);
    }
    return file;
}
void close_file(FILE *file) {
    if (file)
        fclose(file);
}

char *read_line(FILE *file) {
    char *line = NULL;
    size_t capacity = 0;

    if (getline(&line, &capacity, file) == -1) {
        free(line);
        return NULL;
    }
    return line;
}

void free_dataset(dataset *dataset) {
    for (size_t row = 0; row < dataset->rows; row++)
        free(dataset->data[row]);
    free(dataset->data);
    free(dataset->expected_output);
    free(dataset);
}

void allocate_more(dataset *dataset) {
    dataset->allocated_rows += 1000;
    if (!dataset->data) {
        dataset->data = calloc(dataset->allocated_rows, sizeof(*dataset->data));
        dataset->expected_output = calloc(dataset->allocated_rows, sizeof(char));

    } else {
        dataset->data = realloc(dataset->data, dataset->allocated_rows * sizeof(*dataset->data));
        dataset->expected_output =
            realloc(dataset->expected_output, dataset->allocated_rows * sizeof(char));
    }
}

dataset *load_dataset_from_csv(const char *filename) {
    FILE *file = open_file(filename);

    dataset *dataset = calloc(1, sizeof(*dataset));
    allocate_more(dataset);

    char *line = read_line(file);
    free(line);
    while ((line = read_line(file))) {
        if (dataset->rows >= dataset->allocated_rows)
            allocate_more(dataset);

        char *val = strtok(line, ",");
        dataset->data[dataset->rows] = calloc(INPUT_DIM, sizeof(*dataset->data[dataset->rows]));
        for (int i = 0; i < INPUT_DIM; i++) {
            val = strtok(NULL, ",");
            dataset->data[dataset->rows][i] = (float)atof(val) / 255.0f;
        }
        val = strtok(NULL, ",");
        dataset->expected_output[dataset->rows] = val[0];
        dataset->rows++;
        free(line);
    }

    close_file(file);
    return dataset;
}

dataset *load_dataset_from_batch_csv(const char **filename, size_t num_files) {
    dataset *dataset = calloc(1, sizeof(*dataset));
    allocate_more(dataset);

    for (size_t i = 0; i < num_files; i++) {
        FILE *file = open_file(filename[i]);

        char *line = read_line(file);
        free(line);
        while ((line = read_line(file))) {
            if (dataset->rows >= dataset->allocated_rows)
                allocate_more(dataset);

            char *val = strtok(line, ",");
            dataset->data[dataset->rows] = calloc(INPUT_DIM, sizeof(*dataset->data[dataset->rows]));
            for (int i = 0; i < INPUT_DIM; i++) {
                val = strtok(NULL, ",");
                dataset->data[dataset->rows][i] = (float)atof(val) / 255.0f;
            }
            val = strtok(NULL, ",");
            dataset->expected_output[dataset->rows] = val[0];
            dataset->rows++;
            free(line);
        }

        close_file(file);
    }

    return dataset;
}