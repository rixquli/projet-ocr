# Projet OCR

Gayerie Adrien, Lipski Joel, Barrafitte Sohan, Privat Theo

## Build

The project needs a C compiler, pkg-config, SDL2 and SDL2_image. Check them
on an EPITA machine:

    pkg-config --modversion sdl2 SDL2_image
    make

SDL2_image is a separate library from SDL2. It is used to load and save PNG
images. No image processing algorithm is delegated to it.

## Run

Create the output directory before running the program:

    mkdir -p output
    ./ocr data/input.png output

The program saves gray.png, binary.png and rotated.png in that directory.
Existing files with the same names are replaced. The default black and white
threshold is calculated with Otsu's method. To choose one manually:

    ./ocr data/input.png output --threshold 180

Rotate clockwise by 15 degrees, or estimate a correction between -30 and 30:

    ./ocr data/input.png output --rotate 15
    ./ocr data/input.png output --auto

Extract a grid with visible borders:

    ./ocr data/input.png output --grid

The extraction returns a LetterGrid in memory. Its letters[row][column]
contains a 28 by 28 Image. The caller owns it and must call letter_grid_free.
The command also saves files named grid_ROW_COLUMN.png, starting at zero.
The grid detector
is experimental: it assumes long, dark, straight borders and can mistake other
lines for grid lines. Inspect the resulting images and counts.

To inspect connected components in a rectangular area:

    ./ocr data/input.png output --components X Y WIDTH HEIGHT

Coordinates refer to rotated.png, not the original image. The files are named
with their position. A component can be noise, part of a letter, or several
touching letters; components are not grouped into words. Choose a region
containing the word list and check the output visually.

Each extracted image is 28 by 28 pixels, with a black character on a white
background. The character fits in a centered 20 by 20 area while retaining its
proportions. Options can be combined, for example:

    ./ocr data/input.png output --rotate -5 --grid

Use a clean output directory when comparing runs: older letter files are not
automatically removed. `make clean` removes the executable and object files.

## Neural network

The letter classifier does not need SDL:

    make nn
    ./ocr_nn

It reads the trained weights from data/weights.bin and evaluates the CSV files
listed in neural_network/ocr.c. Without data/weights.bin, it trains first and
saves them.

## Solver

The word search solver does not need SDL either:

    make solver
    ./solver/solver solver/test hello

The grid file contains one row of uppercase letters per line. All rows must
have the same length; empty lines are ignored. The word is case insensitive
and is searched in all eight directions, including backwards and diagonals.

The program prints the first and last letter of the first match as
(column, row)(column, row), starting at zero:

    $ ./solver/solver solver/test hello
    (9, 8)(5, 8)

If the word is not in the grid, it prints Not found.
