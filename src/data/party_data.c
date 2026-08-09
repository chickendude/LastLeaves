#include "party_data.h"

#include "lynne-battle.h"
#include "party_combos.h"
#include "roak-battle.h"
#include "tann-battle.h"

const PlayerGraphics tann_graphics = {
    .idle = {
        .sprite_data = tann_battleTiles,
        .frame_cycles = {20, 20, 20},
        .num_frames = 3
    },
    .walk = {
        .sprite_data = tann_battleTiles + 128 * 3,
        .frame_cycles = {3, 2, 2, 3, 2, 2},
        .num_frames = 6
    },
    .hit = {
        .sprite_data = tann_battleTiles + 128 * 9,
        .frame_cycles = {10},
        .num_frames = 1
    },
    .attack_left = {
        .sprite_data = lynne_battleTiles + 128 * 12,
        .frame_cycles = {30, 5},
        .num_frames = 2
    },
    .attack_right = {
        .sprite_data = lynne_battleTiles + 128 * 14,
        .frame_cycles = {30},
        .num_frames = 1
    },
    .attack_up = {
        .sprite_data = lynne_battleTiles + 128 * 15,
        .frame_cycles = {30, 5},
        .num_frames = 2
    },
    .attack_down = {
        .sprite_data = lynne_battleTiles + 128 * 17,
        .frame_cycles = {30},
        .num_frames = 1
    },
};

const PlayerGraphics roak_graphics = {
    .idle = {
        .sprite_data = roak_battleTiles,
        .frame_cycles = {30, 30},
        .num_frames = 2
    },
    .walk = {
        .sprite_data = roak_battleTiles + 128 * 2,
        .frame_cycles = {3, 3},
        .num_frames = 2
    },
    .hit = {
        .sprite_data = roak_battleTiles + 128 * 4,
        .frame_cycles = {10},
        .num_frames = 1
    },
    .attack_left = {
        .sprite_data = lynne_battleTiles + 128 * 12,
        .frame_cycles = {30, 5},
        .num_frames = 2
    },
    .attack_right = {
        .sprite_data = lynne_battleTiles + 128 * 14,
        .frame_cycles = {30},
        .num_frames = 1
    },
    .attack_up = {
        .sprite_data = lynne_battleTiles + 128 * 15,
        .frame_cycles = {30, 5},
        .num_frames = 2
    },
    .attack_down = {
        .sprite_data = lynne_battleTiles + 128 * 17,
        .frame_cycles = {30},
        .num_frames = 1
    },
};

const PlayerGraphics lynne_graphics = {
    .idle = {
        .sprite_data = lynne_battleTiles,
        .frame_cycles = {10, 5, 5, 10},
        .num_frames = 4
    },
    .walk = {
        .sprite_data = lynne_battleTiles + 128 * 4,
        .frame_cycles = {3, 2, 2, 3, 2, 2},
        .num_frames = 6
    },
    .hit = {
        .sprite_data = lynne_battleTiles + 128 * 10,
        .frame_cycles = {10, 5},
        .num_frames = 2
    },
    .attack_left = {
        .sprite_data = lynne_battleTiles + 128 * 12,
        .frame_cycles = {30, 5},
        .num_frames = 2
    },
    .attack_right = {
        .sprite_data = lynne_battleTiles + 128 * 14,
        .frame_cycles = {30},
        .num_frames = 1
    },
    .attack_up = {
        .sprite_data = lynne_battleTiles + 128 * 15,
        .frame_cycles = {30, 5},
        .num_frames = 2
    },
    .attack_down = {
        .sprite_data = lynne_battleTiles + 128 * 17,
        .frame_cycles = {30},
        .num_frames = 1
    },
};

const PlayerData tann_data = {
    &tann_graphics,
    combos_tann,
    NUM_COMBOS_TANN,
    .name = "Tann",
    .level = 1,
    .max_hp = 55,
    .max_mp = 10,
    .max_cp = 10,
    .atk = 18,
    .def = 8,
    .mag = 30,
    .sta = 30
};

const PlayerData roak_data = {
    &roak_graphics,
    combos_roak,
    NUM_COMBOS_ROAK,
    "Roak",
    1,
    50,
    10,
    12,
    25,
    7,
    30,
    14
};

const PlayerData lynne_data = {
    &lynne_graphics,
    combos_lynne,
    NUM_COMBOS_LYNNE,
    "Lynne",
    1,
    47,
    10,
    15,
    27,
    5,
    35,
    23
};
