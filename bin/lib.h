#ifndef LIB_H
#define LIB_H
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <malloc.h>

typedef enum game_internal_state_t
{
    GAME_INIT,    // Initialize level assets(bg, level design, sprites, prng seed, etc)
    BULLET_READY, // Bullet has been initialized (color/type set) and waiting for player to aim and shoot bullet --> update state to BULLET_FIRED after player fires bullet
    BULLET_FIRED, /* Player has set bullet angle and fired the bullet --> update positon and reflect angle if barrier is hit --> update state to BULLET_HIT when distance to any bubble is less than update displacement*/
    BULLET_HIT    /* Bullet has hit a bubble --> comprison of bullet and bubble color --> destroy bubbles if 3 same color are adjacent --> destroy bubbles not connected to original layout --> Count score --> rng get next bullet type --> update state to BULLET_READY*/
} g_is_t;

typedef struct assetF_addr_t
{
    char *bgF_addr;
    char *bubbleF_addr;
    char *bubble_layoutF_addr;
    char *arrowF_addr;
} asset_addr_t;

typedef struct level_design_t
{
    char **bubble_layout;
    char **bg;
    char **arrow;
} l_d_t;

typedef struct bullet_t
{
    double x;
    double y;
    double angle_rad;
    double speed;
} b_t;

typedef struct game_object_t
{
    g_is_t _state;
    int score;
    l_d_t level_design;
    b_t bullet;
    //...
} g_o_t;

void setGameInternalState(g_o_t *game, g_is_t state);
void loadGameAssets(g_o_t *game, asset_addr_t);
// ...

#endif
