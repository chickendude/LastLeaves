#ifndef LASTLEAVES_PLAYER_H
#define LASTLEAVES_PLAYER_H
#include "stats.h"

typedef enum PlayerType
{
    TANN, ROAK, LYNNE
} PlayerType;

typedef struct Player
{
    char name[10];
    PlayerType type;
    Stats stats;
} Player;
#endif //LASTLEAVES_PLAYER_H
