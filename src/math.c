#include "math.h"

// --------------- public functions -------------------

int random(const int max)
{
    static unsigned int randSeed = 0xB0B15BAD;

    randSeed *= 0x0019660D;
    randSeed += 0x3C6EF35F;

    return ((int)(randSeed >> 16) % max);
}
