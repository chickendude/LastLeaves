#include "party.h"

#include "party_data.h"

Player party[3];
int party_size;
// ----------------------- Private Function Definitions ------------------------

// ----------------------------- Public Functions ------------------------------

void initialize_party()
{
    party_size = 3;
    initialize_player(&party[0], TANN, &tann_data);
    initialize_player(&party[1], ROAK, &roak_data);
    initialize_player(&party[2], LYNNE, &lynne_data);
}

// --------------------------- Private Function --------------------------------
