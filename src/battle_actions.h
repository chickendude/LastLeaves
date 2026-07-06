#ifndef LASTLEAVES_BATTLE_ACTIONS_H
#define LASTLEAVES_BATTLE_ACTIONS_H
#include "battle_vars.h"

void perform_battle_queue();

void queue_add_action(
    ActionType type,
    BattleCharacter *actor,
    BattleCharacter *target
);

#endif //LASTLEAVES_BATTLE_ACTIONS_H
