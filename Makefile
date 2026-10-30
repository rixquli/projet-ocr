CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Iinclude $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm
OBJECTS = src/main.o src/image.o src/segment.o

all: ocr

ocr: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

src/%.o: src/%.c include/image.h include/segment.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) ocr

.PHONY: all clean
