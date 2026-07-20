#include <tonc.h>
#include "battle.h"

#include "battlemap.h"
#include "battle_actions.h"
#include "battle_gfx.h"
#include "battle_menu.h"
#include "battle_vars.h"
#include "enemy_data.h"
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
int select_attack(BattleCharacter* character, int* target_enemy_index);

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
int are_all_dead(const BattleCharacter* characters, int size);

/**
 * Adjusts a player character's display HP to shift towards their current HP.
 * @param character The (player) character whose display HP should be adjusted.
 */
bool update_hp(BattleCharacter* character);

void load_enemy_data(Player* enemy, const PlayerData* enemy_data);

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
    int target_enemy_index = 0;
    int battle_over = 0;
    while (!battle_over)
    {
        show_statbox();
        BattleMenu selection = battle_start_menu();
        if (selection == MENU_FLEE)
        {
            battle_over = 2;
            break;
        }
        int player_index = 0;
        while (player_index < party_size)
        {
            BattleCharacter* character = &battle_party[player_index];
            if (!character->is_alive) continue;

            const BattleMenu selection = battle_fight_menu();
            switch (selection)
            {
            case MENU_ATTACK:
                // If B was pressed, restart selection
                if (select_attack(character, &target_enemy_index)) player_index++;
                break;
            case MENU_ITEM:
            case MENU_SPIRIT:
            case MENU_MAGIC:
            default:
                break;
            }
        }
        select_enemy_attacks();
        perform_battle_queue();
        clear_battle_queue();
        // Check if all enemies or players are dead
        if (are_all_dead(enemies, enemies_size)) battle_over = 1;
        else if (are_all_dead(battle_party, party_size)) battle_over = 2;
    }
    return battle_over;
}

int select_attack(BattleCharacter* character, int* target_enemy_index)
{
    int success = 0;
    while (true)
    {
        key_poll();
        VBlankIntrWait();
        // Simple animation to show which character is selected
        // TODO: Not really needed anymore
        character->cur_y -= character->vel_y;
        const int off_y = character->y - character->cur_y;
        character->vel_y += character->vel_y;
        if (fxpt_to_int(off_y) > 5)
        {
            character->vel_y = -25;
        }
        else if (off_y <= 0)
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
            enemies[*target_enemy_index].is_targeted = false;
            success = 1;
            break;
        }
        if (key_hit(KEY_B))
        {
            success = 0;
            break;
        }
        // Check if enemy's getting selected in player attack selection
        for (int i = 0; i < enemies_size; i++)
        {
            enemies[i].is_targeted = i == *target_enemy_index;
        }
    }
    // Clear target for all enemies
    for (int j = 0; j < enemies_size; j++)
    {
        enemies[j].is_targeted = false;
    }
    return success;
}

void battle_vblank(void)
{
    // Update player HPs
    bool hp_changed = false;
    for (int i = 0; i < party_size; i++)
    {
        hp_changed |= update_hp(&battle_party[i]);
    }

    // If HP changed, update the stat box to show it
    if (hp_changed) show_statbox();

    update_damage_texts();
    oam_copy(oam_mem, oam_buf, 30);
    // Update player sprites
    for (int i = 0; i < party_size; i++)
    {
        draw_sprite(i, &battle_party[i]);
    }

    // Update enemy sprites
    for (int i = 0; i < enemies_size; i++)
    {
        draw_sprite(i + party_size, &enemies[i]);
    }
}

void initialize_parties()
{
    for (int i = 0; i < party_size; i++)
    {
        battle_party[i].frame_cycle = 0;
        battle_party[i].frame_index = 0;
        battle_party[i].animation = &party[i].graphics->idle;
        battle_party[i].index = i;
        battle_party[i].is_alive = 1;
        battle_party[i].x = fxpt(188 + i * 10);
        battle_party[i].cur_x = battle_party[i].x;
        battle_party[i].y = fxpt(110 - i * 28);
        battle_party[i].cur_y = battle_party[i].y;
        battle_party[i].character = &party[i];
        battle_party[i].disp_hp = party[i].stats.hp;
        draw_sprite(i, &battle_party[i]);
    }
    for (int i = 0; i < enemies_size; i++)
    {
        BattleCharacter* enemy = &enemies[i];
        load_enemy_data(enemies[i].character, &enemy_data[i]);
        enemy->animation = &enemy->character->graphics->idle;
        enemy->index = i + party_size;
        enemy->is_alive = 1;
        enemy->dir = 1;
        enemy->x = fxpt(20 + i * 10);
        enemy->cur_x = enemies[i].x;
        enemy->y = fxpt(80 - i * 25);
        enemy->cur_y = enemies[i].y;
        enemy->character->stats.hp = 50;
        enemy->character->stats.max_hp = 50;
        enemy->disp_hp = 50;
        enemy->frame_index = i;
        // Set frame cycle to zero first so that the sprite data gets loaded
        enemy->frame_cycle = 0;
        draw_sprite(i, enemy);
        enemy->frame_cycle = 6 * i;
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
        const int tile_start = MENU_TILES_OCCUPIED + i * 16;
        const int x = i * 8;
        const int y = 17;
        print_box(i * 8, 16, 8, 4);
        print(tile_start, x + 1, y, battle_party[i].character->name);
        print_num(tile_start + 5, x + 1, y + 1, battle_party[i].disp_hp);
        print_num(tile_start + 9, x + 4, y + 1,
                  battle_party[i].character->stats.max_hp);
    }
}

int are_all_dead(const BattleCharacter* characters, const int size)
{
    int num_dead = 0;
    for (int i = 0; i < size; i++)
    {
        if (!characters[i].is_alive) num_dead++;
    }
    return num_dead == size;
}

bool update_hp(BattleCharacter* character)
{
    bool changed = 0;
    if (character->disp_hp < character->character->stats.hp)
    {
        character->disp_hp++;
        changed = true;
    }
    else if (character->disp_hp > character->character->stats.hp)
    {
        character->disp_hp--;
        changed = true;
    }
    return changed;
}

void draw_map()
{
    memset32(tile_mem, 0, TILE_OFFSET * 8);
    memcpy32(tile_mem[0] + TILE_OFFSET, battlemapTiles,
             sizeof(battlemapTiles) / 4);
    memcpy32(pal_bg_mem, battlemapPal, battlemapPalLen / 4);
    memcpy32(pal_obj_mem, tann_battlePal, tann_battlePalLen / 4);
    for (int row = 0; row < 10; row++)
    {
        for (int x = 0; x < 15; x++)
        {
            const int y = row * 2 * 32;
            const int tile = map_battle[row * 15 + x] * 4 + TILE_OFFSET;
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
        BattleCharacter* character = &battle_party[i];
        character->priority = 2;
        character->vel_y = 0;
        character->vel_x = 0;
    }
    // Update enemies
    for (int i = 0; i < enemies_size; i++)
    {
        BattleCharacter* enemy = &enemies[i];
        enemy->priority = 2;
        enemy->vel_y = 0;
        enemy->vel_x = 0;
    }
}

void load_enemy_data(Player* enemy, const PlayerData* enemy_data)
{
    memcpy16(enemy->name, enemy_data->name, 10 / 2);
    enemy->graphics = enemy_data->graphics;
    // TODO: Not sure if we really need this or not, currently type tells us which of the main characters it is
    // enemy->type = type;
    enemy->stats.lvl = 1;
    enemy->stats.hp = enemy_data->max_hp;
    enemy->stats.max_hp = enemy_data->max_hp;
    enemy->stats.mp = enemy_data->max_mp;
    enemy->stats.max_mp = enemy_data->max_mp;
    enemy->stats.cp = enemy_data->max_cp;
    enemy->stats.max_cp = enemy_data->max_cp;
    enemy->stats.atk = enemy_data->atk;
    enemy->stats.def = enemy_data->def;
    enemy->stats.mag = enemy_data->mag;
    enemy->stats.sta = enemy_data->sta;
    enemy->stats.spd = enemy_data->spd;
}
