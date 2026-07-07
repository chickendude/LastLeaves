#include "battle_gfx.h"

#include <tonc.h>

#include "battle_vars.h"
#include "global.h"
#include "math.h"
#include "party.h"
#include "player.h"
#include "text.h"

// --------------- public functions -------------------

void draw_sprite(const int index, BattleCharacter *character)
{
    const Animation *animation = character->animation;
    if (character->frame_cycle++ == 0)
    {
        memcpy32(tile_mem[4] + character->index * 16, animation->sprite_data + character->frame_index*128, 32*4);
    }
    if (character->frame_cycle >= animation->frame_cycles[character->frame_index])
    {
        character->frame_index++;
        if (character->frame_index >= animation->num_frames) character->frame_index = 0;
        character->frame_cycle = 0;
    }
    if (!character->is_alive)
    {
        obj_set_pos(&oam_buf[index], -60, -60);
        return;
    }
    const int palette = character->is_targeted;
    obj_set_attr(&oam_buf[index],
                 ATTR0_4BPP | ATTR0_SQUARE | fxpt_to_int(character->cur_y),
                 ATTR1_SIZE_32x32 | fxpt_to_int(character->cur_x) | ATTR1_FLIP(
                     character->dir),
                 ATTR2_PALBANK(palette) |
                 ATTR2_PRIO(character->priority) | (index * 16)
    );
}

void draw_damage(const int damage, const int x, const int y)
{
    DamageText *dmgText = NULL;
    int index;
    for (index = 0; index < MAX_DAMAGE_TEXTS; index++)
    {
        dmgText = &damage_texts[index];
        if (dmgText->frames_left <= 0) break;
    }
    dmgText->num_digits = count_digits(damage, dmgText->digits);
    dmgText->x = x;
    dmgText->y = y;
    dmgText->frames_left = DAMAGE_TEXT_FRAMES;
    dmgText->affine_mat = (OBJ_AFFINE *) &oam_buf[index * 4];

    // Set initial OAM data (will be updated in updated_damage_texts, run in
    //  vblank interrupt)
    for (int i = 0; i < dmgText->num_digits; i++)
    {
        const int digit = dmgText->digits[i];
        obj_set_attr(
            &oam_buf[party_size + enemies_size + index * 4 + i],
            ATTR0_4BPP | ATTR0_SQUARE | ATTR0_AFF_DBL | fxpt_to_int(y),
            ATTR1_SIZE_8x8 | ATTR1_AFF_ID(index) | (fxpt_to_int(x) + i * 7),
            ATTR2_PALBANK(1) | ATTR2_PRIO(0) | (VRAM_NUMBERS + digit)
        );
    }
}

void update_damage_texts()
{
    for (int i = 0; i < MAX_DAMAGE_TEXTS; i++)
    {
        // If frames_left == 0, we need to clear the sprites out
        // If it's < 0, sprites have been cleared out and we can skip it
        DamageText *dmgText = &damage_texts[i];
        if (dmgText->frames_left < 0) continue;

        const int frame = DAMAGE_TEXT_FRAMES - dmgText->frames_left;

        // Make the text rise for the first 15 frames
        if (frame < 15) dmgText->y -= FIXED_PIXEL * .8;

        // Scale the text in. Larger number means the sprite will be smaller.
        // They are in FPN, so 1<<8 means no transformation, 2<<8 = .5x zoom
        // (it's half the normal size), 1<<7 = 2x zoom (twice normal size).
        int scale = (2 << 8) - frame * .10 * (1<<8);
        if (scale < 1<<8) scale = 1<<8;

        // With the zoom, we want the number sprites to start off closer and
        // gradually move away from each other.
        int separation = frame >> 1;
        if (separation > 6) separation = 6;

        for (int j = 0; j < dmgText->num_digits; j++)
        {
            OBJ_ATTR *oam_entry =
                    &oam_buf[party_size + enemies_size + i * 4] + j;

            if (dmgText->frames_left == 0)
            {
                obj_set_pos(oam_entry, -8, -8);
            } else
            {
                obj_aff_scale(dmgText->affine_mat, scale, scale);
                obj_set_pos(
                    oam_entry,
                    fxpt_to_int(dmgText->x) + j * separation - separation / 2,
                    fxpt_to_int(dmgText->y)
                );
            }
        }
        dmgText->frames_left--;
    }
}

// --------------- private functions -------------------
