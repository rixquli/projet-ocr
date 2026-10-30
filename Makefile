CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
CPPFLAGS = -Iinclude $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm
OBJECTS = src/main.o src/image.o src/segment.o

all: ocr

ocr: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

src/%.o: src/%.c include/image.h include/segment.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

solver: solver/solver.c solver/main.c solver/solver.h
	$(CC) $(CFLAGS) solver/main.c solver/solver.c -o solver/solver

clean:
	rm -f $(OBJECTS) ocr solver/solver

.PHONY: all clean solver
