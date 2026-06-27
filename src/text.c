#include <tonc.h>
#include "text.h"

#include "math.h"

#include "borders.h"
#include "numbers.h"

#define VRAM_BORDERS 1
#define VRAM_NUMBERS 12
#define VRAM_TEXT 22
#define TEXT_SBB 30

enum
{
    TOPL, TOP, TOPR, MIDL, MID, MIDR, BOTL, BOT, BOTR
};

const u8 number_widths[] = {
    6, // 0
    5, // 1
    6, // 2
    6, // 3
    6, // 4
    6, // 5
    6, // 6
    6, // 7
    6, // 8
    6, // 9
};
// ----------------------- Private Function Definitions -----------------------

/**
 * Copies sprite data over when the full number sprite can be drawn in one tile.
 */
void copy_tile(uint tile_id, TILE *vram_tile, uint pixel_offset);

/**
 * Copies sprite data over when the sprite spans two tiles, i.e. the left half
 * is drawn in the first tile and the right half in the second.
*/
void copy_split_tile(uint tile_id, TILE *vram_tile, uint pixel_offset);

void setup_tilemap();

// ----------------------------- Public Functions -----------------------------

void load_number_tiles()
{
    memcpy32(tile_mem[0] + VRAM_BORDERS, bordersTiles, bordersTilesLen / 4);
    memcpy32(tile_mem[0] + VRAM_NUMBERS, numbersTiles, numbersTilesLen / 4);
}

// TODO: Switch to drawing on tiles directly
void print_num(const int tile_start, const int x, const int y, const int number)
{
    int digits[8];
    const int num_digits = count_digits(number, digits);
    TILE *vram_tile = &tile_mem[0][VRAM_TEXT + tile_start];

    // Set up tilemap + clear tile data to be used in tilemap
    int width = 0;
    for (int i = 0; i < num_digits; i++)
    {
        width += number_widths[digits[i]];
    }
    const int num_tiles = (width >> 3) + 1;
    for (int i = 0; i < num_tiles; i++)
    {
        // Set tilemap entry
        se_mem[TEXT_SBB][y * 32 + x + i] =
                (VRAM_TEXT + tile_start + i) | SE_PALBANK(1);
        // Set VRAM tile to palette 2, default bg palette color
        memset32(vram_tile++, 0x22222222, 8);
    }

    // Draw numbers to VRAM tiles
    vram_tile = &tile_mem[0][VRAM_TEXT + tile_start];
    uint pixel_offset = 0;
    uint digit_index = 0;
    while (digit_index < num_digits)
    {
        const int tile_id = digits[digit_index];
        const uint number_width = number_widths[tile_id];
        if (pixel_offset + number_width < 8)
        {
            copy_tile(tile_id, vram_tile, pixel_offset);
            pixel_offset += number_width;
        } else
        {
            copy_split_tile(tile_id, vram_tile, pixel_offset);
            pixel_offset += number_width;
            pixel_offset -= 8;
            vram_tile++;
        }
        digit_index++;
    }
}

void print_box(int x, int y, int w, int h)
{
    for (int row = 0; row < h; row++)
    {
        int left = VRAM_BORDERS;
        int middle = VRAM_BORDERS;
        int right = VRAM_BORDERS;
        if (row == 0)
        {
            left += TOPL;
            middle += TOP;
            right += TOPR;
        } else if (row == h - 1)
        {
            left += BOTL;
            middle += BOT;
            right += BOTR;
        } else
        {
            left += MIDL;
            middle += MID;
            right += MIDR;
        }
        u16 *vram = &se_mem[TEXT_SBB][(y + row) * 32 + x];
        *vram++ = left | SE_PALBANK(1);
        for (int x_off = 1; x_off < w - 1; x_off++)
        {
            *vram++ = middle | SE_PALBANK(1);
        }
        *vram = right | SE_PALBANK(1);
    }
}

// ----------------------------- Private Functions -----------------------------

void copy_tile(const uint tile_id, TILE *vram_tile, const uint pixel_offset)
{
    const uint *tile_data = &numbersTiles[tile_id * 8];
    for (int j = 0; j < 8; j++)
    {
        const uint mask = 0xFFFFFFFF << (pixel_offset * 4);
        const uint pixel_row = tile_data[j] << (pixel_offset * 4);
        vram_tile->data[j] &= mask ^ 0xFFFFFFFF;
        vram_tile->data[j] |= pixel_row & mask;
    }
}

void copy_split_tile(const uint tile_id, TILE *vram_tile,
                     const uint pixel_offset)
{
    // -- Draw left side
    // We
    const uint *tile_data = &numbersTiles[tile_id * 8];
    for (int j = 0; j < 8; j++)
    {
        const uint mask = 0xFFFFFFFF << (pixel_offset * 4);
        const uint pixel_row = tile_data[j] << (pixel_offset * 4);
        vram_tile->data[j] &= mask ^ 0xFFFFFFFF;
        vram_tile->data[j] |= pixel_row & mask;
    }

    // -- Draw right side
    vram_tile++;
    //
    const uint new_offset = 8 - pixel_offset;
    for (int j = 0; j < 8; j++)
    {
        const uint mask = 0xFFFFFFFF >> (new_offset * 4);
        const uint pixel_row = tile_data[j] >> (new_offset * 4);
        vram_tile->data[j] &= mask ^ 0xFFFFFFFF;
        vram_tile->data[j] |= pixel_row & mask;
    }
}
