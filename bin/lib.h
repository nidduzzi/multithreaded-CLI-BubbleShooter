#ifndef LIB_H
#define LIB_H
#define _GNU_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include <locale.h>
// #include <sys/queue.h>

typedef enum game_internal_state_t
{
    GAME_INIT,    // Initialize level assets(bg, level design, sprites, prng seed, etc)
    BULLET_READY, // Bullet has been initialized (color/type set) and waiting for player to aim and shoot bullet --> update state to BULLET_FIRED after player fires bullet
    BULLET_FIRED, /* Player has set bullet angle and fired the bullet --> update positon and reflect angle if barrier is hit --> update state to BULLET_HIT when distance to any bubble is less than update displacement*/
    BULLET_HIT,   /* Bullet has hit a bubble --> comprison of bullet and bubble color --> destroy bubbles if 3 same color are adjacent --> destroy bubbles not connected to original layout --> Count score --> rng get next bullet type --> update state to BULLET_READY*/
    GAME_END,
    GAME_ERROR
} gameIs_t;

typedef enum bubble_color_type
{
    BUBBLE_RED,
    BUBBLE_BLUE,
    BUBBLE_GREEN,
    BUBBLE_YELLOW,
    BUBBLE_ORANGE,
    BUBBLE_WHITE,
    BUBBLE_BLACK
} bc_t;

typedef struct sprite_type
{
    char **data;
    int maxline;
    int maxcol;
} sprite_t;

typedef struct level_assets_type
{
    sprite_t bg;
    sprite_t layout;
    sprite_t bubble;
    sprite_t arrow;
} la_t;

typedef struct bullet_type
{
    double x;
    double y;
    double angle_rad;
    double speed;
    bc_t color;
} b_t;

typedef struct bubble_type *bubbleptr_t;

typedef struct bubble_type
{
    double x;
    double y;
    bc_t color;
    int player_bubble;
    bubbleptr_t sibling[6];
} bubble_t;

typedef struct target_type
{
    bubble_t *bubble;
    int num;
} target_t;

typedef struct game_object_type
{
    gameIs_t state;
    int score;
    la_t assets;
    b_t bullet;
    target_t targets;
    //...
} game_o_t;

typedef struct textLoaderArgs_type
{
    char addr[30];
    char ***dest;
    int *maxcol;
    int *maxline;
} tLoaderArg_t;

typedef struct errorbuffer_type
{
    char *buffer;
    int size;
    int num;
} errorbuffer_t;

errorbuffer_t errbuffer;

int errbuff(const char *s, ...);
int game_loop(int level);
int textLoader(char address[], char ***dest, int *maxcol, int *maxline);
int loadAssets(game_o_t *game, int level);
void *fwrapper_textLoader(void *args);
void setGameInternalState(game_o_t *game, gameIs_t state);
// ...

#endif
