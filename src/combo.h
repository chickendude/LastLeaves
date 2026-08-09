#ifndef LASTLEAVES_COMBO_H
#define LASTLEAVES_COMBO_H
#include "animation.h"
#include "global.h"

typedef struct Combo
{
    char name[20];
    Animation animation;
    const AttackDir combo[10];

} Combo;

const Combo* is_combo(
    const AttackDir *attack_array,
    int index,
    const Combo *combos,
    int num_combos
);

#endif //LASTLEAVES_COMBO_H
