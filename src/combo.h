#ifndef LASTLEAVES_COMBO_H
#define LASTLEAVES_COMBO_H
#include "animation.h"
#include "global.h"

typedef struct Combo
{
    /** Name of the combo. */
    char name[20];
    /** Animation to play when combo is run. */
    Animation animation;
    /**
     * List of attacks required to execute the combo. Note that these are
     * stored in reverse order to simplify checking combos against the attack
     * array.
     */
    const AttackDir combo[10];
    /** Number of attack directions in the combo. */
    int combo_length;
    /**
     * Which frames to calculate damage on. For each number, the target will
     * be hit with damage on that frame.
     *
     * Note that this is the frame in the animation, not the number of VBlanks.
     */
    int damage_frames[10];
    /** Number of damage frames in the combo. */
    int num_damage_frames;
} Combo;

const Combo *is_combo(
    const AttackDir *attack_array,
    int index,
    const Combo *combos,
    int num_combos
);

#endif //LASTLEAVES_COMBO_H
