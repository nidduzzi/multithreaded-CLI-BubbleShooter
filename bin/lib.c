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
    int len = vasprintf(&tmpptr, s, args), oldlen = errbuffer.size;
    va_end(args);
    if (len != -1 && tmpptr != NULL)
    {
        errbuffer.size += (len + errlen);
        errbuffer.buffer = (char *)realloc(errbuffer.buffer, (size_t)(errbuffer.size * sizeof(char)));
        memcpy((errbuffer.buffer + oldlen), tmpptr, len);
        memcpy(errbuffer.buffer + oldlen + len, errorstr, errlen);
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
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Failed to open file: %s\n", address);
        pthread_mutex_unlock(&(errbuff_mutex));
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
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Failed to allocate memory!\n");
                pthread_mutex_unlock(&(errbuff_mutex));
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
    // clock_t delta  = clock() - cstart;
    pthread_mutex_lock(&(errbuff_mutex));
    // errbuff("clock: %ld, time: %Lf\n", delta, (long double)delta/(long double)CLOCKS_PER_SEC);
    pthread_mutex_unlock(&(errbuff_mutex));
    for (i = 0; i < 4; ++i)
    {
        if (textLoader(params[i].addr, params[i].dest, params[i].maxcol, params[i].maxline) != 0)
            return -1;
    }
    // LOAD LAYOUT TO GAME
    LIST_INIT(&(game->targets.bubbles));
    struct container_bubble_type *ptr;
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
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_RED, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_RED] = 1;
                ++(game->targets.num);
                break;

            case 'b':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_BLUE, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_BLUE] = 1;
                ++(game->targets.num);
                break;

            case 'g':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_GREEN, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_GREEN] = 1;
                ++(game->targets.num);
                break;

            case 'y':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_YELLOW, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_YELLOW] = 1;
                ++(game->targets.num);
                break;

            case 'c':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_CYAN, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_CYAN] = 1;
                ++(game->targets.num);
                break;

            case 'm':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_MAGENTA, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_MAGENTA] = 1;
                ++(game->targets.num);
                break;

            case 'w':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_WHITE, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_WHITE] = 1;
                ++(game->targets.num);
                break;

            case 'x':
                ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                if (ptr == NULL)
                {
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("couldn't assign memory for adding to bubble list");
                    pthread_mutex_unlock(&(errbuff_mutex));
                    return -2;
                }
                ptr->container = (bubble_t){2 + (j * 2), 2 + (i * 4), BUBBLE_BLACK, 0};
                LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                game->activeCtypes[BUBBLE_BLACK] = 1;
                ++(game->targets.num);
                break;
            }
        }
    }
    return 0;
}

void *fwrapper_textLoader(void *args)
{
    tLoaderArg_t *params = (tLoaderArg_t *)args;
    textLoader(params->addr, params->dest, params->maxcol, params->maxline);
    return (void *)0;
}

void drawbubble(wattr_t *wattr, bubble_t bubble, sprite_t sprite)
{
    int i, j;
    wattron(wattr->win, COLOR_PAIR(bubble.color) | A_STANDOUT);
    for (i = 0; i < sprite.maxline; ++i)
    {
        for (j = 0; j < sprite.maxcol; ++j)
        {
            if (sprite.data[i][j] != ' ' && sprite.data[i][j] != '\n')
            {
                mvwaddch(wattr->win, (bubble.y / 2) - (sprite.maxline / 2) - (sprite.maxline % 2) + i + 1, bubble.x - (sprite.maxcol / 2) - (sprite.maxcol % 2) + j + 1, sprite.data[i][j]);
                wrefresh(wattr->win);
            }
        }
    }
    wattroff(wattr->win, COLOR_PAIR(bubble.color));
    wattroff(wattr->win, A_STANDOUT);
}

double lineEq(double x, double m, double c)
{
    return m * x + c;
}

void drawarrow(wattr_t *wattr, double angle, target_t *targets, sprite_t *sprite)
{
    int touchTarget = 0, line = wattr->lines - 1, col = (wattr->cols / 2) + (wattr->cols % 2) ? 1 : ((angle > 0.0) ? 0 : 1);
    cbubble_t *cbubbleptr;
    double x = 0.0, y = 0.0, m = tan(angle), c = 0.0, bx, by, leasterror, dotpos[8][2] = {{0.5, 0.2}, {0.5, 0.8}, {0.7, 0.8}, {0.3, 0.8}, {0.7, 0.5}, {0.3, 0.5}, {0.7, 0.2}, {0.3, 0.2}}, cerror;
    int leasterroridx, i;
    while (!touchTarget)
    {
        // reflect the line if it touches the sides of the game box
        if (lineEq(x, m, c) > (((double)(wattr->cols - 2) / 2)))
        {
            m = -m;
            c = (((double)(wattr->cols - 2)));
        }
        else if (lineEq(x, m, c) < -(((double)(wattr->cols - 2) / 2)))
        {
            m = -m;
            c = -(((double)(wattr->cols - 2)));
        }
        // calculate the next arrow segment column position on the next line
        col = (int)(((double)wattr->cols / 2) - (ceil(y = lineEq(x, m, c)) + 0.005 /*offset any potential floating point errors*/));
        // check current distance to the targets
        LIST_FOREACH(cbubbleptr, &(targets->bubbles), entries)
        {
            bx = (((double)wattr->lines - 2) * 2) - (cbubbleptr->container.y);
            by = (((double)wattr->cols - 2) / 2) - (cbubbleptr->container.x);
            if (sqrt(pow((bx - x), 2) + pow((by - y), 2)) < 4.1)
            {
                touchTarget = 1;
                break;
            }
        }
        if (line == 1)
            touchTarget = 1;
        if (touchTarget)
        {
            mvwaddch(wattr->win, line, col, sprite->data[0][0]);
        }
        else
        {
            for (i = 0, leasterroridx = 0, leasterror = MAXFLOAT; i < 8; ++i)
            {
                bx = (dotpos[i][1] + (dotpos[i][0] / m) - c) / (m + (1 / m)); /* Get the intersect coordinates */
                by = lineEq(bx, m, c);
                if (bx < x + 2 && bx > x)
                {
                    /* Calulate the distance of the braille dot from the line*/
                    if ((cerror = sqrt(pow(dotpos[i][0] - bx, 2) + pow(dotpos[i][1] - by, 2))) < leasterror)
                    {
                        leasterror = cerror;
                        leasterroridx = i;
                    }
                }
            }
            if (col > 0 && col < wattr->cols && line > 0 && line < wattr->lines)
                mvwaddch(wattr->win, line, col, sprite->data[leasterroridx / 2 + 1][leasterroridx % 2]);
        }
        --line;
        x += 2;
    }
}

void *draw(void *args)
{
    game_o_t *game = (game_o_t *)args;
    int *retval = (int *)malloc(sizeof(int));
    *retval = 0;
    *retval = pthread_mutex_lock(&(game->game_mutex));
    cbubble_t *cbubbleptr;
    if (*retval)
    {
        game->state = GAME_ERROR;
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Error: pthread_mutex_lock() in draw() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    while (game->state != GAME_END && game->state != GAME_ERROR)
    {
        switch (game->state)
        {
        case BULLET_READY:
            while (game->state == BULLET_READY)
            {
                game->draw_state = BULLET_READY;
                if ((*retval = pthread_cond_wait(&(game->draw_cv), &(game->game_mutex))))
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_cond_wait() in draw() BULLET_READY returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else
                {
                    game->draw_signaled = 1;
                    wclear(game->wattr.win);
                    box(game->wattr.win, 0, 0);
                    LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                    {
                        drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                    }
                    drawarrow(&(game->wattr), game->bullet.angle_deg, &(game->targets), &(game->assets.arrow));
                    wrefresh(game->wattr.win);
                }
                pthread_cond_destroy(&(game->draw_cv));
                pthread_cond_init(&(game->draw_cv), NULL);
            }
            break;

        case BULLET_FIRED:
            game->draw_state = BULLET_FIRED;
            pthread_cond_wait(&(game->draw_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in draw() BULLET_FIRED returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            pthread_cond_destroy(&(game->draw_cv));
            pthread_cond_init(&(game->draw_cv), NULL);
            pthread_mutex_unlock(&(game->game_mutex));
            break;

        case BULLET_HIT:
            game->draw_state = BULLET_HIT;
            /* code */
            break;

        default:
            break;
        }
    }
    *retval = pthread_mutex_unlock(&(game->game_mutex));
    if (*retval)
    {
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Error: pthread_mutex_unlock() in draw() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    return (void *)retval;
}

void *input(void *args)
{
    game_o_t *game = (game_o_t *)args;
    int *retval = (int *)malloc(sizeof(int)), key;
    cinput_t *cinputptr;
    *retval = 0;
    *retval = pthread_mutex_lock(&(game->game_mutex));
    if (*retval)
    {
        game->state = GAME_ERROR;
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Error: pthread_mutex_lock() in input() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    while (game->state != GAME_END && game->state != GAME_ERROR)
    {
        switch (game->state)
        {
        case BULLET_READY:
            game->input_state = BULLET_READY;
            if ((*retval = pthread_cond_wait(&(game->input_cv), &(game->game_mutex))))
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in input() BULLET_READY returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            else
            {
                if ((*retval = pthread_mutex_unlock(&(game->game_mutex))))
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_mutex_unlock() in input() BULLET_READY returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else
                {
                    game->input_signaled = 1;
                    nodelay(game->wattr.win, false);
                    noecho();
                    keypad(game->wattr.win, true);
                    while ((key = wgetch(game->wattr.win)) != 10)
                    {
                        cinputptr = (cinput_t *)malloc(sizeof(cinput_t));
                        cinputptr->c = key;
                        TAILQ_INSERT_TAIL(&(game->input_queue), cinputptr, entries);
                        pthread_cond_signal(&(game->mechanics_cv));
                    }
                    // insert the enter into the queue as the last key
                    cinputptr = (cinput_t *)malloc(sizeof(cinput_t));
                    cinputptr->c = key;
                    TAILQ_INSERT_TAIL(&(game->input_queue), cinputptr, entries);
                    pthread_cond_signal(&(game->mechanics_cv));
                }
            }
            pthread_cond_destroy(&(game->input_cv));
            pthread_cond_init(&(game->input_cv), NULL);
            break;

        case BULLET_FIRED:
            game->input_state = BULLET_FIRED;
            *retval = pthread_cond_wait(&(game->input_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in input() BULLET_FIRED returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            pthread_cond_destroy(&(game->input_cv));
            pthread_cond_init(&(game->input_cv), NULL);
            pthread_mutex_unlock(&(game->game_mutex));
            break;

        case BULLET_HIT:
            game->input_state = BULLET_HIT;
            /* code */
            break;

        default:
            break;
        }
        if (game->state != GAME_END)
        {
            *retval = pthread_cond_wait(&(game->input_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in input() after switch(game->state) returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            pthread_cond_destroy(&(game->input_cv));
            pthread_cond_init(&(game->input_cv), NULL);
            pthread_mutex_unlock(&(game->game_mutex));
        }
    }
    *retval = pthread_mutex_unlock(&(game->game_mutex));
    if (*retval)
    {
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Error: pthread_mutex_unlock() in input() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    return (void *)retval;
}

void *mechanics(void *args)
{
    game_o_t *game = (game_o_t *)args;
    int *retval = (int *)malloc(sizeof(int));
    double angle_increment = 0.05;
    cinput_t *cinputptr, *cinputptr1;
    *retval = 0;
    *retval = pthread_mutex_lock(&(game->game_mutex));
    if (*retval)
    {
        game->state = GAME_ERROR;
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Error: pthread_mutex_lock() in mechanics() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    while (game->state != GAME_END && game->state != GAME_ERROR)
    {
        switch (game->state)
        {
        case BULLET_READY:
            game->bullet.color = (bc_t)(1 + (rand() / RAND_MAX) * 8);
            game->bullet.angle_deg = 0.0;
            game->bullet.x = game->wattr.cols / 2;
            game->bullet.y = game->wattr.lines - 3;
            while (game->state == BULLET_READY)
            {
                game->mechanics_state = BULLET_READY;
                *retval = pthread_cond_wait(&(game->mechanics_cv), &(game->game_mutex));
                if (*retval)
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_cond_wait() in mechanics() BULLET_READY returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else
                {
                    // Unlock mutex so that input thread can continue to queue input
                    if ((*retval = pthread_mutex_unlock(&(game->game_mutex))))
                    {
                        game->state = GAME_ERROR;
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY returned: %d\n", *retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                    }
                    else
                    {
                        game->mechanics_signaled = 1;
                        cinputptr = TAILQ_FIRST(&(game->input_queue));
                        while (cinputptr != NULL)
                        {
                            switch (cinputptr->c)
                            {
                            case KEY_RIGHT:
                                // lock here so that mechanics thread waits for the draw thread to finish
                                *retval = pthread_mutex_lock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;
                                    pthread_mutex_lock(&(errbuff_mutex));
                                    errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_READY KEY_RIGHT returned: %d\n", *retval);
                                    pthread_mutex_unlock(&(errbuff_mutex));
                                }
                                else
                                {
                                    game->bullet.angle_deg -= angle_increment;
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;
                                        pthread_mutex_lock(&(errbuff_mutex));
                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY KEY_RIGHT returned: %d\n", *retval);
                                        pthread_mutex_unlock(&(errbuff_mutex));
                                    }
                                }
                                break;
                            case KEY_LEFT:
                                *retval = pthread_mutex_lock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;
                                    pthread_mutex_lock(&(errbuff_mutex));
                                    errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_READY KEY_LEFT returned: %d\n", *retval);
                                    pthread_mutex_unlock(&(errbuff_mutex));
                                }
                                else
                                {
                                    game->bullet.angle_deg += angle_increment;
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;
                                        pthread_mutex_lock(&(errbuff_mutex));
                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY KEY_LEFT returned: %d\n", *retval);
                                        pthread_mutex_unlock(&(errbuff_mutex));
                                    }
                                }
                                break;
                            case 10:
                                *retval = pthread_mutex_lock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;
                                    pthread_mutex_lock(&(errbuff_mutex));
                                    errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_READY 10 returned: %d\n", *retval);
                                    pthread_mutex_unlock(&(errbuff_mutex));
                                }
                                else
                                {
                                    game->state = BULLET_FIRED;
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;
                                        pthread_mutex_lock(&(errbuff_mutex));
                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY 10 returned: %d\n", *retval);
                                        pthread_mutex_unlock(&(errbuff_mutex));
                                    }
                                    else
                                    {
                                        pthread_cond_signal(&(game->input_cv));
                                    }
                                }
                                break;
                            }
                            pthread_cond_signal(&(game->draw_cv));
                            cinputptr1 = TAILQ_NEXT(cinputptr, entries);
                            if (!TAILQ_EMPTY(&(game->input_queue)))
                            {
                                TAILQ_REMOVE(&(game->input_queue), cinputptr, entries);
                                free(cinputptr);
                            }
                            if (cinputptr1 != NULL)
                                cinputptr = cinputptr1;
                            else
                                cinputptr = NULL;
                            // break loop if a game error occured
                            if (game->state == GAME_ERROR)
                                break;
                        }
                    }
                }
                pthread_cond_destroy(&(game->mechanics_cv));
                pthread_cond_init(&(game->mechanics_cv), NULL);
            }
            break;

        case BULLET_FIRED:
            game->mechanics_state = BULLET_FIRED;
            *retval = pthread_cond_wait(&(game->mechanics_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            pthread_cond_destroy(&(game->mechanics_cv));
            pthread_cond_init(&(game->mechanics_cv), NULL);
            pthread_mutex_unlock(&(game->game_mutex));
            break;

        case BULLET_HIT:
            game->mechanics_state = BULLET_HIT;
            /* code */
            break;

        default:
            break;
        }
    }
    *retval = pthread_mutex_unlock(&(game->game_mutex));
    if (*retval)
    {
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("Error: pthread_mutex_unlock() in mechanics() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    return (void *)retval;
}

int game_loop(WINDOW *win, int level)
{
    game_o_t game = {};
    int newmaxlines, newmaxcols;
    int retval = 0;
    // default thread atribute
    pthread_attr_t tmpattr;
    game.state = GAME_INIT;
    // clear the main menu
    wclear(win);
    wrefresh(win);
    while (game.state != GAME_END && game.state != GAME_ERROR)
    {
        switch (game.state)
        {
        case GAME_INIT:
            if ((retval = loadAssetsFromFile(&game, level)) == 0)
            {
                // Set next game state
                game.state = BULLET_READY;
                game.draw_state = GAME_INIT;
                game.input_state = GAME_INIT;
                game.mechanics_state = GAME_INIT;
                game.input_signaled = 0;
                game.mechanics_signaled = 0;
                game.draw_signaled = 0;
                // initialize window attributes
                game.wattr.win = win;
                game.wattr.cols = (game.assets.layout.maxcol / 2 + game.assets.layout.maxcol % 2) * game.assets.bubble.maxcol + 2;
                game.wattr.lines = 32;
                // initialize colors
                init_pair(BUBBLE_BLACK, COLOR_BLACK, COLOR_WHITE);
                init_pair(BUBBLE_BLUE, COLOR_BLUE, COLOR_BLACK);
                init_pair(BUBBLE_RED, COLOR_RED, COLOR_BLACK);
                init_pair(BUBBLE_GREEN, COLOR_GREEN, COLOR_BLACK);
                init_pair(BUBBLE_WHITE, COLOR_WHITE, COLOR_BLACK);
                init_pair(BUBBLE_YELLOW, COLOR_YELLOW, COLOR_BLACK);
                init_pair(BUBBLE_CYAN, COLOR_CYAN, COLOR_BLACK);
                init_pair(BUBBLE_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
                // initialize thread queues
                TAILQ_INIT(&(game.draw_queue));
                TAILQ_INIT(&(game.input_queue));
                // initialize threads and other related variables
                {
                    retval = pthread_attr_init(&tmpattr);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: phtread_attr_init(tmpattr) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_attr_setdetachstate(&tmpattr, PTHREAD_CREATE_JOINABLE);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: phtread_attr_setdetachstate(tmpattr) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_cond_init(&(game.draw_cv), NULL);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: phtread_cond_init(draw_cv) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_cond_init(&(game.input_cv), NULL);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: phtread_cond_init(input_cv) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_cond_init(&(game.mechanics_cv), NULL);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: phtread_cond_init(mechanics_cv) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_mutex_init(&(game.game_mutex), NULL);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: pthread_mutex_init(game_mutex) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_create(&(game.draw_thread), &tmpattr, draw, (void *)(&game));
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: pthread_create(draw_thread) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_create(&(game.mechanics_thread), &tmpattr, mechanics, (void *)(&game));
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: pthread_create(mechanics_thread) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_create(&(game.input_thread), &tmpattr, input, (void *)(&game));
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: pthread_create(input_thread) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                    retval = pthread_attr_destroy(&tmpattr);
                    if (retval)
                    {
                        pthread_mutex_lock(&(errbuff_mutex));
                        errbuff("Error: pthread_attr_destroy(tmpattr) returned: %d\n", retval);
                        pthread_mutex_unlock(&(errbuff_mutex));
                        return retval;
                    }
                }
            }
            else
            {
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Failed to load assets. loadAssetsFromFile() returned: %d\n", retval);
                pthread_mutex_unlock(&(errbuff_mutex));
                game.state = GAME_ERROR;
            }
            break;
        case BULLET_READY:
            // signal to draw thread to draw the initialized game once it's ready
            if (!game.draw_signaled && game.draw_state == BULLET_READY)
            {
                pthread_cond_signal(&(game.draw_cv));
            }
            // then signal to the input thread to start queueing inputs from the user
            if (game.draw_signaled && !game.input_signaled && game.input_state == BULLET_READY)
            {
                pthread_cond_signal(&(game.input_cv));
            }
            break;
        case BULLET_FIRED:
            // wait for all threads to reach this stage then signal to them the game state changed and to continue
            if (game.input_state == BULLET_FIRED && game.draw_state == BULLET_FIRED && game.mechanics_state == BULLET_FIRED)
            {
                game.state = BULLET_HIT;
                pthread_cond_signal(&(game.draw_cv));
                pthread_cond_signal(&(game.input_cv));
                pthread_cond_signal(&(game.mechanics_cv));
            }
            break;
        case BULLET_HIT:
            game.state = GAME_END;
            break;
        default:
            game.state = GAME_END;
            break;
        }
        getmaxyx(stdscr, newmaxlines, newmaxcols);
        // redraw the window if the terminal changed size
        if (newmaxlines != game.wattr.maxlines || newmaxcols != game.wattr.maxcols)
        {
            game.wattr.maxlines = newmaxlines;
            game.wattr.maxcols = newmaxcols;
            wclear(win);
            wresize(win, game.wattr.lines, game.wattr.cols);
            mvwin(win, (game.wattr.maxlines / 2) - (game.wattr.lines / 2) - (game.wattr.lines % 2), (game.wattr.maxcols / 2) - (game.wattr.cols / 2) - (game.wattr.cols % 2));
            box(win, 0, 0);
            wrefresh(win);
        }
    }
    int *threadret;
    // TODO: Take care of post game stuff (Score, completion, free dynamic vars(sprites,etc) etc)
    if (game.state == GAME_ERROR)
    {
        pthread_cancel(game.input_thread);
        pthread_cancel(game.draw_thread);
        pthread_cancel(game.mechanics_thread);
    }
    else if (game.state == GAME_END)
    {
        // signal to any threads still waiting on a condition variable signal
        pthread_cond_signal(&(game.draw_cv));
        pthread_cond_signal(&(game.input_cv));
        pthread_cond_signal(&(game.mechanics_cv));
        // join all threads to main thread
        pthread_join(game.draw_thread, (void **)&threadret);
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("draw_thread joined and returned: %d\n", *threadret);
        pthread_mutex_unlock(&(errbuff_mutex));
        pthread_join(game.input_thread, (void **)&threadret);
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("input_thread joined and returned: %d\n", *threadret);
        pthread_mutex_unlock(&(errbuff_mutex));
        pthread_join(game.mechanics_thread, (void **)&threadret);
        pthread_mutex_lock(&(errbuff_mutex));
        errbuff("mechanics_thread joined and returned: %d\n", *threadret);
        pthread_mutex_unlock(&(errbuff_mutex));
        // destroy the remaining condition variables
        pthread_cond_destroy(&(game.draw_cv));
        pthread_cond_destroy(&(game.input_cv));
        pthread_cond_destroy(&(game.mechanics_cv));
        // destroy the game mutex
        pthread_mutex_destroy(&(game.game_mutex));
        // clear the window and refresh
        wclear(win);
        wrefresh(win);
    }
    return retval;
}
