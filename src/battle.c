#include <tonc.h>
#include "battle.h"

#include <string.h>

#include "battlemap.h"
#include "maps/battle/map_battle.h"

void draw_map(void);

// --------------- public functions -------------------
void battle()
{
    // Enable mode 0 (4 layers) and onlyshow BG0
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(29) | BG_PRIO(2) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    draw_map();
    unsigned int keys = key_curr_state();
    while (!keys)
    {
        key_poll();
        keys = key_curr_state();
    }
    while (keys)
    {
        key_poll();
        keys = key_curr_state();
    }
}

// --------------- private functions -------------------
void draw_map()
{
    memcpy32(tile_mem, battlemapTiles, sizeof(battlemapTiles) / 4);
    memcpy32(pal_bg_mem, battlemapPal, battlemapPalLen / 4);
    for (int row = 0; row < 10; row++)
    {
        for (int x = 0; x < 15; x++)
        {
            int y = row * 2 * 32;
            int tile = map_battle[row * 15 + x] * 4;
            se_mem[29][y + x * 2] = tile;
            se_mem[29][y + x * 2 + 1] = tile + 1;
            se_mem[29][y + 32 + x * 2] = tile + 2;
            se_mem[29][y + 32 + x * 2 + 1] = tile + 3;
        }
    }
}
