#include <tonc.h>

#include "battle.h"
#include "party.h"
#include "title.h"

void vblank(void)
{
    // do nothing
}

int main(void)
{
    while (true)
    {
        irq_init(isr_master);
        irq_add(II_VBLANK, vblank);
        irq_enable(II_VBLANK);
        oam_init(obj_mem, 128);
        memset32(tile_mem_obj, 0, 2048);
        initialize_party();
        titlescreen();
        battle();
        memset32(tile_mem, 0, VRAM_SIZE);
    }
}
