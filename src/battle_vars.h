#ifndef LASTLEAVES_BATTLE_VARS_H
#define LASTLEAVES_BATTLE_VARS_H
#include "global.h"
#include "player.h"

typedef enum ActionType
{
    AT_NONE,
    AT_ITEM,
    AT_ATTACK,
    AT_MAGIC,
    AT_MOVE,
    AT_RETURN
} ActionType;

typedef struct BattleCharacter
{
    Player *character;
    /** 0 = left, 1 = right */
    int dir;
    /** Player's home X position on. */
    int x;
    /** Where player X currently is on screen. */
    int cur_x;
    /** X velocity. */
    int vel_x;
    /** Player's home Y position on. */
    int y;
    /** Where player Y currently is on screen. */
    int cur_y;
    /** Y velocity. */
    int vel_y;
    int is_alive;
    /** Is being targeted for an attack. */
    int is_targeted;
    int priority;
    /** Used to display and animate the hp when getting hit or healing. */
    int disp_hp;
} BattleCharacter;

typedef struct DamageText
{
    int digits[4];
    int num_digits;
    int x;
    int y;
    int frames_left;
} DamageText;

/** An action to take in a battle. */
typedef struct BattleAction
{
    ActionType type;
    BattleCharacter *actor;
    BattleCharacter *target;
} BattleAction;

extern int battle_queue_index;
extern BattleAction battle_queue[MAX_ACTIONS];

extern BattleCharacter battle_party[3];
extern BattleCharacter enemies[3];
extern int enemies_size;

extern DamageText damage_texts[MAX_DAMAGE_TEXTS];

#endif //LASTLEAVES_BATTLE_VARS_H
