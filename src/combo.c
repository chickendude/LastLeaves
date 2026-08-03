#include "combo.h"

const Combo* is_combo(const AttackDir *attack_array, const int index,
              const Combo *combos, const int num_combos)
{
    int is_combo_impossible[MAX_COMBOS] = {0};
    for (int i = num_combos; i < MAX_COMBOS; i++) is_combo_impossible[i] = 1;
    int combo_index = 0;
    for (int i = index; i >= -1; i--)
    {
        const AttackDir atk_dir = attack_array[i];
        for (int j = 0; j < num_combos; j++)
        {
            if (is_combo_impossible[j]) continue;
            const Combo *combo = &combos[j];
            // If the current combo == ATK_NONE, we've gone through entire combo
            const AttackDir cur_attack = combo->combo[combo_index];
            if (cur_attack == ATK_NONE) return combo;
            if (cur_attack != atk_dir) is_combo_impossible[j] = 1;
        }
        combo_index++;
    }
    return NULL;
}