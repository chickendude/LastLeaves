#include "battle_actions.h"

#include <tonc.h>

#include "battle_gfx.h"
#include "battle_vars.h"
#include "global.h"
#include "math.h"
#include "mgba_log.h"
#include "party.h"
#include "text.h"

void perform_attack(const BattleAction *action);

/** Handles a single, physical attack. */
void perform_physical_attack(AttackDir attack_dir, BattleCharacter *actor,
                             BattleCharacter *target);

/** Handles combo. Runs on the last hit of the combo. */
void perform_combo(const Combo *combo, BattleCharacter *actor,
                   BattleCharacter *target);

void perform_move(const BattleAction *action);

void perform_return(const BattleAction *action);

void move_character(BattleCharacter *character, int target_x, int target_y,
                    int steps);

void wait_for_animation_frame(const BattleCharacter *actor,
                              BattleCharacter *target);

void wait_for_animation_finish(const BattleCharacter *actor,
                               BattleCharacter *target);

/**
 * Calculates and applies the damage from one character's physical attack to
 * another character.
 * @param actor Attacking character
 * @param target Defending character
 * @param bonus_multiplier Bonus damage. It's added on top of the normal damage
 * and is calculated based on the attack value. A value of 1 adds an extra
 * 0-100% of the attacker's ATK. A value of 2 adds an extra 50-150% of the
 * attacker's ATK. Each additional value adds 50% to the base value.
 */
void apply_damage(const BattleCharacter *actor, const BattleCharacter *target,
                  int bonus_multiplier);

// --------------- public functions -------------------

void perform_battle_queue()
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        BattleAction *action = &battle_queue[i];
        // Make sure actor is still alive
        if (!action->actor->is_alive) continue;
        // Make sure target is still alive and pick new target if not
        if (action->target && !action->target->is_alive)
        {
            for (int j = 0; j < action->target_party_size; j++)
            {
                BattleCharacter *new_target = &action->target_party[j];
                if (new_target->is_alive)
                {
                    action->target = new_target;
                    break;
                }
            }
        }
        // If target is still not alive, there are no living targets left
        if (!action->target->is_alive) return;

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
    BattleCharacter *target,
    BattleCharacter *target_party,
    const int target_party_size
)
{
    if (battle_queue_index == MAX_ACTIONS) return;

    BattleAction *action = &battle_queue[battle_queue_index++];
    action->type = type;
    action->actor = actor;
    action->target = target;
    action->target_party = target_party;
    action->target_party_size = target_party_size;
}

void queue_remove_player_actions(const BattleCharacter *actor)
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        if (battle_queue[i].actor == actor) battle_queue[i].type = AT_NONE;
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
        BattleCharacter *character = &battle_party[i];
        character->priority = 2;
        character->vel_y = 0;
        character->vel_x = 0;
    }
    // Update enemies
    for (int i = 0; i < enemies_size; i++)
    {
        BattleCharacter *enemy = &enemies[i];
        enemy->priority = 2;
        enemy->vel_y = 0;
        enemy->vel_x = 0;
    }
}

// --------------- private functions -------------------

void perform_attack(const BattleAction *action)
{
    BattleCharacter *target = action->target;
    BattleCharacter *actor = action->actor;
    Stats *target_stats = &action->target->character->stats;
    int attack_index = 0;
    AttackDir attack_dir = actor->attacks[attack_index];
    while (attack_dir != ATK_NONE)
    {
        // Reset animation frames
        actor->frame_index = 0;
        actor->frame_cycle = 0;
        target->frame_index = 0;
        target->frame_cycle = 0;

        // Check for a combo
        const Combo *combo = is_combo(actor->attacks, attack_index,
                                      actor->character->combos,
                                      actor->character->num_combos);
        if (combo != NULL)
        {
            perform_combo(combo, actor, target);
        } else
        {
            perform_physical_attack(attack_dir, actor, target);
        }

        attack_dir = actor->attacks[++attack_index];
    }

    // Check if target was killed
    if (target_stats->hp <= 0)
    {
        target_stats->hp = 0;
        action->target->is_alive = false;
    }
}

void perform_combo(const Combo *combo, BattleCharacter *actor,
                   BattleCharacter *target)
{
    print_box(8, 10, 12, 3);
    print(0, 9, 11, combo->name);
    VBlankIntrDelay(60);
    memset32(&se_mem[30], 0, 224);
    actor->animation = &combo->animation;
    do
    {
        // Check if the current animation frame should have damage applied
        for (int i = 0; i < combo->num_damage_frames; i++)
        {
            if (combo->damage_frames[i] == actor->frame_index)
            {
                mgbalog("frame %d", actor->frame_index);
                const int bonus = combo->combo_length - 2;
                apply_damage(actor, target, bonus);
            }
        }
        wait_for_animation_frame(actor, target);
    } while (actor->frame_index);
    actor->animation = &actor->character->graphics->idle;
    VBlankIntrDelay(20);
}

void perform_physical_attack(const AttackDir attack_dir, BattleCharacter *actor,
                             BattleCharacter *target)
{
    switch (attack_dir)
    {
        case ATK_LEFT:
            actor->animation = &actor->character->graphics->attack_left;
            break;
        case ATK_RIGHT:
            actor->animation = &actor->character->graphics->attack_right;
            break;
        case ATK_HIGH:
            actor->animation = &actor->character->graphics->attack_up;
            break;
        case ATK_LOW:
            actor->animation = &actor->character->graphics->attack_down;
            break;
        default:
            actor->animation = &actor->character->graphics->idle;
            break;
    }
    target->animation = &target->character->graphics->hit;
    apply_damage(actor, target, 0);
    wait_for_animation_finish(actor, target);
    actor->animation = &actor->character->graphics->idle;
    VBlankIntrDelay(10);
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

void wait_for_animation_frame(const BattleCharacter *actor,
                              BattleCharacter *target)
{
    while (true)
    {
        VBlankIntrWait();
        // Reset hurt animation
        if (target->frame_index == 0 && target->frame_cycle == 0)
        {
            target->animation = &target->character->graphics->idle;
        }
        if (actor->frame_cycle == 0) return;
    }
}

void wait_for_animation_finish(const BattleCharacter *actor,
                               BattleCharacter *target)
{
    // Wait for the animation to loop, frame_cycle and frame_index are updated
    // in the battle_vblank() function, so after first vblank frame_cycle == 1
    while (true)
    {
        wait_for_animation_frame(actor, target);
        if (actor->frame_index == 0 && actor->frame_cycle == 0) return;
    }
}

void apply_damage(const BattleCharacter *actor, const BattleCharacter *target,
                  const int bonus_multiplier)
{
    const Stats *actor_stats = &actor->character->stats;
    const int atk = actor_stats->atk;
    Stats *target_stats = &target->character->stats;

    // Add random bonus to base atk: multiplier is a range:
    // 1 adds extra 0-100% of atk
    // 2 adds extra 50-150% of atk
    // 3 adds extra 100-200% of atk
    const int bonus_dmg = bonus_multiplier
                              ? ((bonus_multiplier - 1) * atk + random(2*atk)) >> 1
                              : 0;
    const int atk_boost = random(atk >> 3);
    const int dmg = (atk + bonus_dmg + atk_boost) * 12 >> 4;

    target_stats->hp -= dmg;
    draw_damage(dmg, target->x + fxpt(8), target->y);
}
