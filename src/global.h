#ifndef LASTLEAVES_GLOBAL_H
#define LASTLEAVES_GLOBAL_H
#include <tonc_types.h>

//  Defines missing from tonc.
/** ALPHA option for REG_BLDCNT / BLD_BUILD. */
#define FADE_ALPHA 1

/** Buffer for OAM entries so we can update the OAM outside of VBlank. */
extern OBJ_ATTR oam_buf[128];

/** Max number of actions that can take place in a battle each turn. */
#define MAX_ACTIONS 40

/** Max number of damage texts that can be shown at once. */
#define MAX_DAMAGE_TEXTS 5

/** Number of frames to display damage text in battles. */
#define DAMAGE_TEXT_FRAMES 40

#endif //LASTLEAVES_GLOBAL_H
