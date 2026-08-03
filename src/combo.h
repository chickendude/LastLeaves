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

#endif //LASTLEAVES_COMBO_H
