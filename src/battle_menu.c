#include "battle_menu.h"
#include <tonc.h>

#include "battle_gfx.h"
#include "battle_vars.h"
#include "text.h"

// ------------------ Private Declarations --------------

// ------------------ Public Functions ------------------

BattleMenu battle_start_menu()
{
    print_box(8, 10, 6, 3);
    print_box(15, 10, 6, 3);
    print(0, 9, 11, "Fight");
    print(5, 16, 11, "Flee");
    int key_pressed = MENU_NONE;
    while (key_pressed == MENU_NONE)
    {
        VBlankIntrWait();
        key_poll();
        if (key_hit(KEY_LEFT) || key_hit(KEY_A))
        {
            key_pressed = MENU_FIGHT;
        }
        else if (key_hit(KEY_RIGHT))
        {
            key_pressed = MENU_FLEE;
        }
    }
    unsigned int keys = key_curr_state();
    while (keys)
    {
        VBlankIntrWait();
        key_poll();
        keys = key_curr_state();
    }
    memset32(&se_mem[30], 0, 224);
    return key_pressed;
}

// Coords for the battle menu, left-most coord is BM_X, top-most coord is BM_Y
#define BM_X 8
#define BM_Y 6
#define BM_W 7
BattleMenu battle_fight_menu()
{
    print_box(BM_X, BM_Y + 3, BM_W, 3); // left
    print_box(BM_X + BM_W + 1, BM_Y + 3, BM_W, 3); // right
    print_box(BM_X + BM_W / 2, BM_Y, BM_W, 3); // up
    print_box(BM_X + BM_W / 2, BM_Y + 6, BM_W, 3); // down
    print(0, BM_X + 1, BM_Y + 4, "Attack"); // left
    print(5, BM_X + BM_W + 2, BM_Y + 4, "Magic"); // right
    print(10, BM_X + BM_W / 2 + 1, BM_Y + 1, "Item"); // up
    print(15, BM_X + BM_W / 2 + 1, BM_Y + 7, "Focus"); // down
    int key_pressed = MENU_NONE;
    while (key_pressed == MENU_NONE)
    {
        VBlankIntrWait();
        key_poll();
        if (key_hit(KEY_LEFT) || key_hit(KEY_A))
        {
            key_pressed = MENU_ATTACK;
        }
        else if (key_hit(KEY_UP))
        {
            key_pressed = MENU_ITEM;
        } else if (key_hit(KEY_RIGHT))
        {
            key_pressed = MENU_MAGIC;
        } else if (key_hit(KEY_DOWN))
        {
            key_pressed = MENU_SPIRIT;
        } else if (key_hit(KEY_B))
        {
            break;
        }
    }
    unsigned int keys = key_curr_state();
    while (keys)
    {
        VBlankIntrWait();
        key_poll();
        keys = key_curr_state();
    }
    memset32(&se_mem[30], 0, 240);
    return key_pressed;
}

MenuResult select_attack_menu(BattleCharacter *character)
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

MenuResult select_target(int* target_enemy_index)
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
// ------------------ Private Functions -----------------