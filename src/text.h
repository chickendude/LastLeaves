#ifndef LASTLEAVES_TEXT_H
#define LASTLEAVES_TEXT_H

#define VRAM_BORDERS 1
#define VRAM_NUMBERS (1024 - 10)

/**
 * Prints a number to the screen
 * @param tile_start Tile (sprite data) in VRAM to draw number to
 * @param x X position in the text background
 * @param y Y position in the text background
 * @param number Value to print
 */
void print_num(int tile_start, int x, int y, int number);

/**
 * Load the border tiles and number tiles into VRAM.
 */
void load_number_tiles();

/**
 * Draws a rectangular box on screen.
 * @param x X position to draw box at
 * @param y Y position to draw box at
 * @param w Box width
 * @param h Box height
 */
void print_box(int x, int y, int w, int h);

#endif //LASTLEAVES_TEXT_H
