#include <tonc.h>
#include "text.h"

#include "math.h"

#include "borders.h"
#include "numbers.h"

#define VRAM_BORDERS 1
#define VRAM_NUMBERS 12
#define TEXT_SBB 30

enum
{
    TOPL, TOP, TOPR, MIDL, MID, MIDR, BOTL, BOT, BOTR
};

// ----------------------- Private Function Definitions -----------------------

// ----------------------------- Public Functions -----------------------------

// TODO: Switch to drawing on tiles directly
void print_num(const int tile_start, const int x, const int y, const int number)
{
    int digits[8];
    const int num_digits = count_digits(number, digits);

    memcpy32(tile_mem[0] + VRAM_BORDERS, bordersTiles, bordersTilesLen);
    memcpy32(tile_mem[0] + VRAM_NUMBERS, numbersTiles, numbersTilesLen);

    for (int i = 0; i < num_digits; i++)
    {
        const int tile_id = digits[i] + VRAM_NUMBERS;
        se_mem[TEXT_SBB][y * 32 + x + i] = tile_id | SE_PALBANK(1);
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
