#include "input.h"
#include <tonc.h>

// ------------------ Private Declarations --------------

// ------------------ Public Functions ------------------

void release_keys()
{
    unsigned int keys = key_curr_state();
    while (keys)
    {
        VBlankIntrWait();
        key_poll();
        keys = key_curr_state();
    }
}

// ------------------ Private Functions -----------------
