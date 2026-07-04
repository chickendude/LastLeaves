#include "battle_gfx.h"

#include <tonc.h>

#include "battle_vars.h"
#include "global.h"
#include "math.h"
#include "player.h"

void draw_sprite(const int index, const BattleCharacter *character)
{
    if (!character->is_alive)
    {
        obj_set_pos(&oam_buf[index], -60, -60);
        return;
    }
    int sprite_id = 0;
    switch (character->character->type)
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
                 ATTR0_4BPP | ATTR0_SQUARE | fxpt_to_int(character->cur_y),
                 ATTR1_SIZE_32x32 | fxpt_to_int(character->cur_x) | ATTR1_FLIP(
                     character->dir),
                 ATTR2_PALBANK(palette) |
                 ATTR2_PRIO(character->priority) | sprite_id
    );
}

