#include "battle_gfx.h"

#include <tonc.h>

#include "battle_vars.h"
#include "global.h"
#include "math.h"
#include "party.h"
#include "player.h"
#include "text.h"

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
    const int palette = character->is_targeted;
    obj_set_attr(&oam_buf[index],
                 ATTR0_4BPP | ATTR0_SQUARE | fxpt_to_int(character->cur_y),
                 ATTR1_SIZE_32x32 | fxpt_to_int(character->cur_x) | ATTR1_FLIP(
                     character->dir),
                 ATTR2_PALBANK(palette) |
                 ATTR2_PRIO(character->priority) | sprite_id
    );
}

void draw_damage(const int damage, const int x, const int y)
{
    DamageText *dmgText = NULL;
    int oam_index = 0;
    for (int i = 0; i < MAX_DAMAGE_TEXTS; i++)
    {
        dmgText = &damage_texts[i];
        if (dmgText->frames_left == 0) break;
        oam_index += 4;
    }
    dmgText->num_digits = count_digits(damage, dmgText->digits);
    dmgText->x = x;
    dmgText->y = y;
    dmgText->frames_left = DAMAGE_TEXT_FRAMES;

    // Set initial OAM data (will be updated in updated_damage_texts, run in
    //  vblank interrupt)
    for (int i = 0; i < dmgText->num_digits; i++)
    {
        const int digit = dmgText->digits[i];
        obj_set_attr(
            &oam_buf[party_size + enemies_size] + oam_index + i,
            ATTR0_4BPP | ATTR0_SQUARE | fxpt_to_int(y),
            ATTR1_SIZE_8x8 | (fxpt_to_int(x) + i * 7),
            ATTR2_PALBANK(1) |
            ATTR2_PRIO(0) | (VRAM_NUMBERS + digit)
        );
    }
}

void update_damage_texts()
{
    for (int i = 0; i < MAX_DAMAGE_TEXTS; i++)
    {
        DamageText *dmgText = &damage_texts[i];
        const int frame = DAMAGE_TEXT_FRAMES - dmgText->frames_left;
        if (frame < 50)
        {
            dmgText->y -= FIXED_PIXEL * .33;
        }

        for (int j = 0; j < dmgText->num_digits; j++)
        {
            OBJ_ATTR *oam_entry =
                    &oam_buf[party_size + enemies_size] + i * 4 + j;

            if (dmgText->frames_left == 0)
            {
                obj_set_pos(oam_entry, -8, -8);
            } else
            {
                dmgText->frames_left--;
                obj_set_pos(
                    oam_entry,
                    fxpt_to_int(dmgText->x) + j * 7,
                    fxpt_to_int(dmgText->y)
                );
            }
        }
    }
}
