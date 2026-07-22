#include "battle_menu.h"
#include <tonc.h>

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
    memset32(&se_mem[30], 0, 256);
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
