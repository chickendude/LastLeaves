#ifndef LASTLEAVES_PLAYER_H
#define LASTLEAVES_PLAYER_H
#include "party_data.h"
#include "stats.h"

typedef enum PlayerType
{
    TANN, ROAK, LYNNE
} PlayerType;

typedef struct Player
{
    char name[10];
    PlayerType type;
    const PlayerGraphics *graphics;
    Stats stats;
} Player;

void initialize_player(Player *player, PlayerType type, const PlayerData *data);

#endif //LASTLEAVES_PLAYER_H
