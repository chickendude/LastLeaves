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

/**
 * Input your attack directions for this character.
 * @returns RESULT_OK if selection made, RESULT_CANCEL if action was cancelled
 */
MenuResult select_attack(BattleCharacter* character, int* target_enemy_index);

MenuResult select_target(BattleCharacter* character, int* target_enemy_index);

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
    REG_BG0CNT = BG_CBB(0) | BG_SBB(SBB) | BG_PRIO(3) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG3CNT = BG_CBB(0) | BG_SBB(30) | BG_PRIO(1) | BG_REG_32x32 | BG_4BPP;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;

    draw_map();
    initialize_parties();
    load_number_tiles();
    load_battlebar_tiles();
    irq_add(II_VBLANK, battle_vblank);
    start_battle();
    irq_delete(II_VBLANK);
    // TODO handle win (2) vs lose (1) state
}

// --------------- private functions -------------------

int start_battle()
{
    clear_battle_queue();
    // The currently targeted enemy
    int target_enemy_index = 0;
    int battle_over = 0;
    while (!battle_over)
    {
        show_statbox();
        const BattleMenu selection = battle_start_menu();
        if (selection == MENU_FLEE)
        {
            battle_over = 2;
            break;
        }
        int player_index = 0;
        while (player_index >= 0 && player_index < party_size)
        {
            BattleCharacter* character = &battle_party[player_index];
            if (!character->is_alive)
            {
                player_index++;
                continue;
            }

            // Move player forward to show they are attacking
            character->cur_x -= FIXED_PIXEL * 8;
            character->cur_y -= FIXED_PIXEL * 4;

            // Show the fight/flee menu
            const BattleMenu fight_selection = battle_fight_menu();

            MenuResult result;
            switch (fight_selection)
            {
                case MENU_ATTACK:
                    // If B was pressed, restart selection
                    result = select_attack(character, &target_enemy_index);
                    if (result == RESULT_OK)
                    {
                        result = select_target(character, &target_enemy_index);
                        if (result == RESULT_OK)
                        {
                            queue_add_action(AT_MOVE, character, &enemies[target_enemy_index], enemies, enemies_size);
                            queue_add_action(AT_ATTACK, character, &enemies[target_enemy_index], enemies, enemies_size);
                            queue_add_action(AT_RETURN, character, NULL, NULL, 0);
                            player_index++;
                        }
                    }
                    break;
                case MENU_ITEM:
                case MENU_SPIRIT:
                case MENU_MAGIC:
                case MENU_NONE: // Battle Menu was cancelled
                    player_index--;
                default:
                    break;
            }
            // Move player back
            character->cur_x += FIXED_PIXEL * 8;
            character->cur_y += FIXED_PIXEL * 4;
        }
        if (player_index < 0) continue;
        select_enemy_attacks();
        perform_battle_queue();
        clear_battle_queue();
        // Check if all enemies or players are dead
        if (are_all_dead(enemies, enemies_size)) battle_over = 1;
        else if (are_all_dead(battle_party, party_size)) battle_over = 2;
    }
    return battle_over;
}

MenuResult select_target(BattleCharacter* character, int* target_enemy_index)
{
    int success = RESULT_NONE;
    // Wait for attack directions to be made
    while (true)
    {
        key_poll();
        VBlankIntrWait();

        // Check keys

        if (key_hit(KEY_LEFT))
        {
            (*target_enemy_index)--;
        } else if (key_hit(KEY_RIGHT))
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
            enemies[*target_enemy_index].is_targeted = false;
            success = RESULT_OK;
            break;
        }
        if (key_hit(KEY_B))
        {
            success = RESULT_CANCEL;
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

MenuResult select_attack(BattleCharacter* character, int* target_enemy_index)
{
    draw_battlebar(character);
    // Prefill attack bar if we have a previous attack
    if (character->attack_combo[0] != ATK_NONE)
    {
        draw_attack_sprites(character->attack_combo);
    }
    int success = RESULT_NONE;
    int command_index = 0;
    // Wait for attack directions to be made
    while (true)
    {
        key_poll();

#ifdef DEBUG
        int *sta = &character->disp_sta;
        if (key_is_down(KEY_R))
        {
            if (*sta < MAX_COMBO * 7) (*sta)++;
            clear_battlebar();
            draw_battlebar(character);
            show_statbox();
        } else if (key_is_down(KEY_L))
        {
            if (*sta > 10) (*sta)--;
            clear_battlebar();
            draw_battlebar(character);
            show_statbox();
        }
#endif

        VBlankIntrWait();

        // Check keys

        bool attack_added = false;
        if (key_hit(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN) && command_index == 0)
        {
            clear_attackbar_sprites();
        }
        if (key_hit(KEY_LEFT))
        {
            attack_added = add_attack(character, ATK_LEFT, command_index);
        } else if (key_hit(KEY_RIGHT))
        {
            attack_added = add_attack(character, ATK_RIGHT, command_index);
        } else if (key_hit(KEY_UP))
        {
            attack_added = add_attack(character, ATK_HIGH, command_index);
        } else if (key_hit(KEY_DOWN))
        {
            attack_added = add_attack(character, ATK_LOW, command_index);
        }
        if (attack_added) command_index++;

        // Make sure there is at least one attack selected
        if (key_hit(KEY_A) && character->attack_combo[0] != ATK_NONE)
        {
            success = RESULT_OK;
            break;
        }

        // Clear the combo if attacks have been input, otherwise cancel the menu
        if (key_hit(KEY_B))
        {
            if (command_index > 0)
            {
                remove_last_attack(character);
                command_index--;
            } else if (command_index == 0 &&
                       character->attack_combo[0] != ATK_NONE)
            {
                character->attack_combo[0] = ATK_NONE;
                clear_attackbar_sprites();
            } else
            {
                success = RESULT_CANCEL;
                break;
            }
        }
    }
    clear_battlebar();
    clear_attackbar_sprites();
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
    oam_copy(oam_mem, oam_buf, 128);
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
        battle_party[i].disp_mp = party[i].stats.mp;
        battle_party[i].disp_sta = party[i].stats.sta;
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
        enemy->disp_sta = enemy->character->stats.sta;
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
        const int stamina = enemies[i].disp_sta;
        for (int j = 0; j < MAX_COMBO; j++)
        {
            if (j * 7 > stamina) break;
            enemies[i].attack_combo[j] = random(4) + 1;
        }
        queue_add_action(AT_MOVE, &enemies[i], &battle_party[target], battle_party, party_size);
        queue_add_action(AT_ATTACK, &enemies[i], &battle_party[target], battle_party, party_size);
        queue_add_action(AT_RETURN, &enemies[i], NULL, NULL, 0);
    }
}

void show_statbox()
{
    print_statbox();
    for (int i = 0; i < party_size; i++)
    {
        const int tile_start = MENU_TILES_OCCUPIED + i * 19;
        const int x = i * 8;
        const int y = 16;
        // print_box(i * 8, 16, 8, 4);
        print(tile_start, x + 1, y, battle_party[i].character->name);
        print_num(tile_start + 5, x + 1, y + 1, battle_party[i].disp_hp);
        print_num(tile_start + 9, x + 5, y + 1,
                  battle_party[i].character->stats.max_hp);
        print_num(tile_start + 13, x + 1, y + 2, battle_party[i].disp_mp);
        print_num(tile_start + 17, x + 5, y + 2,
                  battle_party[i].character->stats.max_mp);
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
