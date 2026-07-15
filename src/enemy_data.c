#include "enemy_data.h"
#include "lynne-battle.h"
#include "roak-battle.h""
#include "tann-battle.h"

// Graphics definitions

const PlayerGraphics enemy1_graphics = {
    .idle = {
        .sprite_data = tann_battleTiles,
        .frame_cycles = {20, 20, 20},
        .num_frames = 3
    },
    .walk = {
        .sprite_data = tann_battleTiles + 128 * 3,
        .frame_cycles = {3, 2, 2, 3, 2, 2},
        .num_frames = 6
    }
};

const PlayerGraphics enemy2_graphics = {
    .idle = {
        .sprite_data = roak_battleTiles,
        .frame_cycles = {30, 30},
        .num_frames = 2
    },
    .walk = {
        .sprite_data = roak_battleTiles + 128 * 2,
        .frame_cycles = {3, 3},
        .num_frames = 2
    }
};

const PlayerGraphics enemy3_graphics = {
    .idle = {
        .sprite_data = lynne_battleTiles,
        .frame_cycles = {30, 30},
        .num_frames = 2
    },
    .walk = {
        .sprite_data = lynne_battleTiles + 128 * 2,
        .frame_cycles = {3, 3},
        .num_frames = 2
    }
};

// Data definitions

const PlayerData enemy1_data = {
    &enemy1_graphics,
    "Monster 1",
    50,
    25,
    10,
    10,
    8,
    8,
    30,
    10
};

const PlayerData enemy2_data = {
    &enemy2_graphics,
    "Guard",
    45,
    30,
    10,
    12,
    7,
    7,
    30,
    10
};

const PlayerData enemy3_data = {
    &enemy3_graphics,
    "Shifu",
    45,
    30,
    10,
    12,
    7,
    7,
    30,
    10
};

const PlayerData enemy_data[NUM_ENEMIES] = {enemy1_data, enemy2_data, enemy3_data};
