CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
CPPFLAGS = -Iinclude $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm
OBJECTS = src/main.o src/image.o src/segment.o

# Neural network: no SDL, and no -std=c11 because csv.c uses POSIX getline
NN_CFLAGS = -Wall -Wextra -fsanitize=address -O2 -Iinclude
NN_LDLIBS = -lm

all: ocr

ocr: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

src/%.o: src/%.c include/image.h include/segment.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

solver: solver/solver.c solver/main.c solver/solver.h
	$(CC) $(CFLAGS) solver/main.c solver/solver.c -o solver/solver

nn: neural_network_app ocr_nn nxor

neural_network/csv.o: neural_network/csv.c include/csv.h include/config.h
	$(CC) $(NN_CFLAGS) -c $< -o $@

neural_network_app: neural_network/main.c neural_network/csv.o
	$(CC) $(NN_CFLAGS) $< neural_network/csv.o -o $@ $(NN_LDLIBS)

ocr_nn: neural_network/ocr.c neural_network/csv.o
	$(CC) $(NN_CFLAGS) $< neural_network/csv.o -o $@ $(NN_LDLIBS)

nxor: neural_network/nxor.c
	$(CC) $(NN_CFLAGS) $< -o $@ $(NN_LDLIBS)

clean:
	rm -f $(OBJECTS) ocr solver/solver neural_network/csv.o neural_network_app ocr_nn nxor

.PHONY: all clean solver nn
