#include "lib.h"

int errbuff(const char *s, ...)
{
    if (errbuffer.buffer == NULL)
    {
        errbuffer.buffer = (char *)malloc(sizeof(char));
        errbuffer.size = 0;
        errbuffer.num = 0;
    }
    char *tmpptr = NULL, *errorstr = NULL;
    ++errbuffer.num;
    int errlen = asprintf(&errorstr, "errno(%d): %s\n\n", (int)errno, strerror(errno));
    va_list args;
    va_start(args, s);
    int len = vasprintf(&tmpptr, s, args);
    va_end(args);
    if (len != -1 && tmpptr != NULL)
    {
        errbuffer.buffer = (char *)realloc(errbuffer.buffer, (size_t)((errbuffer.size += (len + errlen + 2)) * sizeof(char)));
        strcat(errbuffer.buffer, tmpptr);
        strcat(errbuffer.buffer, errorstr);
        free(tmpptr);
        free(errorstr);
    }
    else
        return -1;
    return 0;
}

int textLoader(char address[], char ***dest, int *maxcol, int *maxline)
{
    FILE *fp = fopen(address, "r");
    int linenum = 0, colnum = 0;
    char c;
    if (fp == NULL)
    {
        errbuff("Failed to open file: %s\n", address);
        return -1;
    }
    else
    {
        *maxcol = 0, *maxline = 0;
        (*dest) = (char **)malloc(sizeof(**dest));
        while (fscanf(fp, "%c", &c) != EOF)
        {
            if (*dest == NULL)
            {
                errbuff("Failed to allocate memory!\n");
                return -1;
            }
            if (c == '\n')
            {
                ++linenum;
                if ((*maxcol) < (colnum - 1))
                    *maxcol = colnum;
                colnum = 0;
                (*dest) = (char **)realloc((*dest), (linenum + 1) * sizeof(*(*dest)));
            }
            else
            {
                if (colnum == 0)
                    (*dest)[linenum] = (char *)malloc(sizeof(char));
                else
                    (*dest)[linenum] = (char *)realloc((*dest)[linenum], (colnum + 1) * sizeof(char));
                (*dest)[linenum][colnum] = c;
                ++colnum;
            }
        }
        *maxline = linenum;
        for (linenum = 0; linenum < *maxline; ++linenum)
        {
            (*dest)[linenum] = realloc((*dest)[linenum], (*maxcol) * sizeof(char));
        }
    }
    fclose(fp);
    return 0;
}

int game_loop(WINDOW *win, int level)
{
    game_o_t game = {};
    pthread_t physicsThread, drawThread, inputThread; // initial thread assignment
    // TODO: Debugging purposes only
    start_color();
    init_pair(BUBBLE_BLACK, COLOR_BLACK, COLOR_WHITE);
    init_pair(BUBBLE_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(BUBBLE_RED, COLOR_RED, COLOR_BLACK);
    init_pair(BUBBLE_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(BUBBLE_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(BUBBLE_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BUBBLE_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(BUBBLE_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    elemen *ptr;
    // TODO: Debugging purposes only
    int returnValue = 0;
    game.state = GAME_INIT;
    while (game.state != GAME_END && game.state != GAME_ERROR)
    {
        switch (game.state)
        {
        case GAME_INIT:
            if (loadAssetsFromFile(&game, level))
            {
                game.wattr.win = win;
                game.wattr.cols = (game.assets.layout.maxcol / 2 + game.assets.layout.maxcol % 2) * game.assets.bubble.maxcol + 2;
                game.wattr.lines = 32;
                wresize(win, game.wattr.lines, game.wattr.cols);
                mvwin(win, (LINES / 2) - (game.wattr.lines / 2) - (game.wattr.lines % 2), (COLS / 2) - (game.wattr.cols / 2) - (game.wattr.cols % 2));
                errbuff("number of threads allocated after loadAssetsFromFile in game_loop(): 3\n");
                game.state = BULLET_READY;
            }
            else
            {
                returnValue = -1;
                errbuff("Failed to load assets. game_loop() returned: %d\n", returnValue);
                game.state = GAME_ERROR;
            }
            break;
        case BULLET_READY:
            wclear(win);
            box(win, 0, 0);
            wrefresh(win);
            ptr = game.targets.bubbles.first;
            while (ptr != NULL)
            {
                drawbubble(win, ptr->kontainer, game.assets.bubble);
                ptr = ptr->next;
            }
            wrefresh(win);
            game.state = GAME_END;
            break;
        case BULLET_FIRED:
            game.state = GAME_END;
            break;
        case BULLET_HIT:
            game.state = GAME_END;
            break;
        default:
            game.state = GAME_END;
            break;
        }
    }
    // TODO: Take care of post game stuff (Score, completion, free dynamic vars(sprites,etc) etc)
    return returnValue;
}

void drawbubble(WINDOW *win, bubble_t bubble, sprite_t sprite)
{
    int i, j;
    wattron(win, COLOR_PAIR(bubble.color) | A_STANDOUT);
    for (i = 0; i < sprite.maxline; ++i)
    {
        for (j = 0; j < sprite.maxcol; ++j)
        {
            wmove(win, (bubble.y / 2) - (sprite.maxline / 2) - (sprite.maxline % 2) + i + 1, bubble.x - (sprite.maxcol / 2) - (sprite.maxcol % 2) + j + 1);
            if (sprite.data[i][j] != ' ' && sprite.data[i][j] != '\n')
            {
                waddch(win, sprite.data[i][j]);
                wrefresh(win);
            }
        }
    }
    wattroff(win, COLOR_PAIR(bubble.color));
    wattroff(win, A_STANDOUT);
}

void *draw(void *args)
{
}

void *input(void *args)
{
}

void *physics(void *args)
{
}

int loadAssetsFromFile(game_o_t *game, int level)
{
    tLoaderArg_t params[4];
    sprintf(params[0].addr, "../data/bubble.txt");
    params[0].dest = &(game->assets.bubble.data);
    params[0].maxcol = &(game->assets.bubble.maxcol);
    params[0].maxline = &(game->assets.bubble.maxline);
    sprintf(params[1].addr, "../data/arrow.txt");
    params[1].dest = &(game->assets.arrow.data);
    params[1].maxcol = &(game->assets.arrow.maxcol);
    params[1].maxline = &(game->assets.arrow.maxline);
    sprintf(params[2].addr, "../data/l%d/background.txt", level);
    params[2].dest = &(game->assets.bg.data);
    params[2].maxcol = &(game->assets.bg.maxcol);
    params[2].maxline = &(game->assets.bg.maxline);
    sprintf(params[3].addr, "../data/l%d/layout.txt", level);
    params[3].dest = &(game->assets.layout.data);
    params[3].maxcol = &(game->assets.layout.maxcol);
    params[3].maxline = &(game->assets.layout.maxline);
    int i;
    // clock_t cstart = clock();
    // pthread_t threads[4];
    // for (i = 0; i < 4; ++i)
    // {
    //     pthread_create(&(threads[i]), NULL, fwrapper_textLoader, (void *)&(params[i]));
    // }
    // for (i = 0; i < 4; ++i)
    //     pthread_join(threads[i], NULL);
    for (i = 0; i < 4; ++i)
    {
        if (textLoader(params[i].addr, params[i].dest, params[i].maxcol, params[i].maxline) == -1)
            return -1;
    }
    // clock_t delta  = clock() - cstart;
    // errbuff("clock: %ld, time: %Lf\n", delta, (long double)delta/(long double)CLOCKS_PER_SEC);
    // LOAD LAYOUT TO GAME
    createList(&(game->targets.bubbles));
    game->targets.num = 0;
    for (i = 0; i < 7; ++i)
        game->activeCtypes[i] = 0;
    game->num_activeCtypes = 0;
    int j;
    for (i = 0; i < game->assets.layout.maxline; ++i)
    {
        for (j = 0; j < game->assets.layout.maxcol; ++j)
        {
            switch (game->assets.layout.data[i][j])
            {
            case 'r':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_RED, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_RED] = 1;
                break;

            case 'b':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_BLUE, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_BLUE] = 1;
                break;

            case 'g':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_GREEN, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_GREEN] = 1;
                break;

            case 'y':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_YELLOW, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_YELLOW] = 1;
                break;

            case 'c':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_CYAN, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_CYAN] = 1;
                break;

            case 'm':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_MAGENTA, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_MAGENTA] = 1;
                break;

            case 'w':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_WHITE, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_WHITE] = 1;
                break;

            case 'x':
                addLast((bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_BLACK, 0}, &(game->targets.bubbles));
                game->activeCtypes[BUBBLE_BLACK] = 1;
                break;
            }
        }
    }
    return 1;
}

void *fwrapper_textLoader(void *args)
{
    tLoaderArg_t *params = (tLoaderArg_t *)args;
    textLoader(params->addr, params->dest, params->maxcol, params->maxline);
    return (void *)0;
}

// custom list function definitions
void copyElement(bubble_t *dest, bubble_t *src)
{
    dest->color = src->color;
    dest->modified = src->modified;
    dest->x = src->x;
    dest->y = src->y;
}
