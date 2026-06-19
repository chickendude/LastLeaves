#include <tonc.h>
#include "math.h"

// --------------- public functions -------------------

int random(const int max)
{
    static unsigned int randSeed = 0xB0B15BAD;

    randSeed *= 0x0019660D;
    randSeed += 0x3C6EF35F;

    return ((int) (randSeed >> 16) % max);
}

const int UNITS[6] = {100000, 10000, 1000, 100, 10, 1};

int count_digits(u32 number, int *digits)
{
    // Quick exit if number is less than 10
    if (number < 10)
    {
        digits[0] = number;
        return 1;
    }

    // Loop through each digit, starting from the highest, calculating how many
    // of that digit there are
    int index = 0;
    bool is_initialized = false;
    for (int i = 0; i < sizeof(UNITS) / sizeof(int); i++)
    {
        int cur_num = 0;
        while (UNITS[i] <= number)
        {
            cur_num++;
            number -= UNITS[i];
        }
        if (cur_num > 0 || is_initialized)
        {
            is_initialized = true;
            digits[index] = cur_num;
            index++;
        }
    }
    return index;
}
