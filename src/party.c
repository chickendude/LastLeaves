#include "party.h"

#include "data/party_data.h"

Party party;
int party_size;
// ----------------------- Private Function Definitions ------------------------

// ----------------------------- Public Functions ------------------------------

void initialize_party()
{
    party_size = 3;
    initialize_player(&party.members[0], TANN, &tann_data);
    initialize_player(&party.members[1], ROAK, &roak_data);
    initialize_player(&party.members[2], LYNNE, &lynne_data);
    add_item(&it_life_herb, 5);
    add_item(&it_life_berry, 2);
    add_item(&it_life_nuts, 2);
    add_item(&it_life_tea, 1);
    add_item(&it_mag_herb, 2);
}

// --------------------------- Private Function --------------------------------
