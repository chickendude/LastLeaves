#include <tonc.h>
#include "battle.h"

#include "battlemap.h"
#include "global.h"
#include "player.h"
#include "maps/battle/map_battle.h"
#include "lynne-battle.h"
#include "roak-battle.h"
#include "tann-battle.h"

/** Screen Block for battle map. */
#define SBB 29

typedef struct BattleCharacter
{
    Player character;
    /** 0 = left, 1 = right */
    int dir;
    /** Player's home X position on. */
    int x;
    /** Where player X currently is on screen. */
    int cur_x;
    /** X velocity. */
    int vel_x;
    /** Player's home Y position on. */
    int y;
    /** Where player Y currently is on screen. */
    int cur_y;
    /** Y velocity. */
    int vel_y;
    int is_alive;
    /** Is being targeted for an attack. */
    int is_targeted;
} BattleCharacter;

void draw_map(void);

void draw_sprite(int index, BattleCharacter *character);

void select_attack(BattleCharacter *character, int *target_enemy_index);

void start_battle(void);

BattleCharacter party[3] = {
    {.character = {.type = TANN},},
    {.character = {.type = LYNNE},},
    {.character = {.type = ROAK},},
};
BattleCharacter enemies[3] = {
    {},
    {},
    {},
};

// Number of players in party
int party_size = 3;

// Number of enemies in the battle
int enemies_size = 3;

// --------------- public functions -------------------

void battle()
{
    // Enable mode 0 (4 layers) and onlyshow BG0
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(SBB) | BG_PRIO(2) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    // TODO: Load character and enemy bitmaps based off of party and enemy list

    draw_map();
    start_battle();
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
    memcpy32(tile_mem[4], tann_battleTiles, tann_battleTilesLen / 4);
    memcpy32(tile_mem[4] + 16, roak_battleTiles, roak_battleTilesLen / 4);
    memcpy32(tile_mem[4] + 32, lynne_battleTiles, lynne_battleTilesLen / 4);
    memcpy32(pal_bg_mem, battlemapPal, battlemapPalLen / 4);
    memcpy32(pal_obj_mem, tann_battlePal, tann_battlePalLen / 4);
    for (int row = 0; row < 10; row++)
    {
        for (int x = 0; x < 15; x++)
        {
            int y = row * 2 * 32;
            int tile = map_battle[row * 15 + x] * 4;
            se_mem[SBB][y + x * 2] = tile;
            se_mem[SBB][y + x * 2 + 1] = tile + 1;
            se_mem[SBB][y + 32 + x * 2] = tile + 2;
            se_mem[SBB][y + 32 + x * 2 + 1] = tile + 3;
        }
    }
}

void draw_sprite(const int index, BattleCharacter *character)
{
    int sprite_id = 0;
    switch (character->character.type)
    {
        case TANN:
            sprite_id = 0;
            break;
        case ROAK:
            sprite_id = 16;
            break;
        case LYNNE:
            sprite_id = 32;
            break;
    }
    int palette = character->is_targeted;
    obj_set_attr(&oam_buf[index],
                 ATTR0_4BPP | ATTR0_SQUARE | character->cur_y >> 8,
                 ATTR1_SIZE_32x32 | character->cur_x >> 8 | ATTR1_FLIP(
                     character->dir),
                 ATTR2_PALBANK(palette) |
                 ATTR2_PRIO(2) | sprite_id
    );
}

void start_battle()
{
    for (int i = 0; i < party_size; i++)
    {
        party[i].is_alive = 1;
        party[i].x = (180 + i * 10) << 8;
        party[i].cur_x = party[i].x;
        party[i].y = (120 - i * 15) << 8;
        party[i].cur_y = party[i].y;
    }
    for (int i = 0; i < enemies_size; i++)
    {
        enemies[i].is_alive = 1;
        enemies[i].dir = 1;
        enemies[i].x = (20 + i * 10) << 8;
        enemies[i].cur_x = enemies[i].x;
        enemies[i].y = (80 - i * 25) << 8;
        enemies[i].cur_y = enemies[i].y;
    }

    // Which player is currently selecting moves, 0 < active_player < party_size
    int active_player_index = 0;
    int target_enemy_index = 0;
    int battle_over = 0;
    while (!battle_over)
    {
        key_poll();
        VBlankIntrWait();
        oam_copy(oam_mem, oam_buf, 6);
        if (active_player_index >= party_size)
        {
            // Select enemy turns
        } else
        {
            select_attack(&party[active_player_index], &target_enemy_index);
            if (target_enemy_index < 0) target_enemy_index = enemies_size - 1;
            if (target_enemy_index >= enemies_size) target_enemy_index = 0;
        }

        // Update players
        for (int i = 0; i < party_size; i++)
        {
            draw_sprite(i, &party[i]);
        }

        // Update enemies
        for (int i = 0; i < enemies_size; i++)
        {
            BattleCharacter *enemy = &enemies[i];
            enemy->is_targeted = i == target_enemy_index;
            draw_sprite(i + party_size, enemy);
        }
    }
}

void select_attack(BattleCharacter *character, int *target_enemy_index)
{
    // Simple animation to show which character is selected
    character->cur_y -= character->vel_y;
    const int off_y = character->y - character->cur_y;
    character->vel_y += character->vel_y;
    if (off_y >> 8 > 5)
    {
        character->vel_y = -25;
    } else if (off_y <= 0)
    {
        character->vel_y = 25;
    }

    // Check keys

    if (key_hit(KEY_LEFT))
    {
        (*target_enemy_index)--;
    }
    if (key_hit(KEY_RIGHT))
    {
        (*target_enemy_index)++;
    }
}
