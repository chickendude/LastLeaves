#include <tonc.h>

#include "battle.h"
#include "title.h"

void vblank(void)
{
    // do nothing
}

int main(void)
{
    irq_init(isr_master);
    irq_add(II_VBLANK, vblank);
    irq_enable(II_VBLANK);

    while (true)
    {
        oam_init(obj_mem, 128);
        memset32(tile_mem_obj, 0, 2048);
        titlescreen();
        battle();
    }
}
