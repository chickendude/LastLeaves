#ifndef LASTLEAVES_PARTY_H
#define LASTLEAVES_PARTY_H
#include "player.h"
#include "items.h"

typedef struct Party
{
    Player members[3];
    int party_size;
    ItemSlot inventory[20];
    int inventory_size;
} Party;

extern Party party;
extern int party_size;

void initialize_party(void);
#endif //LASTLEAVES_PARTY_H
