#include "battle_menu.h"
#include <tonc.h>

#include "text.h"

// ------------------ Private Declarations --------------

// ------------------ Public Functions ------------------

BattleMenu battle_start_menu()
{
    print_box(9, 10, 5, 3);
    print_box(15, 10, 5, 3);
    print(0, 10, 11, "FIGHT");
    print(5, 16, 11, "FLEE");
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
    memset32(&se_mem[30], 0, 256);
    return key_pressed;
}

BattleMenu battle_fight_menu()
{
    print_box(11, 6, 6, 3);
    print_box(11, 12, 6, 3);
    print_box(8, 9, 6, 3);
    print_box(14, 9, 6, 3);
    print(0, 9, 10, "Attack"); // left
    print(5, 15, 10, "Spirit"); // right
    print(10, 12, 7, "Item"); // up
    print(15, 12, 13, "Magic"); // down
    int key_pressed = MENU_NONE;
    while (key_pressed == MENU_NONE)
    {
        VBlankIntrWait();
        key_poll();
        if (key_hit(KEY_LEFT) || key_hit(KEY_A))
        {
            key_pressed = MENU_ATTACK;
        }
        else if (key_hit(KEY_RIGHT))
        {
            key_pressed = MENU_SPIRIT;
        }
    }
    unsigned int keys = key_curr_state();
    while (keys)
    {
        VBlankIntrWait();
        key_poll();
        keys = key_curr_state();
    }
    memset32(&se_mem[30], 0, 256);
    return key_pressed;
}
// ------------------ Private Functions -----------------
