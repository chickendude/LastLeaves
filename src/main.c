#include <tonc.h>

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

    oam_init(obj_mem, 128);
    memset32(tile_mem_obj, 0, 2048);
    titlescreen();
    // Random stuff for future setup
    REG_DISPCNT = DCNT_MODE0 |
                  DCNT_BG0 | DCNT_BG1 | DCNT_BG2 |
                  DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(29) | BG_PRIO(2) | BG_REG_32x32 | BG_4BPP;
    REG_BG1CNT = BG_CBB(3) | BG_SBB(30) | BG_PRIO(3) | BG_REG_32x32 | BG_4BPP;
    REG_BG2CNT = BG_CBB(2) | BG_SBB(31) | BG_PRIO(0) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    while (true)
    {
        VBlankIntrWait();
    }
}
