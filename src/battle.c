#include <tonc.h>
#include "battle.h"

#include "battlemap.h"
#include "battle_actions.h"
#include "battle_gfx.h"
#include "battle_vars.h"
#include "global.h"
#include "math.h"
#include "player.h"
#include "maps/battle/map_battle.h"
#include "lynne-battle.h"
#include "party.h"
#include "roak-battle.h"
#include "tann-battle.h"
#include "text.h"

/** Screen Block for battle map. */
#define SBB 29
/** First tile of battle tilemap (sprite/image) data in tile_mem. */
#define TILE_OFFSET 328

void battle_vblank(void);

void draw_map(void);

void initialize_parties(void);

void clear_battle_queue(void);

/** Select an attack.
 * @returns 0 = no attack selected, 1 = attack selected
 */
int select_attack(BattleCharacter *character, int *target_enemy_index);

void select_enemy_attacks();

/**
 * Runs main battle logic.
 * @returns 1 if all enemies are dead, 2 if all players are dead.
 */
int start_battle(void);

void show_statbox(void);

/**
 * Checks if all members in a party (player or enemy) are dead.
 * @returns 1 if they are all dead, 0 if at least one member is still alive.
 */
int are_all_dead(const BattleCharacter *characters, int size);

/**
 * Adjusts a player character's display HP to shift towards their current HP.
 * @param character The (player) character whose display HP should be adjusted.
 */
void update_hp(BattleCharacter *character);

// --------------- public functions -------------------

void battle()
{
    // Enable mode 0 (4 layers) and only show BG0 for map and BG3 for text
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(SBB) | BG_PRIO(2) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG3CNT = BG_CBB(0) | BG_SBB(30) | BG_PRIO(0) | BG_REG_32x32 | BG_4BPP;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;

    // TODO: Load character and enemy bitmaps based off of party and enemy list

    draw_map();
    initialize_parties();
    load_number_tiles();
    irq_add(II_VBLANK, battle_vblank);
    start_battle();
    irq_delete(II_VBLANK);
    // TODO handle win (2) vs lose (1) state
}

// --------------- private functions -------------------

int start_battle()
{
    clear_battle_queue();
    // Which player is currently selecting moves, 0 < active_player < party_size
    int active_player_index = 0;
    int target_enemy_index = 0;
    int battle_over = 0;
    while (!battle_over)
    {
        // Run through random numbers while waiting
        random(256);
        key_poll();
        VBlankIntrWait();
        oam_copy(oam_mem, oam_buf, 6);

        // Handle enemy turn
        if (active_player_index >= party_size)
        {
            select_enemy_attacks();
            perform_battle_queue();
            active_player_index = 0;
            clear_battle_queue();
        } else
        // Handle player turn
        {
            if (select_attack(&battle_party[active_player_index],
                              &target_enemy_index))
            {
                active_player_index++;
                target_enemy_index = 0;
            }
            if (target_enemy_index < 0) target_enemy_index = enemies_size - 1;
            if (target_enemy_index >= enemies_size) target_enemy_index = 0;
        }

        // Update player sprites
        for (int i = 0; i < party_size; i++)
        {
            draw_sprite(i, &battle_party[i]);
        }

        // Update enemy sprites
        for (int i = 0; i < enemies_size; i++)
        {
            BattleCharacter *enemy = &enemies[i];
            enemy->is_targeted = i == target_enemy_index;
            draw_sprite(i + party_size, enemy);
        }

        // Check if all enemies or players are dead
        if (are_all_dead(enemies, enemies_size)) battle_over = 1;
        else if (are_all_dead(battle_party, party_size)) battle_over = 2;
    }
    return battle_over;
}

int select_attack(BattleCharacter *character, int *target_enemy_index)
{
    if (!character->is_alive) return 1;

    // Simple animation to show which character is selected
    character->cur_y -= character->vel_y;
    const int off_y = character->y - character->cur_y;
    character->vel_y += character->vel_y;
    if (fxpt_to_int(off_y) > 5)
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
    if (*target_enemy_index < 0) *target_enemy_index = enemies_size - 1;
    for (int i = 0; i < enemies_size; i++)
    {
        if (enemies[*target_enemy_index].is_alive) break;
        (*target_enemy_index)++;
        if (*target_enemy_index >= enemies_size) *target_enemy_index = 0;
    }
    if (key_hit(KEY_A))
    {
        queue_add_action(AT_MOVE, character, &enemies[*target_enemy_index]);
        queue_add_action(AT_ATTACK, character, &enemies[*target_enemy_index]);
        queue_add_action(AT_RETURN, character, NULL);
        return 1;
    }
    return 0;
}

void battle_vblank(void)
{
    // Update player HPs
    for (int i = 0; i < party_size; i++) update_hp(&battle_party[i]);
    show_statbox();
    oam_copy(oam_mem, oam_buf, 6);
}

void initialize_parties()
{
    for (int i = 0; i < party_size; i++)
    {
        battle_party[i].is_alive = 1;
        battle_party[i].x = fxpt(180 + i * 10);
        battle_party[i].cur_x = battle_party[i].x;
        battle_party[i].y = fxpt(120 - i * 15);
        battle_party[i].cur_y = battle_party[i].y;
        battle_party[i].character = &party[i];
        battle_party[i].disp_hp = party[i].stats.hp;
    }
    for (int i = 0; i < enemies_size; i++)
    {
        enemies[i].is_alive = 1;
        enemies[i].dir = 1;
        enemies[i].x = fxpt(20 + i * 10);
        enemies[i].cur_x = enemies[i].x;
        enemies[i].y = fxpt(80 - i * 25);
        enemies[i].cur_y = enemies[i].y;
        enemies[i].character->stats.hp = 50;
        enemies[i].character->stats.max_hp = 50;
        enemies[i].disp_hp = 50;
    }
}

void select_enemy_attacks()
{
    for (int i = 0; i < enemies_size; i++)
    {
        if (!enemies[i].is_alive) continue;

        int target = random(party_size);
        while (!battle_party[target].is_alive)
        {
            target++;
            if (target >= party_size) target = 0;
        }
        queue_add_action(AT_MOVE, &enemies[i], &battle_party[target]);
        queue_add_action(AT_ATTACK, &enemies[i], &battle_party[target]);
        queue_add_action(AT_RETURN, &enemies[i], NULL);
    }
}

void show_statbox()
{
    for (int i = 0; i < party_size; i++)
    {
        const int tile_start = i * 8;
        const int x = i * 8;
        print_box(i * 8, 16, 8, 4);
        print_num(tile_start, x + 1, 17, battle_party[i].disp_hp);
        print_num(tile_start + 4, x + 4, 17,
                  battle_party[i].character->stats.max_hp);
    }
}

int are_all_dead(const BattleCharacter *characters, const int size)
{
    int num_dead = 0;
    for (int i = 0; i < size; i++)
    {
        if (!characters[i].is_alive) num_dead++;
    }
    return num_dead == size;
}

void update_hp(BattleCharacter *character)
{
    if (character->disp_hp < character->character->stats.hp)
    {
        character->disp_hp++;
    } else if (character->disp_hp > character->character->stats.hp)
    {
        character->disp_hp--;
    }
}

void draw_map()
{
    memset32(tile_mem, 0, TILE_OFFSET * 8);
    memcpy32(tile_mem[0] + TILE_OFFSET, battlemapTiles,
             sizeof(battlemapTiles) / 4);
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
            int tile = map_battle[row * 15 + x] * 4 + TILE_OFFSET;
            se_mem[SBB][y + x * 2] = tile;
            se_mem[SBB][y + x * 2 + 1] = tile + 1;
            se_mem[SBB][y + 32 + x * 2] = tile + 2;
            se_mem[SBB][y + 32 + x * 2 + 1] = tile + 3;
        }
    }
}

void clear_battle_queue()
{
    battle_queue_index = 0;
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        battle_queue[i].type = AT_NONE;
    }
    for (int i = 0; i < party_size; i++)
    {
        BattleCharacter *character = &battle_party[i];
        character->priority = 2;
        character->vel_y = 0;
        character->vel_x = 0;
    }
    // Update enemies
    for (int i = 0; i < enemies_size; i++)
    {
        BattleCharacter *enemy = &enemies[i];
        enemy->priority = 2;
        enemy->vel_y = 0;
        enemy->vel_x = 0;
    }
}
