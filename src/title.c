#include <tonc.h>
#include "title.h"

#include "titlescreen.h"

#define WIDTH (M4_WIDTH / 2)

void show_title_image();

// --------------- public functions -------------------

void titlescreen()
{
    show_title_image();
    unsigned int keys = key_curr_state();
    while (!keys)
    {
        key_poll();
        keys = key_curr_state();
    }
    while (keys)
    {
        key_poll();
        keys = key_curr_state();
    }
}

// --------------- private functions -------------------

void show_title_image()
{
    memcpy16(pal_bg_mem, titlescreenPal, sizeof(titlescreenPal) / 2);
    u16 *start = vid_mem;
    const u16 *data = (u16 *) titlescreenBitmap;
    for (int i = 0; i < 160; i++)
    {
        const int offset = i * WIDTH;
        memcpy16(start + offset, data + offset, WIDTH);
    }
    vid_vsync();
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;
}
