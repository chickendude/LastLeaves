#include "combo.h"

const Combo *is_combo(const AttackDir *attack_array, const int index,
                      const Combo *combos, const int num_combos)
{
    // Keeps track of which combos are still possible, set to 1 when ruled out
    int is_combo_impossible[MAX_COMBOS] = {0};

    // Mark unused combos as impossible
    for (int i = num_combos; i < MAX_COMBOS; i++) is_combo_impossible[i] = 1;

    // Start from the current attack and work our way back to the previous
    // attacks. As this means combos are checked in reverse order, the combos
    // themselves are stored in reverse order. So UUD is stored as DUU.
    int combo_index = 0;
    for (int i = index; i >= -1; i--)
    {
        const AttackDir atk_dir = attack_array[i];
        for (int j = 0; j < num_combos; j++)
        {
            if (is_combo_impossible[j]) continue;
            const Combo *combo = &combos[j];
            const AttackDir cur_attack = combo->combo[combo_index];

            // If the current combo_index == combo's length, we've gone through
            // entire combo
            if (combo_index == combo->combo_length) return combo;

            // If the combo attack doesn't match the combo, we can skip it
            if (cur_attack != atk_dir) is_combo_impossible[j] = 1;
        }
        combo_index++;
    }
    return NULL;
}
