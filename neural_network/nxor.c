#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define LEARNING_RATE 0.5
#define EPOCHS 10000
#define INPUT_DIM 2
#define OUTPUT_DIM 1
#define HIDDEN_DIM 2
#define DATASET_NUM 4

float input[INPUT_DIM] = {0.0f, 0.0f};
float hidden[HIDDEN_DIM] = {0.0f, 0.0f};
float output[OUTPUT_DIM] = {0.0f};

float weights_input_hidden[INPUT_DIM][HIDDEN_DIM] = {{0.0f}};
float weights_hidden_output[HIDDEN_DIM][OUTPUT_DIM] = {{0.0f}};
float biases_hidden[HIDDEN_DIM] = {0.0f};
float biases_output[OUTPUT_DIM] = {0.0f};

float dataset_input[4][2] = {
    {0, 0},
    {0, 1},
    {1, 0},
    {1, 1},
};
float dataset_output[4][1] = {
    {1},
    {0},
    {0},
    {1},
};

float random_float(void) {
    float limit = 1.0f / sqrtf(2.0f);
    return ((float)rand() / (float)RAND_MAX) * (2.0f * limit) - limit;
}

float sigmoid(float x) {
    return 1.0f / (1.0f + expf(-x));
}

void initialize_weights() {
    for (int i = 0; i < INPUT_DIM; i++) {
        for (int j = 0; j < HIDDEN_DIM; j++) {
            weights_input_hidden[i][j] = random_float();
        }
    }

    for (int i = 0; i < HIDDEN_DIM; i++) {
        for (int j = 0; j < OUTPUT_DIM; j++) {
            weights_hidden_output[i][j] = random_float();
        }
    }

    for (int i = 0; i < HIDDEN_DIM; i++) {
        biases_hidden[i] = random_float();
    }

    for (int i = 0; i < OUTPUT_DIM; i++) {
        biases_output[i] = random_float();
    }
}

void calcul(float input[INPUT_DIM]) {

    // Hidden Layer Calculation
    for (int i = 0; i < HIDDEN_DIM; i++) {
        hidden[i] = 0.0f;
        for (int j = 0; j < INPUT_DIM; j++) {
            hidden[i] += input[j] * weights_input_hidden[j][i];
        }
        hidden[i] += biases_hidden[i];
        hidden[i] = sigmoid(hidden[i]);
    }

    // Output Layer Calculation
    for (int i = 0; i < OUTPUT_DIM; i++) {
        output[i] = 0.0f;
        for (int j = 0; j < HIDDEN_DIM; j++) {
            output[i] += hidden[j] * weights_hidden_output[j][i];
        }
        output[i] += biases_output[i];
        output[i] = sigmoid(output[i]);
    }
}

void train()
{
  for (int e = 0; e < EPOCHS; e++) {
    for (int d = 0; d < DATASET_NUM; d++) {
        calcul(dataset_input[d]);

        // Calculation for backward propagation
        float hidden_delta[HIDDEN_DIM] = {0};
        float output_delta[OUTPUT_DIM] = {0};

        for (int j = 0; j < OUTPUT_DIM; j++) {
            float expected = dataset_output[d][j];
            float prediction = output[j];
            output_delta[j] = (prediction - expected) * prediction * (1.0f - prediction);
        }

        for (int i = 0; i < HIDDEN_DIM; i++) {
            for (int j = 0; j < OUTPUT_DIM; j++) {
                hidden_delta[i] += weights_hidden_output[i][j] * output_delta[j] * hidden[i] *
                                   (1.0f - hidden[i]);
            }
        }

        // Hidden Biais Propagation  
        for (int j = 0; j < HIDDEN_DIM; j++) {
            biases_hidden[j] -= LEARNING_RATE * hidden_delta[j];
        }

        // input -> hidden weigths Propagation   
        for (int i = 0; i < INPUT_DIM; i++) {
            for (int j = 0; j < HIDDEN_DIM; j++) {
                float gradient = hidden_delta[j] * dataset_input[d][i];
                weights_input_hidden[i][j] -= LEARNING_RATE * gradient;
            }
        }

        // Update hidden -> output weigth
        for (int i = 0; i < HIDDEN_DIM; i++) {
            for (int j = 0; j < OUTPUT_DIM; j++) {
                float gradient = output_delta[j] * hidden[i];
                weights_hidden_output[i][j] -= LEARNING_RATE * gradient;
            }
        }

        // Update output biais 
        for (int j = 0; j < OUTPUT_DIM; j++) {
            biases_output[j] -= LEARNING_RATE * output_delta[j];
        }
    }
}
}

int main() {
    initialize_weights();

    train();
  
    printf("\n=== TEST ===\n");

    for (int d = 0; d < DATASET_NUM; d++) {
        calcul(dataset_input[d]);

        printf("%.0f NXOR %.0f -> %.4f (should be %.0f)\n", dataset_input[d][0], dataset_input[d][1],
               output[0], dataset_output[d][0]);
    }

    return 0;
}
