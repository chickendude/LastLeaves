#include "battle_actions.h"

#include <tonc.h>

#include "battle_gfx.h"
#include "battle_vars.h"
#include "global.h"
#include "math.h"
#include "party.h"

void perform_attack(const BattleAction *action);

void perform_move(const BattleAction *action);

void perform_return(const BattleAction *action);

void move_character(BattleCharacter *character, int target_x, int target_y,
                    int steps);

// --------------- public functions -------------------

void perform_battle_queue()
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        const BattleAction *action = &battle_queue[i];
        if (!action->actor->is_alive) continue;
        switch (action->type)
        {
            case AT_ATTACK:
                perform_attack(action);
                break;
            case AT_MOVE:
                perform_move(action);
                break;
            case AT_RETURN:
                perform_return(action);
                break;
            case AT_NONE:
            default:
                break;
        }
    }
}

void queue_add_action(
    const ActionType type,
    BattleCharacter *actor,
    BattleCharacter *target
)
{
    if (battle_queue_index == MAX_ACTIONS) return;

    BattleAction *action = &battle_queue[battle_queue_index++];
    action->type = type;
    action->actor = actor;
    action->target = target;
}

// --------------- private functions -------------------

void perform_attack(const BattleAction *action)
{
    BattleCharacter *target = action->target;
    BattleCharacter *actor = action->actor;
    Stats *stats = &action->target->character->stats;
    stats->hp -= 15;
    if (stats->hp <= 0)
    {
        stats->hp = 0;
        action->target->is_alive = false;
    }
    actor->priority = 1;
    target->priority = 2;
    draw_damage(15, target->x + fxpt(8), target->y);
    for (int j = 0; j < 60; j++) VBlankIntrWait();
}

void perform_move(const BattleAction *action)
{
    int target_x = action->target->x;
    const int target_y = action->target->y;
    if (target_x < action->actor->x)
    {
        target_x += fxpt(18);
    } else
    {
        target_x -= fxpt(18);
    }
    move_character(action->actor, target_x, target_y, 20);
}

void perform_return(const BattleAction *action)
{
    move_character(action->actor, action->actor->x, action->actor->y, 8);
}

void move_character(
    BattleCharacter *character,
    const int target_x,
    const int target_y,
    const int steps
)
{
    character->animation = &character->character->graphics->walk;
    character->frame_cycle = 0;
    character->frame_index = 0;
    // Find sprite ID. TODO: assign sprite ID to the BattleCharacter
    int actor_id = -1;
    for (int j = 0; j < party_size + enemies_size; j++)
    {
        if ((j < party_size && character == &battle_party[j]) ||
            (j >= party_size && character == &enemies[j - party_size]))
        {
            actor_id = j;
        }
    }

    // 16 steps
    const int step_change = fxpt(1) / steps;
    int step = 0;
    const int start_x = character->cur_x;
    const int start_y = character->cur_y;
    for (int i = 0; i < steps; i++)
    {
        character->cur_x = lerp(start_x, target_x, step);
        character->cur_y = lerp(start_y, target_y, step);
        step += step_change;
        draw_sprite(actor_id, character);
        VBlankIntrWait();
    }
    character->cur_x = target_x;
    character->cur_y = target_y;
    character->animation = &character->character->graphics->idle;
    character->frame_cycle = 0;
    character->frame_index = 0;
    draw_sprite(actor_id, character);
    VBlankIntrWait();
}
