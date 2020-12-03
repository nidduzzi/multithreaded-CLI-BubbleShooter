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
#include <errno.h>
#include <math.h>
#include <sys/queue.h>
#define PI 3.14159265

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
    BUBBLE_NONE,
    BUBBLE_RED,
    BUBBLE_BLUE,
    BUBBLE_GREEN,
    BUBBLE_YELLOW,
    BUBBLE_CYAN,
    BUBBLE_MAGENTA,
    BUBBLE_WHITE,
    BUBBLE_BLACK
} bc_t;

typedef enum draw_object_type
{
    DRAW_ALL,
    DRAW_THIS,
    DRAW_TARGET,
    DRAW_BULLET,
    DRAW_ARROW,
    DRAW_SCORE,
    DRAW_CLEAR
} drawobj_t;

typedef enum draw_state_type
{
    DRAW_STATE_INIT,
    DRAW_STATE_START,
    DRAW_STATE_END,
    DRAW_STATE_EXIT,
    DRAW_STATE_ERROR
} draw_state_t;

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
    double angle_deg;
    double speed;
    bc_t color;
} b_t;

typedef struct bubble_type *bubbleptr_t;

typedef struct bubble_type
{
    double x;
    double y;
    bc_t color;
    int modified;
} bubble_t;

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

errorbuffer_t errbuffer; // Global variable for buffering printf

typedef struct container_bubble_type
{
    bubble_t container;
    LIST_ENTRY(container_bubble_type) entries;
} cbubble_t;

typedef struct container_drawobj_type
{
    drawobj_t object;
    TAILQ_ENTRY(container_drawobj_type) entries;
} cdrawobj_t;

typedef struct container_input_type
{
    int c;
    TAILQ_ENTRY(container_input_type) entries;
} cinput_t;

typedef struct target_type
{
    LIST_HEAD(bubble_list, container_bubble_type) bubbles;
    int num;
    double x;
    double y;
} target_t;

typedef struct window_attributes_type
{
    WINDOW *win;
    int cols;
    int lines;
    int maxcols;
    int maxlines;
} wattr_t;

typedef struct thread_type
{
    pthread_t tid;
    pthread_attr_t attr;
} thread_t;

typedef struct game_object_type
{
    gameIs_t state;
    int score;
    la_t assets;
    b_t bullet;
    target_t targets;
    int activeCtypes[9];
    int num_activeCtypes;
    wattr_t wattr;
    pthread_mutex_t game_mutex;
    pthread_t draw_thread;
    pthread_cond_t draw_cv;
    gameIs_t draw_state;
    pthread_t input_thread;
    pthread_cond_t input_cv;
    gameIs_t input_state;
    pthread_t mechanics_thread;
    pthread_cond_t mechanics_cv;
    gameIs_t mechanics_state;
    int draw_signaled;
    int input_signaled;
    int mechanics_signaled;
    TAILQ_HEAD(draw_queue_t, container_drawobj_type) draw_queue;
    TAILQ_HEAD(input_queue_t, container_input_type) input_queue;
    //...
} game_o_t;

pthread_mutex_t errbuff_mutex;

int errbuff(const char *s, ...);
int textLoader(char address[], char ***dest, int *maxcol, int *maxline);
int loadAssetsFromFile(game_o_t *game, int level);
void *fwrapper_textLoader(void *args);
void drawbubble(wattr_t *wattr, bubble_t bubble, sprite_t sprite);
double lineEq(double x, double m, double c);
void drawarrow(wattr_t *wattr, double angle, target_t *targets, sprite_t *sprite);
int game_loop(WINDOW *win, int level);
// ...

#endif
