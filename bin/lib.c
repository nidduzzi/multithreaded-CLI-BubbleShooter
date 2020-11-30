#include "lib.h"

int errbuff(const char *s, ...)
{

    if (errbuffer.buffer == NULL)
    {
        errbuffer.buffer = (char *)malloc(sizeof(char));
        errbuffer.size = 0;
        errbuffer.num = 0;
    }
    char *tmpptr = NULL;
    ++errbuffer.num;
    va_list args;
    va_start(args, s);
    int len = vasprintf(&tmpptr, s, args);
    va_end(args);
    if (len != -1 && tmpptr != NULL)
    {
        errbuffer.buffer = (char *)realloc(errbuffer.buffer, (size_t)((errbuffer.size += len) * sizeof(char)));
        strcat(errbuffer.buffer, tmpptr);
        free(tmpptr);
    }
    else
        return -1;
    return 0;
}

int textLoader(char address[], char ***dest, int *maxcol, int *maxline)
{
    FILE *fp = fopen(address, "r");
    int eofcount = 0, linenum = 0, colnum = 0;
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
            if (colnum == 0)
                (*dest)[linenum] = (char *)malloc(sizeof(char));
            else
                (*dest)[linenum] = (char *)realloc((*dest)[linenum], (colnum + 1) * sizeof(char));
            (*dest)[linenum][colnum] = c;
            ++colnum;
            if (c == '\n')
            {
                ++linenum;
                if ((*maxcol) < (colnum - 1))
                    *maxcol = colnum;
                colnum = 0;
                (*dest) = (char **)realloc((*dest), (linenum + 1) * sizeof(*(*dest)));
            }
        }
        *maxline = linenum;
        for (linenum = 0; linenum < *maxline; ++linenum)
        {
            (*dest)[linenum] = realloc((*dest)[linenum], (*maxcol) * sizeof(char));
        }
    }
    fclose(fp);
}

int game_loop(int level)
{
    game_o_t game = {};
    pthread_t *threads;
    threads = (pthread_t *)malloc(sizeof(pthread_t)); // initial thread assignment
    int threadnum = 0, returnValue = 0;
    WINDOW *win;
    game.state = GAME_INIT;
    while (game.state != GAME_END && game.state != GAME_ERROR)
    {
        switch (game.state)
        {
        case GAME_INIT:
            if (loadAssets(&game, level))
            {

                threads = (pthread_t *)realloc(threads, threadnum * (sizeof(pthread_t)));
                ++threadnum;
                errbuff("number of threads alloccated after loadAssets in game_loop(): %d\n", threadnum);

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
    free(threads);
    return returnValue;
}

int loadAssets(game_o_t *game, int level)
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
    // clock_t cstart = clock();
    // pthread_t threads[4];
    int i;
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
    return 1;
}

void *fwrapper_textLoader(void *args)
{
    tLoaderArg_t *params = (tLoaderArg_t *)args;
    textLoader(params->addr, params->dest, params->maxcol, params->maxline);
    return (void *)0;
}
