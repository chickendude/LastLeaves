#ifndef LASTLEAVES_BATTLE_ACTIONS_H
#define LASTLEAVES_BATTLE_ACTIONS_H
#include "battle_vars.h"

void perform_battle_queue();

void queue_add_action(
    ActionType type,
    BattleCharacter *actor,
    BattleCharacter *target,
    BattleCharacter *target_party,
    int target_party_size
);

void queue_remove_player_actions(const BattleCharacter *actor);

void clear_battle_queue();

#endif //LASTLEAVES_BATTLE_ACTIONS_H
