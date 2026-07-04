#include "battle_actions.h"

#include <tonc.h>

#include "battle_gfx.h"
#include "battle_vars.h"
#include "global.h"
#include "party.h"

void perform_attacks()
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        const BattleAction *action = &battle_queue[i];
        if (!action->actor->is_alive) continue;
        switch (action->type)
        {
            case AT_ATTACK:

                action->actor->cur_x = action->target->x;
                action->actor->cur_y = action->target->y;
                Stats *stats = &action->target->character->stats;
                stats->hp -= 15;
                if (stats->hp <= 0)
                {
                    stats->hp = 0;
                    action->target->is_alive = false;
                }
                break;
            case AT_NONE:
                continue;
            default:
                break;
        }
        action->actor->priority = 1;
        action->target->priority = 2;
        VBlankIntrWait();
        // Update players
        for (int j = 0; j < party_size; j++)
        {
            draw_sprite(j, &battle_party[j]);
        }

        // Update enemies
        for (int j = 0; j < enemies_size; j++)
        {
            BattleCharacter *enemy = &enemies[j];
            enemy->is_targeted = false;
            draw_sprite(j + party_size, enemy);
        }
        oam_copy(oam_mem, oam_buf, 6);

        for (int j = 0; j < 60; j++) VBlankIntrWait();
        action->actor->cur_x = action->actor->x;
        action->actor->cur_y = action->actor->y;
    }
}

