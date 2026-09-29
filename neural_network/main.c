#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define LEARNING_RATE 0.01
#define EPOCHS 100
#define INPUT_DIM 784
#define OUTPUT_DIM 26
#define HIDDEN_DIM 256
#define DATASET_NUM 13129
#define DATASET_TEST_NUM 7799

float input[INPUT_DIM] = {0.0f};
float hidden[HIDDEN_DIM] = {0.0f};
float output[OUTPUT_DIM] = {0.0f};

float weights_input_hidden[INPUT_DIM][HIDDEN_DIM];
float weights_hidden_output[HIDDEN_DIM][OUTPUT_DIM];
float biases_hidden[HIDDEN_DIM];
float biases_output[OUTPUT_DIM];

float dataset_input[DATASET_NUM][INPUT_DIM];
float dataset_output[DATASET_NUM][OUTPUT_DIM];

float dataset_input_for_test[DATASET_TEST_NUM][INPUT_DIM];
float dataset_output_for_test[DATASET_TEST_NUM][OUTPUT_DIM];

char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int random_sample_index[DATASET_NUM];

void shuffle(int *array, size_t n)
{
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + (size_t)rand() % (n - i);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void setup_random_sample_index(void)
{
    for (int i = 0; i < DATASET_NUM; i++) {
        random_sample_index[i] = i;
    }
    shuffle(random_sample_index, DATASET_NUM);
}

void load_dataset(void)
{
    FILE *stream = fopen("digital_letters.csv", "r");
    if (!stream) {
        perror("digital_letters.csv");
        exit(1);
    }

    char line[16384];
    int dataset_i = 0;

    if (!fgets(line, (int)sizeof(line), stream)) {
        fprintf(stderr, "empty dataset\n");
        exit(1);
    }

    while (dataset_i < DATASET_NUM && fgets(line, (int)sizeof(line), stream)) {
        char *tok = strtok(line, ",\n");
        if (!tok)
            continue;

        tok = strtok(NULL, ",\n");
        for (int i = 0; i < INPUT_DIM; i++) {
            if (!tok) {
                fprintf(stderr, "row %d: missing pixel %d\n", dataset_i, i);
                exit(1);
            }
            dataset_input[dataset_i][i] = strtof(tok, NULL) / 255.0f;
            tok = strtok(NULL, ",\n");
        }

        if (!tok || tok[0] < 'A' || tok[0] > 'Z') {
            fprintf(stderr, "row %d: bad label\n", dataset_i);
            exit(1);
        }
        memset(dataset_output[dataset_i], 0, sizeof(dataset_output[dataset_i]));
        dataset_output[dataset_i][tok[0] - 'A'] = 1.0f;
        dataset_i++;
    }

    if (dataset_i < DATASET_NUM) {
        fprintf(stderr, "only loaded %d / %d samples\n", dataset_i, DATASET_NUM);
        exit(1);
    }
    fclose(stream);
}
void load_dataset_for_test(void)
{
    FILE *stream = fopen("digital_letters_alt.csv", "r");
    if (!stream) {
        perror("digital_letters_alt.csv");
        exit(1);
    }

    char line[16384];
    int dataset_i = 0;

    if (!fgets(line, (int)sizeof(line), stream)) {
        fprintf(stderr, "empty dataset\n");
        exit(1);
    }

    while (dataset_i < DATASET_TEST_NUM && fgets(line, (int)sizeof(line), stream)) {
        char *tok = strtok(line, ",\n");
        if (!tok)
            continue;

        tok = strtok(NULL, ",\n");
        for (int i = 0; i < INPUT_DIM; i++) {
            if (!tok) {
                fprintf(stderr, "row %d: missing pixel %d\n", dataset_i, i);
                exit(1);
            }
            dataset_input_for_test[dataset_i][i] = strtof(tok, NULL) / 255.0f;
            tok = strtok(NULL, ",\n");
        }

        if (!tok || tok[0] < 'A' || tok[0] > 'Z') {
            fprintf(stderr, "row %d: bad label\n", dataset_i);
            exit(1);
        }
        memset(dataset_output_for_test[dataset_i], 0,
               sizeof(dataset_output_for_test[dataset_i]));
        dataset_output_for_test[dataset_i][tok[0] - 'A'] = 1.0f;
        dataset_i++;
    }

    if (dataset_i < DATASET_TEST_NUM) {
        fprintf(stderr, "only loaded %d / %d samples\n", dataset_i, DATASET_TEST_NUM);
        exit(1);
    }
    fclose(stream);
}

float random_float(float limit)
{
    return ((float)rand() / (float)RAND_MAX) * (2.0f * limit) - limit;
}


float sigmoid_prime(float x)
{
  return x * (1-x);
}

float sigmoid(float x)
{
  return 1.0f / (1.0f + expf(-x));
}

float binary_cross_entropy(float prediction, float expected)
{
    return -(expected * logf(prediction)
           + (1.0f - expected) * logf(1.0f - prediction));
}


void initialize_weights(void)
{
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

void calcul(float sample[INPUT_DIM])
{
    for (int i = 0; i < HIDDEN_DIM; i++) {
        float sum = biases_hidden[i];
        for (int j = 0; j < INPUT_DIM; j++) {
            sum += sample[j] * weights_input_hidden[j][i];
        }
        hidden[i] = sigmoid(sum);
    }

    for (int i = 0; i < OUTPUT_DIM; i++) {
        float sum = biases_output[i];
        for (int j = 0; j < HIDDEN_DIM; j++) {
            sum += hidden[j] * weights_hidden_output[j][i];
        }
        output[i] = sigmoid(sum);
    }
}

float sample_cost(float expected[OUTPUT_DIM])
{
    float c = 0.0f;
    for (int j = 0; j < OUTPUT_DIM; j++) {
        float prediction = output[j];
        c += binary_cross_entropy(prediction, expected[j]);
    }
    return c;
}

char max_letter(float values[OUTPUT_DIM])
{
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

void train(void)
{
    for (int e = 0; e < EPOCHS; e++) {
        setup_random_sample_index();
        float epoch_cost = 0.0f;
        int correct = 0;

        for (int d = 0; d < DATASET_NUM; d++) {
            int idx = random_sample_index[d];
            calcul(dataset_input[idx]);
            epoch_cost += sample_cost(dataset_output[idx]);

            if (max_letter(output) == max_letter(dataset_output[idx])) {
                correct++;
            }

            float hidden_delta[HIDDEN_DIM] = {0};
            float output_delta[OUTPUT_DIM] = {0};

            for (int j = 0; j < OUTPUT_DIM; j++) {
                float expected = dataset_output[idx][j];
                float prediction = output[j];
                output_delta[j] = prediction - expected;
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
                for (int j = 0; j < HIDDEN_DIM; j++) {
                    weights_input_hidden[i][j] -=
                        LEARNING_RATE * hidden_delta[j] * dataset_input[idx][i];
                }
            }

            for (int i = 0; i < HIDDEN_DIM; i++) {
                for (int j = 0; j < OUTPUT_DIM; j++) {
                    weights_hidden_output[i][j] -=
                        LEARNING_RATE * output_delta[j] * hidden[i];
                }
            }

            for (int j = 0; j < OUTPUT_DIM; j++) {
                biases_output[j] -= LEARNING_RATE * output_delta[j];
            }
        }

        epoch_cost /= (float)DATASET_NUM;
        float accuracy = 100.0f * (float)correct / (float)DATASET_NUM;
        printf("Epoch %3d | cout = %.6f | precision = %.2f%%\n", e + 1,
               epoch_cost, accuracy);
        fflush(stdout);
    }
}

void test(void)
{
    float total_cost = 0.0f;
    int correct = 0;

    printf("\n=== TEST ===\n");

    for (int d = 0; d < DATASET_TEST_NUM; d++) {
        calcul(dataset_input_for_test[d]);
        total_cost += sample_cost(dataset_output_for_test[d]);

        char pred = max_letter(output);
        char expected = max_letter(dataset_output_for_test[d]);
        if (pred == expected) {
            correct++;
        }

        printf("%c (attendu %c)%s\n", pred, expected,
               pred == expected ? "" : "  X");
    }

    total_cost /= (float)DATASET_TEST_NUM;
    float accuracy = 100.0f * (float)correct / (float)DATASET_TEST_NUM;

    printf("\n=== RESULTATS ===\n");
    printf("Cout moyen     : %.6f\n", total_cost);
    printf("Validation     : %d / %d (%.2f%%)\n", correct, DATASET_TEST_NUM,
           accuracy);
}

int main(void)
{
    srand((unsigned)time(NULL));
    load_dataset();
    load_dataset_for_test();
    initialize_weights();
    train();
    test();
    return 0;
}
