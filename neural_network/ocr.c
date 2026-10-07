#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "csv.h"
#include "config.h"

float input[INPUT_DIM] = {0.0f};
float hidden[HIDDEN_DIM] = {0.0f};
float output[OUTPUT_DIM] = {0.0f};

float weights_input_hidden[INPUT_DIM][HIDDEN_DIM];
float weights_hidden_output[HIDDEN_DIM][OUTPUT_DIM];
float biases_hidden[HIDDEN_DIM];
float biases_output[OUTPUT_DIM];

dataset *training_dataset;

dataset *test_dataset;

char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

size_t *random_sample_index;

void shuffle(size_t *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + (size_t)rand() % (n - i);
            size_t t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void setup_random_sample_index(void) {
    random_sample_index = calloc(training_dataset->rows, sizeof(*random_sample_index));
    for (size_t i = 0; i < training_dataset->rows; i++)
        random_sample_index[i] = i;
    shuffle(random_sample_index, training_dataset->rows);
}

void free_random_sample_index(void) {
    free(random_sample_index);
    random_sample_index = NULL;
}

float random_float(float limit) {
    return ((float)rand() / (float)RAND_MAX) * (2.0f * limit) - limit;
}

float sigmoid_prime(float x) {
    return x * (1 - x);
}

float sigmoid(float x) {
    return 1.0f / (1.0f + expf(-x));
}

float binary_cross_entropy(float prediction, float expected) {
    return -(expected * logf(prediction) + (1.0f - expected) * logf(1.0f - prediction));
}

void initialize_weights(void) {
    float lim_ih = 1.0f / sqrtf((float)INPUT_DIM);
    float lim_ho = 1.0f / sqrtf((float)HIDDEN_DIM);

    for (int i = 0; i < INPUT_DIM; i++) {
        for (int j = 0; j < HIDDEN_DIM; j++) {
            weights_input_hidden[i][j] = random_float(lim_ih);
        }
    }

    for (int i = 0; i < HIDDEN_DIM; i++) {
        for (int j = 0; j < OUTPUT_DIM; j++) {
            weights_hidden_output[i][j] = random_float(lim_ho);
        }
    }

    for (int i = 0; i < HIDDEN_DIM; i++) {
        biases_hidden[i] = 0.0f;
    }

    for (int i = 0; i < OUTPUT_DIM; i++) {
        biases_output[i] = 0.0f;
    }
}

void calcul(float sample[INPUT_DIM]) {
    float sums[HIDDEN_DIM];
    for (int i = 0; i < HIDDEN_DIM; i++)
        sums[i] = biases_hidden[i];

    for (int j = 0; j < INPUT_DIM; j++) {
        float pixel = sample[j];
        if (pixel == 0.0f)
            continue;
        for (int i = 0; i < HIDDEN_DIM; i++)
            sums[i] += pixel * weights_input_hidden[j][i];
    }

    for (int i = 0; i < HIDDEN_DIM; i++)
        hidden[i] = sigmoid(sums[i]);

    for (int i = 0; i < OUTPUT_DIM; i++) {
        float sum = biases_output[i];
        for (int j = 0; j < HIDDEN_DIM; j++) {
            sum += hidden[j] * weights_hidden_output[j][i];
        }
        output[i] = sigmoid(sum);
    }
}

char max_letter(float values[OUTPUT_DIM]) {
    float val_max = values[0];
    int i_max = 0;
    for (int i = 1; i < OUTPUT_DIM; i++) {
        if (values[i] > val_max) {
            val_max = values[i];
            i_max = i;
        }
    }
    return alphabet[i_max];
}

int encode_label(char label, float expected[OUTPUT_DIM]) {
    int index = 0;
    while (index < OUTPUT_DIM && alphabet[index] != label)
        index++;
    if (index == OUTPUT_DIM)
        return -1;

    for (int i = 0; i < OUTPUT_DIM; i++)
        expected[i] = 0.0f;
    expected[index] = 1.0f;
    return index;
}

void train(void) {
    for (int e = 0; e < EPOCHS; e++) {
        setup_random_sample_index();
        int correct = 0;

        for (size_t d = 0; d < training_dataset->rows; d++) {
            size_t idx = random_sample_index[d];
            float expected[OUTPUT_DIM];
            int expected_index = encode_label(training_dataset->expected_output[idx], expected);
            if (expected_index < 0) {
                fprintf(stderr, "Invalid training label: %c\n",
                        training_dataset->expected_output[idx]);
                exit(EXIT_FAILURE);
            }

            calcul(training_dataset->data[idx]);

            if (max_letter(output) == alphabet[expected_index]) {
                correct++;
            }

            float hidden_delta[HIDDEN_DIM] = {0};
            float output_delta[OUTPUT_DIM] = {0};

            for (int j = 0; j < OUTPUT_DIM; j++) {
                float prediction = output[j];
                output_delta[j] = prediction - expected[j];
            }

            for (int i = 0; i < HIDDEN_DIM; i++) {
                float sum = 0.0f;
                for (int j = 0; j < OUTPUT_DIM; j++) {
                    sum += weights_hidden_output[i][j] * output_delta[j];
                }
                hidden_delta[i] = sum * sigmoid_prime(hidden[i]);
            }

            for (int j = 0; j < HIDDEN_DIM; j++) {
                biases_hidden[j] -= LEARNING_RATE * hidden_delta[j];
            }

            for (int i = 0; i < INPUT_DIM; i++) {
                float pixel = training_dataset->data[idx][i];
                if (pixel == 0.0f)
                    continue;
                for (int j = 0; j < HIDDEN_DIM; j++) {
                    weights_input_hidden[i][j] -= LEARNING_RATE * hidden_delta[j] * pixel;
                }
            }

            for (int i = 0; i < HIDDEN_DIM; i++) {
                for (int j = 0; j < OUTPUT_DIM; j++) {
                    weights_hidden_output[i][j] -= LEARNING_RATE * output_delta[j] * hidden[i];
                }
            }

            for (int j = 0; j < OUTPUT_DIM; j++) {
                biases_output[j] -= LEARNING_RATE * output_delta[j];
            }
        }

        float accuracy = 100.0f * (float)correct / (float)training_dataset->rows;
        printf("Epoch %3d | precision = %.2f%%\n", e + 1, accuracy);
        fflush(stdout);
    }
}

void test(void) {
    int correct = 0;

    printf("\n=== TEST ===\n");

    for (size_t d = 0; d < test_dataset->rows; d++) {
        float expected[OUTPUT_DIM];
        int expected_index = encode_label(test_dataset->expected_output[d], expected);
        if (expected_index < 0) {
            fprintf(stderr, "Invalid test label: %c\n", test_dataset->expected_output[d]);
            exit(EXIT_FAILURE);
        }

        calcul(test_dataset->data[d]);

        char pred = max_letter(output);
        char expected_letter = alphabet[expected_index];
        if (pred == expected_letter) {
            correct++;
        }

        printf("%c (attendu %c)%s\n", pred, expected_letter, pred == expected_letter ? "" : "  X");
    }

    float accuracy = 100.0f * (float)correct / (float)test_dataset->rows;

    printf("\n=== RESULTATS ===\n");
    printf("Validation     : %d / %ld (%.2f%%)\n", correct, test_dataset->rows, accuracy);
}

void save_weights(char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error opening weights file for writing: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fwrite(weights_input_hidden, sizeof(float), INPUT_DIM * HIDDEN_DIM, file);
    fwrite(weights_hidden_output, sizeof(float), HIDDEN_DIM * OUTPUT_DIM, file);
    fwrite(biases_hidden, sizeof(float), HIDDEN_DIM, file);
    fwrite(biases_output, sizeof(float), OUTPUT_DIM, file);

    fclose(file);
}

// Load all the weights and biases from a file
void init_ocr(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening weights file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fread(weights_input_hidden, sizeof(float), INPUT_DIM * HIDDEN_DIM, file);
    fread(weights_hidden_output, sizeof(float), HIDDEN_DIM * OUTPUT_DIM, file);
    fread(biases_hidden, sizeof(float), HIDDEN_DIM, file);
    fread(biases_output, sizeof(float), OUTPUT_DIM, file);

    fclose(file);
}

int main(void) {
    const char *train_files[] = {"./data/train/chars74k_train.csv",
                                 "./data/train/digital_letters_alt.csv",
                                 "./data/train/fonts_train.csv", "./data/train/tmnist_train.csv"};
    const char *test_files[] = {"./data/test/chars74k_test.csv", "./data/test/fonts_test.csv",
                                "./data/test/tmnist_test.csv"};

    srand((unsigned)time(NULL));
    FILE *file = fopen("./data/weights.bin", "rb");
    if (file) {
        fclose(file);
        init_ocr("./data/weights.bin");

        test_dataset =
            load_dataset_from_batch_csv(test_files, sizeof(test_files) / sizeof(test_files[0]));
        test();
        free_random_sample_index();
        free_dataset(training_dataset);
        free_dataset(test_dataset);
    } else {
        printf("Training the neural network...\n");

        training_dataset =
            load_dataset_from_batch_csv(train_files, sizeof(train_files) / sizeof(train_files[0]));
        test_dataset =
            load_dataset_from_batch_csv(test_files, sizeof(test_files) / sizeof(test_files[0]));
        initialize_weights();
        train();
        save_weights("./data/weights.bin");
        test();
        free_random_sample_index();
        free_dataset(training_dataset);
        free_dataset(test_dataset);
    }

    return 0;
}
