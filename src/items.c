#include "items.h"
#include "party.h"

// ------------------ Private Declarations --------------

ItemSlot* find_item_in_inventory(const Item *item);

// ------------------ Public Objects --------------

// Health
const Item it_life_herb = { "life herb", ITEM_HEALTH, 25, false };
const Item it_life_berry = { "life berry", ITEM_HEALTH, 100, false };
const Item it_life_nuts = { "life nuts", ITEM_HEALTH, 500, false };

const Item it_life_tea = { "life tea", ITEM_HEALTH, 50, true };
const Item it_life_broth = { "life broth", ITEM_HEALTH, 250, true };

// Magic
const Item it_mag_herb = { "magic herb", ITEM_MAGIC, 25, false };
const Item it_mag_berry = { "magic berry", ITEM_MAGIC, 100, false };
const Item it_mag_nuts = { "magic nuts", ITEM_MAGIC, 500, false };

const Item it_mag_tea = { "magic tea", ITEM_MAGIC, 50, true };
const Item it_mag_broth = { "magic broth", ITEM_MAGIC, 250, true };

// ------------------ Public Functions ------------------

void add_item(const Item* item, int value)
{
    ItemSlot *slot = find_item_in_inventory(item);
    // If item doesn't exist in inventory yet, add to end
    if (slot == NULL)
    {
        slot = &party.inventory[party.inventory_size++];
        slot->amount = 0;
        slot->item = item;
    }
    slot->amount += value;
    if (slot->amount > 99) slot->amount = 99;
}
// ------------------ Private Functions -----------------

ItemSlot* find_item_in_inventory(const Item *item)
{
    // Find item with same type in inventory
    for (int i = 0; i < party.inventory_size; i++)
    {
        ItemSlot *slot = &party.inventory[i];
        if (!slot) continue;
        if (slot->item == item) return slot;
    }
    return NULL;
}
