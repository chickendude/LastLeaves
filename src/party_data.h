#ifndef LASTLEAVES_PARTY_DATA_H
#define LASTLEAVES_PARTY_DATA_H

typedef struct Animation
{
    const unsigned int *sprite_data;
    const int num_frames;
    /** How many cycles to show each frame in the animation. */
    const char frame_cycles[10];
} Animation;

typedef struct PlayerGraphics
{
    Animation idle;
    Animation walk;
    Animation *attacks;
    int num_attacks;
} PlayerGraphics;

typedef struct PlayerData {
    const PlayerGraphics *graphics;
    char name[10];
    /** Level **/
    int level;
    /** Max HP **/
    int max_hp;
    /** Max MP **/
    int max_mp;
    /** Max CP - Combo Points **/
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
} PlayerData;

extern const PlayerData tann_data;
extern const PlayerData roak_data;
extern const PlayerData lynne_data;
#endif //LASTLEAVES_PARTY_DATA_H
