#include <tonc.h>
#include "battle.h"

#include "battlemap.h"
#include "global.h"
#include "math.h"
#include "player.h"
#include "maps/battle/map_battle.h"
#include "lynne-battle.h"
#include "roak-battle.h"
#include "tann-battle.h"

/** Screen Block for battle map. */
#define SBB 29
/** Max number of actions that can take place per turn. */
#define MAX_ACTIONS 10

typedef enum ActionType
{
    AT_NONE, AT_ITEM, AT_ATTACK, AT_MAGIC
} ActionType;

typedef struct BattleCharacter
{
    Player character;
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
} BattleCharacter;

/** An action to take in a battle. */
typedef struct BattleAction
{
    ActionType type;
    BattleCharacter *actor;
    BattleCharacter *target;
} BattleAction;

void draw_map(void);

void initialize_parties(void);

void clear_battle_queue(void);

void draw_sprite(int index, BattleCharacter *character);

/** Select an attack.
 * @returns 0 = no attack selected, 1 = attack selected
 */
int select_attack(BattleCharacter *character, int *target_enemy_index);

void select_enemy_attacks();

void perform_attacks();

void start_battle(void);

BattleCharacter party[3] = {
    {.character = {.type = TANN},},
    {.character = {.type = LYNNE},},
    {.character = {.type = ROAK},},
};

BattleCharacter enemies[3] = {
    {},
    {},
    {},
};

int battle_queue_index;
BattleAction battle_queue[MAX_ACTIONS];

// Number of players in party
int party_size = 3;

// Number of enemies in the battle
int enemies_size = 3;

// --------------- public functions -------------------

void battle()
{
    // Enable mode 0 (4 layers) and onlyshow BG0
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(SBB) | BG_PRIO(2) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    // TODO: Load character and enemy bitmaps based off of party and enemy list

    draw_map();
    initialize_parties();
    start_battle();
    unsigned int keys = key_curr_state();
    while (!keys)
    {
        key_poll();
        keys = key_curr_state();
    }
    while (keys)
    {
        key_poll();
        keys = key_curr_state();
    }
}

// --------------- private functions -------------------

void draw_map()
{
    memcpy32(tile_mem, battlemapTiles, sizeof(battlemapTiles) / 4);
    memcpy32(tile_mem[4], tann_battleTiles, tann_battleTilesLen / 4);
    memcpy32(tile_mem[4] + 16, roak_battleTiles, roak_battleTilesLen / 4);
    memcpy32(tile_mem[4] + 32, lynne_battleTiles, lynne_battleTilesLen / 4);
    memcpy32(pal_bg_mem, battlemapPal, battlemapPalLen / 4);
    memcpy32(pal_obj_mem, tann_battlePal, tann_battlePalLen / 4);
    for (int row = 0; row < 10; row++)
    {
        for (int x = 0; x < 15; x++)
        {
            int y = row * 2 * 32;
            int tile = map_battle[row * 15 + x] * 4;
            se_mem[SBB][y + x * 2] = tile;
            se_mem[SBB][y + x * 2 + 1] = tile + 1;
            se_mem[SBB][y + 32 + x * 2] = tile + 2;
            se_mem[SBB][y + 32 + x * 2 + 1] = tile + 3;
        }
    }
}

void draw_sprite(const int index, BattleCharacter *character)
{
    int sprite_id = 0;
    switch (character->character.type)
    {
        case TANN:
            sprite_id = 0;
            break;
        case ROAK:
            sprite_id = 16;
            break;
        case LYNNE:
            sprite_id = 32;
            break;
    }
    int palette = character->is_targeted;
    obj_set_attr(&oam_buf[index],
                 ATTR0_4BPP | ATTR0_SQUARE | fxpt_to_int(character->cur_y),
                 ATTR1_SIZE_32x32 | fxpt_to_int(character->cur_x) | ATTR1_FLIP(
                     character->dir),
                 ATTR2_PALBANK(palette) |
                 ATTR2_PRIO(character->priority) | sprite_id
    );
}

void clear_battle_queue()
{
    battle_queue_index = 0;
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        battle_queue[i].type = AT_NONE;
    }
    for (int i = 0; i < party_size; i++)
    {
        BattleCharacter *character = &party[i];
        character->priority = 2;
        character->vel_y = 0;
        character->vel_x = 0;
    }
}

void perform_attacks()
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
        BattleAction *action = &battle_queue[i];
        switch (action->type)
        {
            case AT_ATTACK:
                action->actor->cur_x = action->target->x;
                action->actor->cur_y = action->target->y;
                break;
            case AT_NONE:
                continue;
            default:
                break;
        }
        action->actor->priority = 0;
        action->target->priority = 1;
        VBlankIntrWait();
        // Update players
        for (int i = 0; i < party_size; i++)
        {
            draw_sprite(i, &party[i]);
        }

        // Update enemies
        for (int i = 0; i < enemies_size; i++)
        {
            BattleCharacter *enemy = &enemies[i];
            enemy->is_targeted = false;
            draw_sprite(i + party_size, enemy);
        }
        oam_copy(oam_mem, oam_buf, 6);

        for (int j = 0; j < 60; j++) VBlankIntrWait();
        action->actor->cur_x = action->actor->x;
        action->actor->cur_y = action->actor->y;
    }
}

void start_battle()
{
    clear_battle_queue();
    // Which player is currently selecting moves, 0 < active_player < party_size
    int active_player_index = 0;
    int target_enemy_index = 0;
    int battle_over = 0;
    while (!battle_over)
    {
        // Run through random numbers while waiting
        random(256);
        key_poll();
        VBlankIntrWait();
        oam_copy(oam_mem, oam_buf, 6);
        if (active_player_index >= party_size)
        {
            select_enemy_attacks();
            perform_attacks();
            active_player_index = 0;
            clear_battle_queue();
        } else
        {
            if (select_attack(&party[active_player_index], &target_enemy_index))
            {
                active_player_index++;
                target_enemy_index = 0;
            }
            if (target_enemy_index < 0) target_enemy_index = enemies_size - 1;
            if (target_enemy_index >= enemies_size) target_enemy_index = 0;
        }

        // Update players
        for (int i = 0; i < party_size; i++)
        {
            draw_sprite(i, &party[i]);
        }

        // Update enemies
        for (int i = 0; i < enemies_size; i++)
        {
            BattleCharacter *enemy = &enemies[i];
            enemy->is_targeted = i == target_enemy_index;
            draw_sprite(i + party_size, enemy);
        }
    }
}

void add_action_to_battle_queue(
    const ActionType type,
    BattleCharacter *actor,
    BattleCharacter *target
)
{
    if (battle_queue_index == MAX_ACTIONS) return;

    BattleAction *action = &battle_queue[battle_queue_index++];
    action->type = type;
    action->actor = actor;
    action->target = target;
}

int select_attack(BattleCharacter *character, int *target_enemy_index)
{
    // Simple animation to show which character is selected
    character->cur_y -= character->vel_y;
    const int off_y = character->y - character->cur_y;
    character->vel_y += character->vel_y;
    if (fxpt_to_int(off_y) > 5)
    {
        character->vel_y = -25;
    } else if (off_y <= 0)
    {
        character->vel_y = 25;
    }

    // Check keys

    if (key_hit(KEY_LEFT))
    {
        (*target_enemy_index)--;
    }
    if (key_hit(KEY_RIGHT))
    {
        (*target_enemy_index)++;
    }
    if (key_hit(KEY_A))
    {
        add_action_to_battle_queue(
            AT_ATTACK,
            character,
            &enemies[*target_enemy_index]
        );
        return 1;
    }
    return 0;
}

void initialize_parties()
{
    for (int i = 0; i < party_size; i++)
    {
        party[i].is_alive = 1;
        party[i].x = fxpt(180 + i * 10);
        party[i].cur_x = party[i].x;
        party[i].y = fxpt(120 - i * 15);
        party[i].cur_y = party[i].y;
    }
    for (int i = 0; i < enemies_size; i++)
    {
        enemies[i].is_alive = 1;
        enemies[i].dir = 1;
        enemies[i].x = fxpt(20 + i * 10);
        enemies[i].cur_x = enemies[i].x;
        enemies[i].y = fxpt(80 - i * 25);
        enemies[i].cur_y = enemies[i].y;
    }
}

void select_enemy_attacks()
{
    for (int i = 0; i < enemies_size; i++)
    {
        if (!enemies[i].is_alive) continue;

        int target = random(party_size);
        while (!party[target].is_alive)
        {
            target++;
            if (target >= party_size) target = 0;
        }
        add_action_to_battle_queue(AT_ATTACK, &enemies[i], &party[target]);
    }
}
