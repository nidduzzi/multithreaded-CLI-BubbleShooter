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

int textLoader(char address[], wchar_t ***dest, int *maxcol, int *maxline)
{
    FILE *fp = fopen(address, "r");
    int linenum = 0, colnum = 0;
    wchar_t c;
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
        (*dest) = (wchar_t **)malloc(sizeof(**dest));
        while ((c = fgetwc(fp)) != WEOF)
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
                (*dest) = (wchar_t **)realloc((*dest), (linenum + 1) * sizeof(*(*dest)));
            }
            else
            {
                if (colnum == 0)
                    (*dest)[linenum] = (wchar_t *)malloc(sizeof(wchar_t));
                else
                    (*dest)[linenum] = (wchar_t *)realloc((*dest)[linenum], (colnum + 1) * sizeof(wchar_t));
                (*dest)[linenum][colnum] = c;
                ++colnum;
            }
        }
        *maxline = linenum;
        for (linenum = 0; linenum < *maxline; ++linenum)
        {
            if ((*dest)[linenum] == NULL)
                (*dest)[linenum] = malloc(sizeof(wchar_t));
            (*dest)[linenum] = realloc((*dest)[linenum], ((*maxcol) + 1) * sizeof(wchar_t));
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
                mvwprintw(wattr->win, (int)round(bubble.y / 2) - (sprite.maxline / 2) + i + 1, (int)round(bubble.x) - (sprite.maxcol / 2) + j + 1, "%lc", sprite.data[i][j]);
                // wrefresh(wattr->win);
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
// TODO: Make collision calculations using line equation (y=mx+c) so that it doesn't skip over parts that isn't (x, y)
int collide(wattr_t *wattr, target_t *targets, double x, double y)
{
    cbubble_t *cbubbleptr;
    double bx, by;
    int retval = 0;
    LIST_FOREACH(cbubbleptr, &(targets->bubbles), entries)
    {
        bx = (((double)wattr->lines - 3) * 2) - (cbubbleptr->container.y);
        by = (((double)wattr->cols - 2) / 2) - (cbubbleptr->container.x);
        if ((atan(fabs((by - y) / (bx - x))) < (atan(2) / 2)) && sqrt(pow(((bx - x)), 2) + pow((by - y), 2)) < 4.0) //check for horizontal collision
        {
            retval = 1;
            break;
        }
        else if (sqrt(pow(((bx - x)), 2) + pow((by - y), 2)) < (2 * sqrt(5.0) + 0.000005)) //check for diagonal collision
        {
            retval = 1;
            break;
        }
    }
    return retval;
}

int bounce(double *x, double *m, double *c, double minY, double maxY)
{
    int retval = 0;
    if ((lineEq(*x, *m, *c) + 2.0) > maxY)
    {
        *c = 2 * lineEq(*x, *m, *c);
        *m = -*m;
        retval = 1;
    }
    else if ((lineEq(*x, *m, *c) - 2.0) < minY)
    {
        *c = 2 * lineEq(*x, *m, *c);
        *m = -*m;
        retval = 1;
    }
    return retval;
}

void drawarrow(wattr_t *wattr, double angle, target_t *targets, sprite_t *sprite)
{
    int touchTarget = 0, line = wattr->lines - 2, col = 1;
    double x = 0.0, y = 0.0, m = tan(angle), c = 0.0, bx, by, leasterror, dotpos[8][2] = {{1.6, 0.5}, {0.4, 0.5}, {1.6, 0.7}, {1.6, 0.3}, {1.0, 0.7}, {1.0, 0.3}, {0.4, 0.7}, {0.4, 0.3}}, cerror;
    int leasterroridx, i;
    while (!touchTarget)
    {
        // reflect the line if it touches the sides of the game box
        bounce(&x, &m, &c, -(((double)(wattr->cols - 2) / 2)), (((double)(wattr->cols - 2) / 2)));
        // calculate the next arrow segment column position on the next line
        col = (int)floor(((double)wattr->cols / 2) - (y = lineEq(x, m, c)) + 0.00000000005 /*offset any potential floating point errors*/);
        // check current distance to the targets
        touchTarget = collide(wattr, targets, x, y);
        if (line == 1)
            touchTarget = 1;
        // if (touchTarget)
        // {
        //     mvwprintw(wattr->win, line, col, "%lc", sprite->data[0][0]);
        // }
        // else
        // {
        // }
        leasterroridx = 0;
        for (i = 0, leasterror = MAXFLOAT; i < 8 && m != 0.0; ++i)
        {
            bx = ((dotpos[i][1] + floor(y)) + ((dotpos[i][0] + floor(x)) / m) - c) / (m + (1 / m)); /* Get the intersect coordinates */
            by = lineEq(bx, m, c);
            if (bx < x + 2 && bx > x)
            {
                /* Calulate the distance of the braille dot from the line*/
                if ((cerror = sqrt(pow(dotpos[i][0] + floor(x) - bx, 2) + pow(dotpos[i][1] + floor(y) - by, 2))) < leasterror)
                {
                    leasterror = cerror;
                    leasterroridx = i;
                }
            }
        }
        if (col > 0 && col < (wattr->cols - 1) && line > 0 && line < (wattr->lines - 1))
        {
            mvwprintw(wattr->win, line, col, "%lc", sprite->data[leasterroridx + 1][0]);
            // TODO:REMOVE AFTER DEBUG
            wrefresh(wattr->win);
            // TODO:REMOVE AFTER DEBUG
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
            game->draw_state = BULLET_READY;
            while (game->state == BULLET_READY)
            {
                if ((*retval = pthread_cond_wait(&(game->draw_cv), &(game->game_mutex))))
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_cond_wait() in draw() BULLET_READY returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else if (game->state == BULLET_READY)
                {
                    game->draw_signaled = 1;
                    wclear(game->wattr.win);
                    box(game->wattr.win, 0, 0);
                    LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                    {
                        drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                    }
                    drawarrow(&(game->wattr), game->bullet.angle_deg, &(game->targets), &(game->assets.arrow));
                    drawbubble(&(game->wattr), (bubble_t){((game->wattr.cols - 2) / 2) - game->bullet.y, ((game->wattr.lines - 3) * 2) - game->bullet.x, game->bullet.color, 0}, game->assets.bubble);
                    mvwprintw(game->wattr.win, game->wattr.lines - 1, game->wattr.cols - 11, "%lf", game->bullet.angle_deg);
                    wrefresh(game->wattr.win);
                }
                pthread_cond_destroy(&(game->draw_cv));
                pthread_cond_init(&(game->draw_cv), NULL);
            }
            break;

        case BULLET_FIRED:
            game->draw_state = BULLET_FIRED;
            while (game->state == BULLET_FIRED)
            {
                if ((*retval = pthread_cond_wait(&(game->draw_cv), &(game->game_mutex))))
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_cond_wait() in draw() BULLET_FIRED returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else if (game->state == BULLET_FIRED)
                {
                    game->draw_signaled = 1;
                    wclear(game->wattr.win);
                    box(game->wattr.win, 0, 0);
                    LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                    {
                        drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                    }
                    drawbubble(&(game->wattr), (bubble_t){((game->wattr.cols - 2) / 2) - game->bullet.y, ((game->wattr.lines - 3) * 2) - game->bullet.x, game->bullet.color, 0}, game->assets.bubble);
                    mvwprintw(game->wattr.win, game->wattr.lines - 1, 1, "%lf, %lf", game->bullet.y, game->bullet.x);
                    mvwprintw(game->wattr.win, game->wattr.lines - 1, game->wattr.cols - 11, "%lf", game->bullet.angle_deg);
                    wrefresh(game->wattr.win);
                }
                pthread_cond_destroy(&(game->draw_cv));
                pthread_cond_init(&(game->draw_cv), NULL);
            }
            break;

        case BULLET_HIT:
            game->draw_state = BULLET_HIT;
            if ((*retval = pthread_cond_wait(&(game->draw_cv), &(game->game_mutex))))
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in draw() BULLET_HIT returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            else if (game->state == BULLET_HIT)
            {
            }
            pthread_cond_destroy(&(game->draw_cv));
            pthread_cond_init(&(game->draw_cv), NULL);
            break;

        default:
            break;
        }
        if (game->state != GAME_END || game->state != GAME_ERROR)
        {
            *retval = pthread_mutex_unlock(&(game->game_mutex));
            if (*retval)
            {
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_mutex_unlock() in draw() while() after switch (game->state) returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
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
    game->input_state = BULLET_READY;
    while (game->state != GAME_END && game->state != GAME_ERROR)
    {
        switch (game->state)
        {
        case BULLET_READY:
            if (game->input_state == BULLET_READY)
            {
                if ((*retval = pthread_cond_wait(&(game->input_cv), &(game->game_mutex))))
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_cond_wait() in input() BULLET_READY returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else if (game->state == BULLET_READY)
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
                        game->input_signaled = 0;
                    }
                }
                pthread_cond_destroy(&(game->input_cv));
                pthread_cond_init(&(game->input_cv), NULL);
                game->input_state = BULLET_FIRED;
            }
            break;

        case BULLET_FIRED:
            *retval = pthread_cond_wait(&(game->input_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in input() BULLET_FIRED returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            game->input_signaled = 1;
            pthread_cond_destroy(&(game->input_cv));
            pthread_cond_init(&(game->input_cv), NULL);
            *retval = pthread_mutex_unlock(&(game->game_mutex));
            if (*retval)
            {
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_mutex_unlock() in input() BULLET_FIRED returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            game->input_signaled = 0;
            break;

        case BULLET_HIT:
            game->input_state = BULLET_HIT;
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
        errbuff("Error: pthread_mutex_unlock() in input() returned: %d\n", *retval);
        pthread_mutex_unlock(&(errbuff_mutex));
    }
    return (void *)retval;
}

void *mechanics(void *args)
{
    // * thread variables
    game_o_t *game = (game_o_t *)args;
    int *retval = (int *)malloc(sizeof(int));
    *retval = 0;
    // * Bullet variables
    double angle_increment = 0.01;
    double tmpx, tmpy, tmpc, tmpm;
    game->bullet.speed = 5.0;
    // * variables for handling input queue
    cinput_t *cinputptr, *cinputptr1;
    // * Timer variables
    struct timeval start, end;
    gettimeofday(&start, NULL);
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
        { // do once
            game->bullet.color = (bc_t)(1 + (rand() / RAND_MAX) * 8);
            game->bullet.angle_deg = 0.0;
            game->bullet.x = 0.0;
            game->bullet.y = 0.0;
            while (game->state == BULLET_READY)
            {
                game->mechanics_state = BULLET_READY;
                // Sleep the thread while there is no input to process
                *retval = pthread_cond_wait(&(game->mechanics_cv), &(game->game_mutex));
                if (*retval)
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_cond_wait() in mechanics() BULLET_READY returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else if (game->state == BULLET_READY)
                {
                    // Unlock mutex so that draw thread can continue to draw
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
                            gettimeofday(&end, NULL);
                            if (((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) > 1000000.0 / 25.0)
                            {
                                pthread_cond_signal(&(game->draw_cv));
                                gettimeofday(&start, NULL);
                            }
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
                        game->mechanics_signaled = 0;
                    }
                }
                pthread_cond_destroy(&(game->mechanics_cv));
                pthread_cond_init(&(game->mechanics_cv), NULL);
            }
        }
        break;

        case BULLET_FIRED:
        {
            // TODO: Make grid snap function to lock incoming bullet to existing bubble grid
            pthread_mutex_lock(&(game->game_mutex));
            game->mechanics_state = BULLET_FIRED;
            tmpx = game->bullet.x;
            tmpy = game->bullet.y;
            tmpm = tan(game->bullet.angle_deg);
            tmpc = 0.0;
            gettimeofday(&start, NULL);
            *retval = pthread_cond_wait(&(game->mechanics_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_cond_wait() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
            else
            {
                *retval = pthread_mutex_unlock(&(game->game_mutex));
                if (*retval)
                {
                    game->state = GAME_ERROR;
                    pthread_mutex_lock(&(errbuff_mutex));
                    errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                    pthread_mutex_unlock(&(errbuff_mutex));
                }
                else
                {
                    game->mechanics_signaled = 1;
                    while (game->state == BULLET_FIRED)
                    {
                        // * Move bullet along the line
                        gettimeofday(&end, NULL);
                        if (((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) > 1000000.0 / 25.0)
                        {
                            // * increment bullet position according to current time
                            tmpx += ((double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec)) / 1000000.0) * game->bullet.speed * cos(game->bullet.angle_deg);
                            bounce(&tmpx, &tmpm, &tmpc, -(((double)(game->wattr.cols - 2) / 2)), (((double)(game->wattr.cols - 2) / 2)));
                            tmpy = lineEq(tmpx, tmpm, tmpc);
                            *retval = pthread_mutex_lock(&(game->game_mutex));
                            if (*retval)
                            {
                                game->state = GAME_ERROR;
                                pthread_mutex_lock(&(errbuff_mutex));
                                errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                                pthread_mutex_unlock(&(errbuff_mutex));
                            }
                            else
                            {
                                if (collide(&(game->wattr), &(game->targets), tmpx, tmpy))
                                {
                                    // TODO: Implement collision grid snapping here buy calculating arctan of the distance and using the angle to divide into sections and calculate which section is closest and snap to that position
                                    game->bullet.x = tmpx;
                                    game->bullet.y = tmpy;
                                    game->state = BULLET_HIT;
                                    pthread_cond_signal(&(game->draw_cv));
                                }
                                else if ((int)(floor(tmpx) - game->bullet.x) != 0 || (int)(floor(tmpy) - game->bullet.y) != 0)
                                {
                                    game->bullet.x = tmpx;
                                    game->bullet.y = tmpy;
                                    pthread_cond_signal(&(game->draw_cv));
                                }
                                *retval = pthread_mutex_unlock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;
                                    pthread_mutex_lock(&(errbuff_mutex));
                                    errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                                    pthread_mutex_unlock(&(errbuff_mutex));
                                }
                            }
                            gettimeofday(&start, NULL);
                        }
                    }
                }
                game->mechanics_signaled = 0;
            }
            pthread_cond_destroy(&(game->mechanics_cv));
            pthread_cond_init(&(game->mechanics_cv), NULL);
        }
        break;

        case BULLET_HIT:
            game->mechanics_state = BULLET_HIT;
            /* code */
            break;

        default:
            break;
        }
        if (game->state != GAME_END || game->state != GAME_ERROR)
        {
            *retval = pthread_mutex_unlock(&(game->game_mutex));
            if (*retval)
            {
                pthread_mutex_lock(&(errbuff_mutex));
                errbuff("Error: pthread_mutex_unlock() in mechanics() while() after switch(game->state) returned: %d\n", *retval);
                pthread_mutex_unlock(&(errbuff_mutex));
            }
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
            if (game.input_state == BULLET_FIRED && game.draw_state == BULLET_FIRED && game.mechanics_state == BULLET_FIRED && (game.mechanics_signaled == 0))
            {
                pthread_cond_signal(&(game.mechanics_cv));
            }
            break;
        case BULLET_HIT:
            wgetch(win);
            game.state = GAME_END;
            if (game.input_state == BULLET_FIRED && game.draw_state == BULLET_FIRED && game.mechanics_state == BULLET_FIRED)
            {
                pthread_cond_signal(&(game.input_cv));
                pthread_cond_signal(&(game.draw_cv));
                pthread_cond_signal(&(game.mechanics_cv));
            }
            break;
        default:
            game.state = GAME_END;
            break;
        }
        getmaxyx(stdscr, newmaxlines, newmaxcols);
        // redraw the window if the terminal changed size
        if ((newmaxlines != game.wattr.maxlines || newmaxcols != game.wattr.maxcols) && pthread_mutex_trylock(&(game.game_mutex)))
        {
            game.wattr.maxlines = newmaxlines;
            game.wattr.maxcols = newmaxcols;
            wclear(win);
            wresize(win, game.wattr.lines, game.wattr.cols);
            mvwin(win, (game.wattr.maxlines / 2) - (game.wattr.lines / 2) - (game.wattr.lines % 2), (game.wattr.maxcols / 2) - (game.wattr.cols / 2) - (game.wattr.cols % 2));
            box(win, 0, 0);
            wrefresh(win);
            pthread_mutex_unlock(&(game.game_mutex));
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
    spriteUnloader(&(game.assets.arrow));
    spriteUnloader(&(game.assets.bg));
    spriteUnloader(&(game.assets.bubble));
    spriteUnloader(&(game.assets.layout));
    targetUnloader(&(game.targets));
    return retval;
}

void spriteUnloader(sprite_t *sprite)
{
    wchar_t ***dest = &(sprite->data);
    int *maxline = &(sprite->maxline);
    int linenum = 0;
    for (linenum = 0; linenum < *maxline; ++linenum)
    {
        free((*dest)[linenum]);
    }
    free((*dest));
}

void targetUnloader(target_t *target)
{
    cbubble_t *ptr1 = LIST_FIRST(&(target->bubbles)), *ptr2;
    while (ptr1 != NULL)
    {
        ptr2 = LIST_NEXT(ptr1, entries);
        free(ptr1);
        ptr1 = ptr2;
    }
}
