#ifndef LASTLEAVES_GLOBAL_H
#define LASTLEAVES_GLOBAL_H
#include <tonc_types.h>

/** Buffer for OAM entries so we can update the OAM outside of VBlank. */
extern OBJ_ATTR oam_buf[128];

/** Max number of actions that can take place in a battle each turn. */
#define MAX_ACTIONS 10

#endif //LASTLEAVES_GLOBAL_H
