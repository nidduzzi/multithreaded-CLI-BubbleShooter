#ifndef LIB_H
#define LIB_H
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <malloc.h>

typedef enum game_internal_state_t {
    GAME_INIT, // Initialize level assets(bg, level design, sprites, prng seed, etc)
    BULLET_READY, // Bullet has been initialized (color/type set) and waiting for player to aim and shoot bullet --> update state to BULLET_FIRED after player fires bullet
    BULLET_FIRED, /* Player has set bullet angle and fired the bullet --> update positon and reflect angle if barrier is hit --> update state to BULLET_HIT when distance to any bubble is less than update displacement*/
    BULLET_HIT /* Bullet has hit a bubble --> comprison of bullet and bubble color --> destroy bubbles if 3 same color are adjacent --> destroy bubbles not connected to original layout --> Count score --> rng get next bullet type --> update state to BULLET_READY*/
}g_is_t;

typedef struct level_design{
    char **bubble_layout;
    char **bg_layout;
}l_d;

typedef struct bullet{
    double x;
    double y;
    double rad_angle;
    double speed;
}b;

typedef struct game_state{
    g_is_t _state;
    int score;
    l_d _level_design;
    //...
}g_s;

void setGameInternalState(g_s *game_state, g_is_t state);
// ...




#endif
