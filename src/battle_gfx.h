#ifndef LASTLEAVES_BATTLE_GFX_H
#define LASTLEAVES_BATTLE_GFX_H
#include "battle_vars.h"

void draw_sprite(int index, BattleCharacter *character);
void draw_damage(int damage, int x, int y);
void update_damage_texts();
#endif //LASTLEAVES_BATTLE_GFX_H
