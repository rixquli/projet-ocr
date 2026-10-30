CC = cc
CFLAGS = -Wall -Wextra -fsanitize=address -O2 -Wall -Wextra -Iinclude
LDLIBS = -lm

.PHONY: all clean

all: csv neural_network_app ocr nxor

csv: neural_network/csv.o

neural_network/csv.o: neural_network/csv.c include/csv.h include/config.h
	$(CC) $(CFLAGS) -c $< -o $@

neural_network_app: neural_network/main.c neural_network/csv.o
	$(CC) $(CFLAGS) $< neural_network/csv.o -o $@ $(LDLIBS)

ocr: neural_network/ocr.c neural_network/csv.o
	$(CC) $(CFLAGS) $< neural_network/csv.o -o $@ $(LDLIBS)

nxor: neural_network/nxor.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f neural_network/csv.o neural_network_app ocr nxor
