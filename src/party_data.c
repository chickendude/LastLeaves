#include "party_data.h"

#include "lynne-battle.h"
#include "player.h"
#include "roak-battle.h"
#include "tann-battle.h"


const PlayerGraphics tann_graphics = {
    .idle = {
        .sprite_data = tann_battleTiles,
        .frame_cycles = {20, 20, 20},
        .num_frames = 3
    },
    .walk = {
        .sprite_data = tann_battleTiles + 128*3,
        .frame_cycles = {3, 2, 2, 3, 2, 2},
        .num_frames = 6
    }
};

const PlayerGraphics roak_graphics = {
    .idle = {
        .sprite_data = roak_battleTiles,
        .frame_cycles = {30, 30},
        .num_frames = 2
    },
    .walk = {
        .sprite_data = roak_battleTiles + 128*2,
        .frame_cycles = {3, 3},
        .num_frames = 2
    }
};

const PlayerGraphics lynne_graphics = {
    .idle = {
        .sprite_data = lynne_battleTiles,
        .frame_cycles = {10, 5, 5, 10},
        .num_frames = 4
    },
    .walk = {
        .sprite_data = lynne_battleTiles + 128*4,
        .frame_cycles = {3, 2, 2, 3, 2, 2},
        .num_frames = 6
    }
};
const PlayerData tann_data = {
    &tann_graphics,
    "Tann",
    50,
    25,
    10,
    10,
    8,
    8,
    30,
    10
};

const PlayerData roak_data = {
    &roak_graphics,
    "Roak",
    45,
    30,
    10,
    12,
    7,
    7,
    30,
    10
};

const PlayerData lynne_data = {
    &lynne_graphics,
    "Lynne",
    55,
    17,
    10,
    15,
    7,
    5,
    35,
    13
};
