#include <tonc.h>
#include "text.h"

#include "math.h"

#include "borders.h"
#include "font.h"

#define NUMBER_ASCII_START 16
#define NUMBER_DATA_START (NUMBER_ASCII_START * 8)
#define VRAM_TEXT 22
#define TEXT_SBB 30

enum
{
    TOPL, TOP, TOPR, MIDL, MID, MIDR, BOTL, BOT, BOTR, MIDT, MIDM, MIDB
};

const u8 font_widths[] = {
    3, // 20   (space)
    2, // 21 !
    4, // 22 "
    6, // 23 #
    6, // 24 $
    6, // 25 %
    6, // 26 &
    3, // 27 '
    3, // 28 (
    3, // 29 )
    6, // 2A *
    6, // 2B +
    3, // 2C ,
    5, // 2D -
    3, // 2E .
    5, // 2F /

    6, // 30 0
    5, // 31 1
    6, // 32 2
    6, // 33 3
    6, // 34 4
    6, // 35 5
    6, // 36 6
    6, // 37 7
    6, // 38 8
    6, // 39 9

    2, // 3A :
    3, // 3B ;
    5, // 3C <
    5, // 3D =
    5, // 3E >
    6, // 3F ?
    8, // 40 @

    7, // 41 A
    6, // 42 B
    6, // 43 C
    6, // 44 D
    6, // 45 E
    6, // 46 F
    6, // 47 G
    6, // 48 H
    5, // 49 I
    6, // 4A J
    6, // 4B K
    5, // 4C L
    6, // 4D M
    6, // 4E N
    6, // 4F O
    6, // 50 P
    7, // 51 Q
    6, // 52 R
    6, // 53 S
    7, // 54 T
    6, // 55 U
    6, // 56 V
    7, // 57 W
    6, // 58 X
    7, // 59 Y
    6, // 5A Z

    3, // 5B [
    5, // 5C backslash
    3, // 5D ]
    6, // 5E ^
    5, // 5F _
    3, // 60 `

    6, // 61 a
    6, // 62 b
    5, // 63 c
    6, // 64 d
    6, // 65 e
    5, // 66 f
    6, // 67 g
    6, // 68 h
    3, // 69 i
    6, // 6A j
    6, // 6B k
    3, // 6C l
    7, // 6D m
    6, // 6E n
    6, // 6F o
    6, // 70 p
    6, // 71 q
    6, // 72 r
    5, // 73 s
    5, // 74 t
    6, // 75 u
    6, // 76 v
    7, // 77 w
    7, // 78 x
    6, // 79 y
    6, // 7A z

    4, // 7B {
    2, // 7C |
    4, // 7D }
    7, // 7E ~
    1 // 7F normally delete, not used yet
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

/**
 * Creates the bitmask for a row of pixels, where a value of %0000 (palette
 * entry 0) means it is transparent.
 * @param pixel_row Row of pixels to extract bitmask from.
 * @return Bitmask for row of pixels.
 */
uint get_bitmask(uint pixel_row);

// ----------------------------- Public Functions -----------------------------

void load_number_tiles()
{
    memcpy32(tile_mem[0] + VRAM_BORDERS, bordersTiles, bordersTilesLen / 4);
    memcpy32(tile_mem_obj[0] + VRAM_NUMBERS, fontTiles + NUMBER_DATA_START,
             10 * 8);
}

void print(const int tile_start, const int x, const int y, const char *text)
{
    TILE *vram_tile = &tile_mem[0][VRAM_TEXT + tile_start];

    // Set up tilemap + clear tile data to be used in tilemap
    int width = 0;
    for (int i = 0; text[i] != '\0'; i++)
    {
        width += font_widths[text[i] - 32];
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
    uint index = 0;
    while (text[index] != '\0')
    {
        const int tile_id = text[index] - 32;
        const uint number_width = font_widths[tile_id];
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
        index++;
    }
}

void print_num(const int tile_start, const int x, const int y, const int number)
{
    int digits[8];
    const int num_digits = count_digits(number, digits);
    char number_string[8];
    for (int i = 0; i < num_digits; i++)
    {
        number_string[i] = digits[i] + 32 + 16;
    }
    number_string[num_digits] = 0;
    print(tile_start, x, y, number_string);
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

void print_statbox()
{
    const int h = 5;
    for (int row = 0; row < h; row++)
    {
        const int x = 0;
        const int w = 25;
        const int y = 15;
        int left = VRAM_BORDERS;
        int middle = VRAM_BORDERS;
        int divider = VRAM_BORDERS;
        int right = VRAM_BORDERS;
        if (row == 0)
        {
            left += TOPL;
            middle += TOP;
            divider += MIDT;
            right += TOPR;
        } else if (row == h - 1)
        {
            left += BOTL;
            middle += BOT;
            divider += MIDB;
            right += BOTR;
        } else
        {
            left += MIDL;
            middle += MID;
            divider += MIDM;
            right += MIDR;
        }
        u16 *vram = &se_mem[TEXT_SBB][(y + row) * 32 + x];
        *vram++ = left | SE_PALBANK(1);
        int count = 0;
        for (int x_off = 1; x_off < w - 1; x_off++)
        {
            int tile = middle;
            if (++count == 8)
            {
                count = 0;
                tile = divider;
            }
            *vram++ = tile | SE_PALBANK(1);
        }
        *vram = right | SE_PALBANK(1);
    }
}

// ----------------------------- Private Functions -----------------------------

uint get_bitmask(const uint pixel_row)
{
    uint row_bitmask = 0xFFFFFFFF; // << (pixel_offset * 4);
    uint bitmask = 0xF;
    for (int j = 0; j < 8; j++)
    {
        if ((pixel_row & bitmask) == 0) row_bitmask &= bitmask ^ 0xFFFFFFFF;
        bitmask <<= 4;
    }
    return row_bitmask;
}

void copy_tile(const uint tile_id, TILE *vram_tile, const uint pixel_offset)
{
    const uint *tile_data = &fontTiles[tile_id * 8];
    for (int i = 0; i < 8; i++)
    {
        const uint pixel_row = tile_data[i] << (pixel_offset * 4);
        const uint mask = get_bitmask(pixel_row);
        vram_tile->data[i] &= mask ^ 0xFFFFFFFF;
        vram_tile->data[i] |= pixel_row & mask;
    }
}

void copy_split_tile(const uint tile_id, TILE *vram_tile,
                     const uint pixel_offset)
{
    // -- Draw left side
    const uint *tile_data = &fontTiles[tile_id * 8];
    for (int j = 0; j < 8; j++)
    {
        const uint pixel_row = tile_data[j] << (pixel_offset * 4);
        const uint mask = get_bitmask(pixel_row);
        vram_tile->data[j] &= mask ^ 0xFFFFFFFF;
        vram_tile->data[j] |= pixel_row & mask;
    }

    // -- Draw right side
    vram_tile++;
    //
    const uint new_offset = 8 - pixel_offset;
    for (int j = 0; j < 8; j++)
    {
        const uint pixel_row = tile_data[j] >> (new_offset * 4);
        const uint mask = get_bitmask(pixel_row);
        vram_tile->data[j] &= mask ^ 0xFFFFFFFF;
        vram_tile->data[j] |= pixel_row & mask;
    }
}
