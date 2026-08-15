#ifndef LASTLEAVES_ITEMS_H
#define LASTLEAVES_ITEMS_H

#include <tonc.h>

typedef enum ItemType
{
    ITEM_NONE,
    /** Restore health. */
    ITEM_HEALTH,
    /** Restore magic. */
    ITEM_MAGIC,
    /** Revive fainted player. */
    ITEM_REVIVE,
    /** Leave Dungeon. */
    ITEM_ESCAPE,
    /** Permanently increase stat. */
    ITEM_STAT,
    /** Temporarily increase stat. */
    ITEM_STAT_BOOST,
    /** Prevent encounters. */
    ITEM_ENCOUNTER,
} ItemType;

typedef struct Item
{
    char name[16];
    ItemType type;
    int value;
    bool all;
} Item;

/**
 * A slot in the inventory.
 */
typedef struct ItemSlot
{
    const Item *item;
    int amount;
} ItemSlot;


void add_item(const Item* item, int value);

extern const Item it_life_herb, it_life_berry, it_life_nuts, it_life_tea, it_life_broth;
extern const Item it_mag_herb, it_mag_berry, it_mag_nuts, it_mag_tea, it_mag_broth;
#endif //LASTLEAVES_ITEMS_H
