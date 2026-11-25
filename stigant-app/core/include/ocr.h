#ifndef OCR_NN_H
#define OCR_NN_H

#include "config.h"
#include "csv.h"

// Global variable for main.c and ocr.c
extern dataset *training_dataset;
extern dataset *test_dataset;

void initialize_weights(void);
void train(void);
void test(void);
void save_weights(char *filename);
void free_random_sample_index(void);

// For inference
void init_ocr(char *filename);  
void infer_img(const float input_arg[INPUT_DIM], float probas[OUTPUT_DIM]);

#endif
