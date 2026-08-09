// This was generated from the 'attacks_[...].txt' files in the data folder
// by running tools/build_party

#include "party_combos.h"
#include "combo_sprites.h"

// lynne

const Combo combo_lynne_sweeper = {
    .name = "Sweeper",
    .animation = {
        .sprite_data = lynne3_sweeperTiles,
        .num_frames = 4,
        .frame_cycles = {30,5,5,20},
     },
    .combo = {ATK_LOW, ATK_HIGH, ATK_HIGH}
};

const Combo combo_lynne_left_strike = {
    .name = "Left Strike",
    .animation = {
        .sprite_data = lynne3_left_strikeTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_LEFT, ATK_LEFT, ATK_RIGHT}
};

const Combo combo_lynne_round_kick = {
    .name = "Round Kick",
    .animation = {
        .sprite_data = lynne4_round_kickTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_HIGH, ATK_LEFT, ATK_LOW, ATK_RIGHT}
};

const Combo combo_lynne_cannon_gash = {
    .name = "Cannon Gash",
    .animation = {
        .sprite_data = lynne4_cannon_gashTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_LOW, ATK_HIGH, ATK_HIGH, ATK_LOW}
};

const Combo combo_lynne_wonder_spin = {
    .name = "Wonder Spin",
    .animation = {
        .sprite_data = lynne4_wonder_spinTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_RIGHT, ATK_LOW, ATK_HIGH, ATK_LOW}
};

const Combo combo_lynne_blind_fist = {
    .name = "Blind Fist",
    .animation = {
        .sprite_data = lynne4_blind_fistTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_LEFT, ATK_HIGH, ATK_LOW, ATK_HIGH}
};

const Combo combo_lynne_whirlpool = {
    .name = "Whirlpool",
    .animation = {
        .sprite_data = lynne5_whirlpoolTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_LOW, ATK_LEFT, ATK_LEFT, ATK_RIGHT, ATK_LEFT}
};

const Combo combo_lynne_jump_kick = {
    .name = "Jump Kick",
    .animation = {
        .sprite_data = lynne5_jump_kickTiles,
        .num_frames = 4,
        .frame_cycles = {10,10,10,10},
     },
    .combo = {ATK_HIGH, ATK_LEFT, ATK_HIGH, ATK_LOW, ATK_LOW}
};

const Combo combos_lynne[8] = { combo_lynne_sweeper, combo_lynne_left_strike, combo_lynne_round_kick, combo_lynne_cannon_gash, combo_lynne_wonder_spin, combo_lynne_blind_fist, combo_lynne_whirlpool, combo_lynne_jump_kick, };

// roak

const Combo combo_roak_board_smash = {
    .name = "Board Smash",
    .animation = {
        .sprite_data = roak3_board_smashTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LOW, ATK_HIGH, ATK_RIGHT}
};

const Combo combo_roak_wild_punch = {
    .name = "Wild Punch",
    .animation = {
        .sprite_data = roak3_wild_punchTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LEFT, ATK_LEFT, ATK_HIGH}
};

const Combo combo_roak_shakedown = {
    .name = "Shakedown",
    .animation = {
        .sprite_data = roak4_shakedownTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_HIGH, ATK_LEFT, ATK_HIGH, ATK_LEFT}
};

const Combo combo_roak_seize = {
    .name = "Seize",
    .animation = {
        .sprite_data = roak4_seizeTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LEFT, ATK_HIGH, ATK_RIGHT, ATK_RIGHT}
};

const Combo combo_roak_windfall = {
    .name = "Windfall",
    .animation = {
        .sprite_data = roak4_windfallTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_RIGHT, ATK_HIGH, ATK_LEFT, ATK_LOW}
};

const Combo combo_roak_light_flow = {
    .name = "Light Flow",
    .animation = {
        .sprite_data = roak4_light_flowTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LEFT, ATK_LEFT, ATK_HIGH, ATK_LOW}
};

const Combo combo_roak_chain_stop = {
    .name = "Chain Stop",
    .animation = {
        .sprite_data = roak5_chain_stopTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LOW, ATK_LEFT, ATK_LOW, ATK_RIGHT, ATK_LEFT}
};

const Combo combo_roak_time_blow = {
    .name = "Time Blow",
    .animation = {
        .sprite_data = roak5_time_blowTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_RIGHT, ATK_LEFT, ATK_LEFT, ATK_LOW, ATK_RIGHT}
};

const Combo combos_roak[8] = { combo_roak_board_smash, combo_roak_wild_punch, combo_roak_shakedown, combo_roak_seize, combo_roak_windfall, combo_roak_light_flow, combo_roak_chain_stop, combo_roak_time_blow, };

// tann

const Combo combo_tann_late_hit = {
    .name = "Late Hit",
    .animation = {
        .sprite_data = tann3_late_hitTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LEFT, ATK_HIGH, ATK_LEFT}
};

const Combo combo_tann_armored_fury = {
    .name = "Armored Fury",
    .animation = {
        .sprite_data = tann3_armored_furyTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_RIGHT, ATK_RIGHT, ATK_HIGH}
};

const Combo combo_tann_sky_breaker = {
    .name = "Sky Breaker",
    .animation = {
        .sprite_data = tann4_sky_breakerTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_HIGH, ATK_LEFT, ATK_LOW, ATK_RIGHT}
};

const Combo combo_tann_petal_blade = {
    .name = "Petal Blade",
    .animation = {
        .sprite_data = tann4_petal_bladeTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_RIGHT, ATK_LOW, ATK_HIGH, ATK_LOW}
};

const Combo combo_tann_heavy_core = {
    .name = "Heavy Core",
    .animation = {
        .sprite_data = tann4_heavy_coreTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LEFT, ATK_LOW, ATK_HIGH, ATK_LEFT}
};

const Combo combo_tann_slash_punch = {
    .name = "Slash Punch",
    .animation = {
        .sprite_data = tann4_slash_punchTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LOW, ATK_HIGH, ATK_LOW, ATK_RIGHT}
};

const Combo combo_tann_frustration = {
    .name = "Frustration",
    .animation = {
        .sprite_data = tann5_frustrationTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_LEFT, ATK_HIGH, ATK_HIGH, ATK_RIGHT, ATK_LOW}
};

const Combo combo_tann_pummel_kick = {
    .name = "Pummel Kick",
    .animation = {
        .sprite_data = tann5_pummel_kickTiles,
        .num_frames = 4,
        .frame_cycles = {60,5,5,20},
     },
    .combo = {ATK_HIGH, ATK_LEFT, ATK_HIGH, ATK_LOW, ATK_LEFT}
};

const Combo combos_tann[8] = { combo_tann_late_hit, combo_tann_armored_fury, combo_tann_sky_breaker, combo_tann_petal_blade, combo_tann_heavy_core, combo_tann_slash_punch, combo_tann_frustration, combo_tann_pummel_kick, };

