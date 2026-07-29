#ifndef LASTLEAVES_BATTLE_MENU_H
#define LASTLEAVES_BATTLE_MENU_H

#include "battle_vars.h"

/** Number of tiles dedicated to the menu text. */
#define MENU_TILES_OCCUPIED 20

typedef enum BattleMenu
{
    MENU_NONE, MENU_FIGHT, MENU_FLEE, MENU_ATTACK, MENU_ITEM, MENU_SPIRIT, MENU_MAGIC
} BattleMenu;

BattleMenu battle_start_menu();
BattleMenu battle_fight_menu();
MenuResult select_attack_menu(BattleCharacter* character);
MenuResult select_target(int* target_enemy_index);

#endif //LASTLEAVES_BATTLE_MENU_H
