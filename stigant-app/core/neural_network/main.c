#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "csv.h"
#include "config.h"
#include "ocr.h"


int main(void) {
    const char *train_files[] = {
        "./data/train/example_train.csv",
        "./data/chars74k_train.csv",
        "./data/digital_letters.csv",
        "./data/digital_letters_alt.csv",
        "./data/fonts_train.csv",
        "./data/tmnist_train.csv",
        "./data/fonts_test.csv",
        "./data/chars74k_test.csv",
        "./data/tmnist_test.csv",
    };
    const char *test_files[] = {"./data/test_26_lettres.csv"};

    srand((unsigned)time(NULL));
    FILE *file = fopen("./data/weights.bin", "rb");
    if (file) {
        fclose(file);
        init_ocr("./data/weights.bin");

        test_dataset =
            load_dataset_from_batch_csv(test_files, sizeof(test_files) / sizeof(test_files[0]));
        test();
        free_random_sample_index();
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
