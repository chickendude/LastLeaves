#ifndef LASTLEAVES_BATTLE_GFX_H
#define LASTLEAVES_BATTLE_GFX_H
#include "battle_vars.h"

void draw_sprite(int index, BattleCharacter *character);
void draw_damage(int damage, int x, int y);
void update_damage_texts();

/**
 * Loads the graphics data needed to draw the battle bar:
 * - the left side + middle of the bar is loaded into tile data
 * - the right edge is loaded into sprite data
 */
void load_battlebar_tiles();

void draw_battlebar(const BattleCharacter *character);
void clear_battlebar();
#endif //LASTLEAVES_BATTLE_GFX_H
