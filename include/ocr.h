#ifndef OCR_NN_H
#define OCR_NN_H

#include "config.h"

void init_ocr(char *filename);   // charge les poids (a appeler une fois avant infer_img)
void infer_img(const float input_arg[INPUT_DIM], float probas[OUTPUT_DIM]);

#endif