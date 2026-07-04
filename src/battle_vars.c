#include "battle_vars.h"

#include "battle_actions.h"
#include "global.h"

int battle_queue_index;
BattleAction battle_queue[MAX_ACTIONS];
// Player's party
BattleCharacter battle_party[3];

// Enemy's party
Player enemy_party[3];
BattleCharacter enemies[3] = {
    {.character = &enemy_party[0]},
    {.character = &enemy_party[1]},
    {.character = &enemy_party[2]},
};
// Number of enemies in the battle
int enemies_size = 3;

