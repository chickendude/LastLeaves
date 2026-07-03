#include "player.h"

#include <tonc.h>

#include "party_data.h"

// ----------------------- Private Function Definitions ------------------------

// ----------------------------- Public Functions ------------------------------

void initialize_player(Player *player, const PlayerType type, const PlayerData *data)
{
    memcpy16(player->name,  data->name, 10/2);
    player->type = type;
    player->stats.lvl = 1;
    player->stats.hp = data->max_hp;
    player->stats.max_hp = data->max_hp;
    player->stats.mp = data->max_mp;
    player->stats.max_mp = data->max_mp;
    player->stats.cp = data->max_cp;
    player->stats.max_cp = data->max_cp;
    player->stats.atk = data->atk;
    player->stats.def = data->def;
    player->stats.mag = data->mag;
    player->stats.sta = data->sta;
    player->stats.spd = data->spd;
}

// --------------------------- Private Function --------------------------------
