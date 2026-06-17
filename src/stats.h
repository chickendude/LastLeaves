#ifndef LASTLEAVES_STATS_H
#define LASTLEAVES_STATS_H

typedef struct Stats
{
    /** Current HP - Health Points **/
    int hp;
    /** Max HP **/
    int max_hp;
    /** Current MP - Magic Points **/
    int mp;
    /** Max MP **/
    int max_mp;
    /** Current CP - Combo Points **/
    int cp;
    /** Max CP **/
    int max_cp;
    /** Attack - Physical attacks **/
    int atk;
    /** Defense **/
    int def;
    /** Magic - Attack/Defense for magic skills **/
    int mag;
    /** Stamina - How long attack combos can be **/
    int sta;
    /** Speed - Determines order of attack **/
    int spd;
} Stats;
#endif //LASTLEAVES_STATS_H
