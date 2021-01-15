#include "lib.h"

int errbuff(const char *s, ...)
{
    pthread_mutex_lock(&(errbuff_mutex));
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
        errbuffer.buffer = (char *)realloc(errbuffer.buffer, (errbuffer.size * sizeof(char)));
        memcpy((errbuffer.buffer + oldlen), tmpptr, len);
        memcpy(errbuffer.buffer + oldlen + len, errorstr, errlen);
        free(tmpptr);
        free(errorstr);
        pthread_mutex_unlock(&(errbuff_mutex));
        return 0;
    }
    else
    {
        pthread_mutex_unlock(&(errbuff_mutex));
        return -1;
    }
}

int textLoader(char address[], wchar_t ***dest, int *maxcol, int *maxline)
{
    FILE *fp = fopen(address, "r");
    int linenum = 0, colnum = 0;
    wchar_t c;
    if (fp == NULL)
    {

        errbuff("Failed to open file: %s\n", address);

        return -1;
    }
    else
    {
        *maxcol = 0, *maxline = 0;
        (*dest) = (wchar_t **)malloc(sizeof(**dest));
        (*dest)[linenum] = NULL;
        while ((c = fgetwc(fp)) != WEOF)
        {
            (*dest)[linenum] = (wchar_t *)realloc((*dest)[linenum], (colnum + 1) * sizeof(wchar_t));
            if (*dest == NULL)
            {

                errbuff("Failed to allocate memory!\n");

                return -1;
            }
            if (c == '\n')
            {
                (*dest)[linenum][colnum] = L'\0';
                ++linenum;
                if ((*maxcol) < (colnum))
                    *maxcol = colnum;
                colnum = 0;
                (*dest) = (wchar_t **)realloc((*dest), (linenum + 1) * sizeof(wchar_t *));
                (*dest)[linenum] = NULL;
            }
            else
            {
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
    char str[30] = "";
    sprintf(str, "../data/l%d/layout.txt", level);
    if (textLoader("../data/bubble.txt", &(game->assets.bubble.data), &(game->assets.bubble.maxcol), &(game->assets.bubble.maxline)) != 0)
        return -1;
    if (textLoader("../data/arrow.txt", &(game->assets.arrow.data), &(game->assets.arrow.maxcol), &(game->assets.arrow.maxline)) != 0)
        return -1;
    if (textLoader(str, &(game->assets.layout.data), &(game->assets.layout.maxcol), &(game->assets.layout.maxline)) != 0)
        return -1;
    // LOAD LAYOUT TO GAME
    LIST_INIT(&(game->targets.bubbles));
    struct container_bubble_type *ptr;
    game->targets.num = 0;
    int i, j;
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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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

                    errbuff("couldn't assign memory for adding to bubble list");

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
                mvwprintw(wattr->win, (int)floor(bubble.y / 2) - (sprite.maxline / 2) + i + 1, (int)bubble.x - (sprite.maxcol / 2) + j + 1, "%lc", sprite.data[i][j]);
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

bubble_t *collide(wattr_t *wattr, target_t *targets, double m, double c, double x, double y)
{
    cbubble_t *cbubbleptr = NULL;
    double bx, by, x1, x2, a, b, constant;
    int retval = 0;
    LIST_FOREACH(cbubbleptr, &(targets->bubbles), entries)
    {
        bx = (((double)wattr->lines - 3) * 2) - (cbubbleptr->container.y);
        by = (((double)wattr->cols - 2) / 2) - (cbubbleptr->container.x);
        a = 1 + pow(m, 2);
        b = 2 * (m * c - m * by - bx);
        constant = pow(by, 2) + pow(bx, 2) + pow(c, 2) - (2 * c * by);                                         // Subtract radius squared to use constant
        if ((atan(fabs((by - y) / (bx - x))) > ((atan(0.5)))) && (discriminant(a, b, constant - 16.0) >= 0.0)) //check for horizontal collision
        {
            x1 = (-b + sqrt(discriminant(a, b, constant - 16.0))) / (2 * a);
            x2 = (-b - sqrt(discriminant(a, b, constant - 16.0))) / (2 * a);
            if (((x >= x1) || (x >= x2))) // If at least one of the intersection points is less than or equal to the current x then we have intersected one of the bubbles
            {
                retval = 1;
                break;
            }
        }
        else if ((discriminant(a, b, constant - 20.0) >= 0.0)) //check for diagonal collision
        {
            x1 = (-b + sqrt(discriminant(a, b, constant - 20.0))) / (2 * a);
            x2 = (-b - sqrt(discriminant(a, b, constant - 20.0))) / (2 * a);
            if (((x >= x1) || (x >= x2))) // If at least one of the intersection points is less than or equal to the current x then we have intersected one of the bubbles
            {
                retval = 1;
                break;
            }
        }
    }
    if (retval == 1)
        return &(cbubbleptr->container);
    else
        return NULL;
}

double discriminant(double a, double b, double c)
{
    return pow(b, 2) - 4 * (a * c);
}

int bounce(double *x, double *m, double *c, double minY, double maxY)
{
    int retval = 0;
    if ((lineEq(*x, *m, *c) + 2.0) > maxY)
    {
        *c = maxY - 2.0 + lineEq(*x, *m, *c) - *c;
        *m = -(*m);
        retval = 1;
    }
    else if ((lineEq(*x, *m, *c) - 2.0) < minY)
    {
        *c = minY + 2.0 + lineEq(*x, *m, *c) - *c;
        *m = -(*m);
        retval = 1;
    }
    return retval;
}

void drawarrow(wattr_t *wattr, double angle, target_t *targets, sprite_t *sprite)
{
    int touchTarget = 0, line = wattr->lines - 2, col = 1;
    double x = 0.0, y = 0.0, m = tan(angle), c = 0.0, bx, by, leasterror, dotpos[8][2] = {{1.6, 0.5}, {0.4, 0.5}, {1.6, 0.9}, {1.6, 0.1}, {1.0, 0.9}, {1.0, 0.1}, {0.4, 0.9}, {0.4, 0.1}}, cerror;
    int leasterroridx, i;
    while (!touchTarget)
    {
        // reflect the line if it touches the sides of the game box
        bounce(&x, &m, &c, -(((double)(wattr->cols - 2) / 2)), (((double)(wattr->cols - 2) / 2)));
        // calculate the next arrow segment column position on the next line
        col = (int)round(((double)wattr->cols / 2.0) - (y = lineEq(x, m, c)) + 0.00000000005 /*offset any potential floating point errors*/);
        // check current distance to the targets
        if (line == 1 || collide(wattr, targets, m, c, x, y) != NULL)
            touchTarget = 1;

        for (i = 0, leasterror = MAXFLOAT, leasterroridx = 0; i < 8 && m != 0.0; ++i)
        {
            bx = ((dotpos[i][1] + round(y)) + ((dotpos[i][0] + x) / m) - c) / (m + (1 / m)); /* Get the intersect coordinates */
            if (bx < x + 2 && bx > x)
            {
                by = lineEq(bx, m, c);
                /* Calulate the distance of the braille dot from the line*/
                if ((cerror = sqrt(pow(dotpos[i][0] + x - bx, 2) + pow(dotpos[i][1] + round(y) - by, 2))) < leasterror)
                {
                    leasterror = cerror;
                    leasterroridx = i;
                }
            }
        }
        if (fabs(m) == 0.0)
            leasterroridx = 4;
        if (col > 0 && col < (wattr->cols - 1) && line > 0 && line < (wattr->lines - 1))
        {
            mvwprintw(wattr->win, line, col, "%lc", sprite->data[leasterroridx + 1][0]);
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

        errbuff("Error: pthread_mutex_lock() in draw() returned: %d\n", *retval);
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

                    errbuff("Error: pthread_cond_wait() in draw() BULLET_READY returned: %d\n", *retval);
                }
                else if (game->state == BULLET_READY)
                {
                    game->draw_signaled = 1;
                    wclear(game->wattr.win);
                    box(game->wattr.win, 0, 0);
                    wattron(game->wattr.win, A_REVERSE);
                    mvwprintw(game->wattr.win, 0, 1, "Score: %d", game->score);
                    wattroff(game->wattr.win, A_REVERSE);
                    drawarrow(&(game->wattr), game->bullet.angle_deg, &(game->targets), &(game->assets.arrow));
                    LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                    {
                        drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                    }
                    drawbubble(&(game->wattr), (bubble_t){((game->wattr.cols - 2) / 2) - game->bullet.y, ((game->wattr.lines - 3) * 2) - game->bullet.x, game->bullet.color, 0}, game->assets.bubble);
                    wattron(game->wattr.win, A_REVERSE);
                    mvwprintw(game->wattr.win, game->wattr.lines - 1, game->wattr.cols - 11, "%lf", game->bullet.angle_deg);
                    wattroff(game->wattr.win, A_REVERSE);
                    wrefresh(game->wattr.win);
                }
                pthread_cond_destroy(&(game->draw_cv));
                pthread_cond_init(&(game->draw_cv), NULL);
                game->draw_signaled = 0;
            }
            game->draw_signaled = 0;
            break;

        case BULLET_FIRED:
            if ((*retval = pthread_mutex_lock(&(game->game_mutex))))
            {
                game->state = GAME_ERROR;

                errbuff("Error: pthread_mutex_lock() in draw() BULLET_FIRED returned: %d\n", *retval);
            }
            game->draw_state = BULLET_FIRED;
            while (game->state == BULLET_FIRED)
            {
                if ((*retval = pthread_cond_wait(&(game->draw_cv), &(game->game_mutex))))
                {
                    game->state = GAME_ERROR;

                    errbuff("Error: pthread_cond_wait() in draw() BULLET_FIRED returned: %d\n", *retval);
                }
                else if (game->state == BULLET_FIRED)
                {
                    game->draw_signaled = 1;
                    wclear(game->wattr.win);
                    box(game->wattr.win, 0, 0);
                    wattron(game->wattr.win, A_REVERSE);
                    mvwprintw(game->wattr.win, 0, 1, "Score: %d", game->score);
                    wattroff(game->wattr.win, A_REVERSE);
                    LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                    {
                        drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                    }
                    drawbubble(&(game->wattr), (bubble_t){((game->wattr.cols - 2) / 2) - game->bullet.y, ((game->wattr.lines - 3) * 2) - game->bullet.x, game->bullet.color, 0}, game->assets.bubble);
                    wattron(game->wattr.win, A_REVERSE);
                    mvwprintw(game->wattr.win, game->wattr.lines - 1, 1, "%lf, %lf", game->bullet.y, game->bullet.x);
                    mvwprintw(game->wattr.win, game->wattr.lines - 1, game->wattr.cols - 11, "%lf", game->bullet.angle_deg);
                    wattroff(game->wattr.win, A_REVERSE);
                    wrefresh(game->wattr.win);
                }
                pthread_cond_destroy(&(game->draw_cv));
                pthread_cond_init(&(game->draw_cv), NULL);
            }
            game->draw_signaled = 0;
            break;

        case BULLET_HIT:
            if ((*retval = pthread_mutex_lock(&(game->game_mutex))))
            {
                game->state = GAME_ERROR;

                errbuff("Error: pthread_mutex_lock() in draw() BULLET_FIRED returned: %d\n", *retval);
            }
            else
            {
                wclear(game->wattr.win);
                box(game->wattr.win, 0, 0);
                wattron(game->wattr.win, A_REVERSE);
                mvwprintw(game->wattr.win, 0, 1, "Score: %d", game->score);
                wattroff(game->wattr.win, A_REVERSE);
                LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                {
                    drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                }
                wattron(game->wattr.win, A_REVERSE);
                mvwprintw(game->wattr.win, game->wattr.lines - 1, 1, "%lf, %lf", game->bullet.y, game->bullet.x);
                mvwprintw(game->wattr.win, game->wattr.lines - 1, game->wattr.cols - 11, "%lf", game->bullet.angle_deg);
                wattroff(game->wattr.win, A_REVERSE);
                wrefresh(game->wattr.win);
                wclear(game->wattr.win);
                wrefresh(game->wattr.win);
                game->draw_state = BULLET_HIT;
            }
            pthread_cond_signal(&(game->mechanics_cv));
            if ((*retval = pthread_cond_wait(&(game->draw_cv), &(game->game_mutex))))
            {
                game->state = GAME_ERROR;

                errbuff("Error: pthread_cond_wait() in draw() BULLET_HIT returned: %d\n", *retval);
            }
            else if (game->state == BULLET_HIT)
            {
                game->draw_signaled = 1;
                wclear(game->wattr.win);
                box(game->wattr.win, 0, 0);
                wattron(game->wattr.win, A_REVERSE);
                mvwprintw(game->wattr.win, 0, 1, "Score: %d", game->score);
                wattroff(game->wattr.win, A_REVERSE);
                LIST_FOREACH(cbubbleptr, &(game->targets.bubbles), entries)
                {
                    drawbubble(&(game->wattr), cbubbleptr->container, game->assets.bubble);
                }
                wrefresh(game->wattr.win);
            }
            game->draw_signaled = 0;
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

                errbuff("Error: pthread_mutex_unlock() in draw() while() after switch (game->state) returned: %d\n", *retval);
            }
        }
    }
    *retval = pthread_mutex_unlock(&(game->game_mutex));
    if (*retval)
    {

        errbuff("Error: pthread_mutex_unlock() in draw() returned: %d\n", *retval);
    }
    return (void *)retval;
}

void *input(void *args)
{
    game_o_t *game = (game_o_t *)args;
    int *retval = (int *)malloc(sizeof(int)), key;
    cinput_t *cinputptr;
    // TODO: Vars for quit to menu in the middle of BULLET_FIRED
    // struct timeval start, end;
    // gettimeofday(&start, NULL);
    nodelay(game->wattr.win, TRUE);
    *retval = 0;
    *retval = pthread_mutex_lock(&(game->game_mutex));
    if (*retval)
    {
        game->state = GAME_ERROR;

        errbuff("Error: pthread_mutex_lock() in input() returned: %d\n", *retval);
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

                errbuff("Error: pthread_cond_wait() in input() BULLET_READY returned: %d\n", *retval);
            }
            else if (game->state == BULLET_READY)
            {
                if ((*retval = pthread_mutex_unlock(&(game->game_mutex))))
                {
                    game->state = GAME_ERROR;

                    errbuff("Error: pthread_mutex_unlock() in input() BULLET_READY returned: %d\n", *retval);
                }
                else
                {
                    game->input_signaled = 1;
                    nodelay(game->wattr.win, FALSE);
                    noecho();
                    keypad(game->wattr.win, TRUE);
                    while ((key = wgetch(game->wattr.win)) != 10)
                    {
                        cinputptr = (cinput_t *)malloc(sizeof(cinput_t));
                        cinputptr->c = key;
                        TAILQ_INSERT_TAIL(&(game->input_queue), cinputptr, entries);
                        pthread_cond_signal(&(game->mechanics_cv));
                        if ((key == (int)'q') || (key == (int)'Q'))
                        {
                            game->result = GAME_QUIT;
                            break;
                        }
                    }
                    // insert the enter into the queue as the last key
                    cinputptr = (cinput_t *)malloc(sizeof(cinput_t));
                    cinputptr->c = key;
                    TAILQ_INSERT_TAIL(&(game->input_queue), cinputptr, entries);
                    nodelay(game->wattr.win, TRUE);
                    pthread_cond_signal(&(game->mechanics_cv));
                }
            }
            pthread_cond_destroy(&(game->input_cv));
            pthread_cond_init(&(game->input_cv), NULL);
            game->input_signaled = 0;
            game->input_state = BULLET_FIRED;
            pthread_mutex_unlock(&(game->game_mutex));
            break;

        case BULLET_FIRED:
            game->input_state = BULLET_FIRED;
            break;

        case BULLET_HIT:
            game->input_state = BULLET_HIT;
            break;

        default:
            break;
        }

        // TODO: Make exiting from bullet fired
        // ! exiting while bubble is moving is not yet working
        // {
        //     gettimeofday(&end, NULL);
        //     if (((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) > 1000000.0 / 25.0)
        //     {
        //         int c = wgetch(game->wattr.win);
        //         if (c == (int)'q' || c == (int)'Q')
        //             game->state = GAME_END;
        //     }
        // }
    }
    nodelay(game->wattr.win, FALSE);
    *retval = pthread_mutex_unlock(&(game->game_mutex));
    if (*retval)
    {

        errbuff("Error: pthread_mutex_unlock() in input() returned: %d\n", *retval);
    }
    return (void *)retval;
}

int findCluster(const cbubble_t *startBubble, TAILQ_HEAD(bubble_queue_t, container_voipq_type) * queueHead, const target_t *targets)
{
    int size = 0;
    cbubble_t *cbptr1 = NULL;
    cvoipq_t *cvqptr1 = NULL, *cvqptr2 = NULL;
    cvqptr1 = malloc(sizeof(cvoipq_t));
    cvqptr1->pointer = startBubble;
    TAILQ_INSERT_TAIL(queueHead, cvqptr1, entries);
    TAILQ_FOREACH(cvqptr1, queueHead, entries)
    {
        LIST_FOREACH(cbptr1, &(targets->bubbles), entries)
        {
            if ((cbptr1->container.color == startBubble->container.color) && !(cbptr1->container.searched))
            {
                // check if already in queue
                int notin = 1;
                TAILQ_FOREACH(cvqptr2, queueHead, entries)
                {
                    if (cbptr1 == cvqptr2->pointer)
                    {
                        notin = 0;
                        break;
                    }
                }
                if (notin)
                {
                    double bx = cbptr1->container.y;
                    double by = cbptr1->container.x;
                    double tmpx = ((cbubble_t *)(cvqptr1->pointer))->container.y;
                    double tmpy = ((cbubble_t *)(cvqptr1->pointer))->container.x;
                    if ((sqrt(pow(tmpx - bx, 2) + pow(tmpy - by, 2)) <= sqrt(20.0)))
                    {
                        cvqptr2 = malloc(sizeof(cvoipq_t));
                        cvqptr2->pointer = cbptr1;
                        TAILQ_INSERT_TAIL(queueHead, cvqptr2, entries);
                    }
                }
            }
        }
        ++size;
    }
    if (startBubble->container.searched == 1)
    {
        cvqptr1 = TAILQ_FIRST(queueHead);
        TAILQ_REMOVE(queueHead, cvqptr1, entries);
        free(cvqptr1);
        --size;
    }
    return size;
}

void unmarkSearched(TAILQ_HEAD(bubble_queue_t, container_voipq_type) * queueHead)
{
    cvoipq_t *cvqptr1 = NULL;
    TAILQ_FOREACH(cvqptr1, queueHead, entries)
    {
        ((cbubble_t *)cvqptr1->pointer)->container.searched = 0;
    }
}

void markSearched(TAILQ_HEAD(bubble_queue_t, container_voipq_type) * queueHead)
{
    cvoipq_t *cvqptr1 = NULL;
    TAILQ_FOREACH(cvqptr1, queueHead, entries)
    {
        ((cbubble_t *)cvqptr1->pointer)->container.searched = 1;
    }
}

int pathToRoof(const cbubble_t *startBubble, const target_t *targets)
{
    int path = 0;
    if (startBubble != NULL)
    {
        TAILQ_HEAD(bubble_queue_t, container_voipq_type)
        searchQueue;
        TAILQ_INIT(&searchQueue);
        cvoipq_t *cvqptr1 = malloc(sizeof(cvoipq_t)), *cvqptr2 = NULL;
        cvqptr1->pointer = startBubble;
        TAILQ_INSERT_HEAD(&searchQueue, cvqptr1, entries);
        TAILQ_FOREACH(cvqptr1, &searchQueue, entries)
        {
            if (((cbubble_t *)cvqptr1->pointer)->container.y == 2.0)
            {
                path = 1;
                break;
            }
            else
            {
                cbubble_t *cbptr1 = NULL;
                LIST_FOREACH(cbptr1, &(targets->bubbles), entries)
                {
                    int notin = 1;
                    TAILQ_FOREACH(cvqptr2, &searchQueue, entries)
                    {
                        if (cbptr1 == cvqptr2->pointer)
                        {
                            notin = 0;
                            break;
                        }
                    }
                    if (notin && !(cbptr1->container.searched))
                    {
                        double bx = cbptr1->container.y;
                        double by = cbptr1->container.x;
                        double tmpx = ((cbubble_t *)(cvqptr1->pointer))->container.y;
                        double tmpy = ((cbubble_t *)(cvqptr1->pointer))->container.x;
                        if ((sqrt(pow(tmpx - bx, 2) + pow(tmpy - by, 2)) <= sqrt(20.0)))
                        {
                            cvqptr2 = malloc(sizeof(cvoipq_t));
                            cvqptr2->pointer = cbptr1;
                            TAILQ_INSERT_TAIL(&searchQueue, cvqptr2, entries);
                        }
                    }
                }
            }
        }
        cvoipq_t *ptr1 = TAILQ_FIRST(&searchQueue), *ptr2;
        while (ptr1 != NULL)
        {
            ptr2 = TAILQ_NEXT(ptr1, entries);
            free(ptr1);
            ptr1 = ptr2;
        }
    }
    return path;
}

void *mechanics(void *args)
{
    // * thread variables
    game_o_t *game = (game_o_t *)args;
    int *retval = (int *)malloc(sizeof(int));
    *retval = 0;
    // * variables for handling targets, and their removal
    TAILQ_HEAD(bubble_queue_t, container_voipq_type)
    bubble_queue;
    TAILQ_INIT(&(bubble_queue));
    // * Timer variables
    struct timeval start, end;
    gettimeofday(&start, NULL);

    *retval = pthread_mutex_lock(&(game->game_mutex));
    if (*retval)
    {
        game->state = GAME_ERROR;

        errbuff("Error: pthread_mutex_lock() in mechanics() returned: %d\n", *retval);
    }
    while (game->state != GAME_END && game->state != GAME_ERROR)
    {
        switch (game->state)
        {
        case BULLET_READY:
        {
            // * do once
            const static double angle_increment = 0.01;
            // * variables for handling input queue
            cinput_t *cinputptr = NULL, *cinputptr1 = NULL;
            // * Bullet variables
            game->bullet.speed = 5.0;
            game->bullet.color = (bc_t)(1 + ((double)rand() / (double)RAND_MAX) * 8.0);
            // // ! DEBUG
            // game->bullet.color = BUBBLE_WHITE;
            // // ! DEBUG
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

                    errbuff("Error: pthread_cond_wait() in mechanics() BULLET_READY returned: %d\n", *retval);
                }
                else if (game->state == BULLET_READY)
                {
                    // Unlock mutex so that draw thread can continue to draw
                    if ((*retval = pthread_mutex_unlock(&(game->game_mutex))))
                    {
                        game->state = GAME_ERROR;

                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY returned: %d\n", *retval);
                    }
                    else
                    {
                        game->mechanics_signaled = 1;
                        cinputptr = TAILQ_FIRST(&(game->input_queue));
                        while (cinputptr != NULL)
                        {
                            if (cinputptr->c == (int)'q' || cinputptr->c == (int)'Q')
                            {
                                game->state = GAME_END;
                                break;
                            }
                            switch (cinputptr->c)
                            {
                            case KEY_RIGHT:
                                // lock here so that mechanics thread waits for the draw thread to finish
                                *retval = pthread_mutex_lock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;

                                    errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_READY KEY_RIGHT returned: %d\n", *retval);
                                }
                                else
                                {
                                    game->bullet.angle_deg -= (game->bullet.angle_deg - angle_increment < -M_PI_2) ? 0.0 : angle_increment;
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;

                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY KEY_RIGHT returned: %d\n", *retval);
                                    }
                                }
                                break;
                            case KEY_LEFT:
                                *retval = pthread_mutex_lock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;

                                    errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_READY KEY_LEFT returned: %d\n", *retval);
                                }
                                else
                                {
                                    game->bullet.angle_deg += (game->bullet.angle_deg + angle_increment > M_PI_2) ? 0.0 : angle_increment;
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;

                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY KEY_LEFT returned: %d\n", *retval);
                                    }
                                }
                                break;
                            case 10:
                                *retval = pthread_mutex_lock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;

                                    errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_READY 10 returned: %d\n", *retval);
                                }
                                else
                                {
                                    game->state = BULLET_FIRED;
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;

                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_READY 10 returned: %d\n", *retval);
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
                                while ((!game->draw_signaled) && game->state == BULLET_READY)
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
            // * Do once
            pthread_mutex_unlock(&(game->game_mutex));
            while ((!game->draw_signaled) && game->state == BULLET_FIRED)
                pthread_cond_signal(&(game->draw_cv));
            pthread_mutex_lock(&(game->game_mutex));
            game->mechanics_state = BULLET_FIRED;
            double tmpx = game->bullet.x;
            double tmpy = game->bullet.y;
            double tmpm = tan(game->bullet.angle_deg);
            double tmpc = 0.0;
            gettimeofday(&start, NULL);
            *retval = pthread_cond_wait(&(game->mechanics_cv), &(game->game_mutex));
            if (*retval)
            {
                game->state = GAME_ERROR;

                errbuff("Error: pthread_cond_wait() in mechanics() BULLET_FIRED returned: %d\n", *retval);
            }
            else
            {
                *retval = pthread_mutex_unlock(&(game->game_mutex));
                if (*retval)
                {
                    game->state = GAME_ERROR;

                    errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                }
                else
                {
                    bubble_t *collidedBubble = NULL;
                    game->mechanics_signaled = 1;
                    while ((game->state == BULLET_FIRED) && (tmpx < (double)((game->wattr.lines - 4) * 2)) && (tmpx >= 0.0))
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

                                errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                            }
                            else
                            {
                                if ((collidedBubble = collide(&(game->wattr), &(game->targets), tmpm, tmpc, tmpx, tmpy)) != NULL)
                                {
                                    double bx = (((double)game->wattr.lines - 3) * 2) - collidedBubble->y;
                                    double by = (((double)game->wattr.cols - 2) / 2) - collidedBubble->x;
                                    double halfangle = (atan(0.5));
                                    if (tmpy - by == 0.0)
                                    {
                                        double chance = ((double)rand()) / ((double)RAND_MAX);
                                        if (chance < 0.5)
                                        {
                                            game->bullet.x = bx - 4.0;
                                            game->bullet.y = by - 2.0;
                                        }
                                        else
                                        {
                                            game->bullet.x = bx - 4.0;
                                            game->bullet.y = by + 2.0;
                                        }
                                    }
                                    else
                                    {
                                        double theta = atan2((tmpy - by), (tmpx - bx));
                                        if ((theta >= 0.0) && (theta < halfangle))
                                        {
                                            game->bullet.x = bx + 4.0;
                                            game->bullet.y = by + 2.0;
                                        }
                                        else if ((theta >= halfangle) && (theta < (M_PI - halfangle)))
                                        {
                                            game->bullet.x = bx;
                                            game->bullet.y = by + 4.0;
                                        }
                                        else if ((theta >= (M_PI - halfangle)) && (theta < M_PI))
                                        {
                                            game->bullet.x = bx - 4.0;
                                            game->bullet.y = by + 2.0;
                                        }
                                        else if ((theta >= -M_PI) && (theta < (-M_PI + halfangle)))
                                        {
                                            game->bullet.x = bx - 4.0;
                                            game->bullet.y = by - 2.0;
                                        }
                                        else if ((theta >= (-M_PI + halfangle)) && (theta < (-halfangle)))
                                        {
                                            game->bullet.x = bx;
                                            game->bullet.y = by - 4.0;
                                        }
                                        else if ((theta < 0.0) && (theta >= (-halfangle)))
                                        {

                                            game->bullet.x = bx + 4.0;
                                            game->bullet.y = by - 2.0;
                                        }
                                    }
                                    // unlock to draw last update
                                    *retval = pthread_mutex_unlock(&(game->game_mutex));
                                    if (*retval)
                                    {
                                        game->state = GAME_ERROR;

                                        errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                                    }
                                    break;
                                }
                                else if ((int)(tmpx - game->bullet.x) != 0 || (int)(tmpy - game->bullet.y) != 0)
                                {
                                    game->bullet.x = tmpx;
                                    game->bullet.y = tmpy;
                                    pthread_cond_signal(&(game->draw_cv));
                                }
                                *retval = pthread_mutex_unlock(&(game->game_mutex));
                                if (*retval)
                                {
                                    game->state = GAME_ERROR;

                                    errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                                }
                            }
                            gettimeofday(&start, NULL);
                        }
                    }
                    if (game->state == BULLET_FIRED)
                    {
                        // lock again to change game state to BULLET_HIT
                        *retval = pthread_mutex_lock(&(game->game_mutex));
                        if (*retval)
                        {
                            game->state = GAME_ERROR;

                            errbuff("Error: pthread_mutex_lock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                        }
                        else
                        {
                            // add the bullet to the existing list of targets with a searched flag to indicate where to start searching for a cluster of the same color in BULLET_HIT
                            cbubble_t *ptr = (cbubble_t *)malloc(sizeof(cbubble_t));
                            ptr->container = (bubble_t){((game->wattr.cols - 2) / 2) - game->bullet.y, ((game->wattr.lines - 3) * 2) - game->bullet.x, game->bullet.color, 1};
                            cvoipq_t *tmpcvoidp = (cvoipq_t *)malloc(sizeof(cvoipq_t));
                            tmpcvoidp->pointer = ptr;
                            TAILQ_INSERT_HEAD(&bubble_queue, tmpcvoidp, entries);
                            LIST_INSERT_HEAD(&(game->targets.bubbles), ptr, entries);
                            // Change game state after other draw thread finshes its task
                            game->state = BULLET_HIT;
                            pthread_cond_signal(&(game->draw_cv));
                            pthread_cond_signal(&(game->input_cv));
                            *retval = pthread_mutex_unlock(&(game->game_mutex));
                            if (*retval)
                            {
                                game->state = GAME_ERROR;

                                errbuff("Error: pthread_mutex_unlock() in mechanics() BULLET_FIRED returned: %d\n", *retval);
                            }
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
        {
            game->mechanics_state = BULLET_HIT;
            pthread_cond_wait(&(game->mechanics_cv), &(game->game_mutex));
            if (game->state == BULLET_HIT)
            {
                TAILQ_HEAD(bubble_queue_t, container_voipq_type)
                tmp_queue;
                TAILQ_INIT(&(tmp_queue));
                cvoipq_t *ptrqueue = NULL;
                int num = findCluster((cbubble_t *)(TAILQ_FIRST(&bubble_queue)->pointer), &tmp_queue, &(game->targets)) + 1;
                markSearched(&tmp_queue);
                TAILQ_CONCAT(&bubble_queue, &tmp_queue, entries);
                if (num >= 3)
                {
                    // search for hanging clusters here
                    cbubble_t *ptr;
                    TAILQ_FOREACH(ptrqueue, &bubble_queue, entries)
                    {
                        LIST_FOREACH(ptr, &(game->targets.bubbles), entries)
                        {
                            if (!(ptr->container.searched))
                            {
                                double bx = (((double)game->wattr.lines - 3) * 2) - ptr->container.y;
                                double by = (((double)game->wattr.cols - 2) / 2) - ptr->container.x;
                                double tmpx = (((double)game->wattr.lines - 3) * 2) - ((cbubble_t *)(ptrqueue->pointer))->container.y;
                                double tmpy = (((double)game->wattr.cols - 2) / 2) - ((cbubble_t *)(ptrqueue->pointer))->container.x;
                                // check if this bubble is connected to a hanging cluster
                                if ((sqrt(pow(tmpx - bx, 2) + pow(tmpy - by, 2)) <= sqrt(20.0)) && (bx <= tmpx))
                                {
                                    int hangingTrue = 1;
                                    int clusterSize = findCluster(ptr, &tmp_queue, &(game->targets));
                                    if (clusterSize > 0)
                                    {
                                        markSearched(&tmp_queue);
                                        cvoipq_t *cvqptr1 = NULL;
                                        TAILQ_FOREACH(cvqptr1, &tmp_queue, entries)
                                        {
                                            // check if the cluster is connected to the ceiling
                                            if(pathToRoof(cvqptr1->pointer, &(game->targets)))
                                            {
                                                hangingTrue = 0;
                                                break;
                                            }
                                        }
                                        if (!hangingTrue)
                                        {
                                            // if this isn't a hanging cluster unmark it so that other clusters can check against this
                                            unmarkSearched(&tmp_queue);
                                        }
                                        else
                                        {
                                            // if this is a hanging cluster then add it to the list of bubbles to be deleted
                                            TAILQ_CONCAT(&bubble_queue, &tmp_queue, entries);
                                            num += clusterSize;
                                        }
                                    }
                                }
                            }
                        }
                        ++num;
                    }
                    // delete clusters the bullet hit and any hanging clusters
                    TAILQ_FOREACH(ptrqueue, &bubble_queue, entries)
                    {
                        LIST_REMOVE((cbubble_t *)(ptrqueue->pointer), entries);
                        free(ptrqueue->pointer);
                    }
                    game->score += ((num - 1) * 250 + 200);
                }
                else
                {
                    TAILQ_FOREACH(ptrqueue, &bubble_queue, entries)
                    {
                        ((cbubble_t *)(ptrqueue->pointer))->container.searched = 0;
                    }
                }
                cvoipq_t *ptr1 = TAILQ_FIRST(&bubble_queue), *ptr2;
                while (ptr1 != NULL)
                {
                    ptr2 = TAILQ_NEXT(ptr1, entries);
                    free(ptr1);
                    ptr1 = ptr2;
                }
                TAILQ_INIT(&bubble_queue);
                if (LIST_EMPTY(&(game->targets.bubbles)))
                {
                    game->state = GAME_END;
                    game->result = GAME_FINISHED;
                }
                else
                {
                    double bx = (((double)game->wattr.lines - 3) * 2) - game->targets.bubbles.lh_first->container.y;
                    double by = (((double)game->wattr.cols - 2) / 2) - game->targets.bubbles.lh_first->container.x;
                    if (sqrt(pow(bx, 2) + pow(by, 2)) <= sqrt(20.1))
                    {
                        game->state = GAME_END;
                        game->result = GAME_OVER;
                    }
                    else
                    {
                        game->state = BULLET_READY;
                    }
                }
            }
            pthread_cond_signal(&(game->draw_cv));
            pthread_mutex_unlock(&(game->game_mutex));
        }
        break;

        default:
            game->state = GAME_ERROR;
            break;
        }
        if (game->state != GAME_END || game->state != GAME_ERROR)
        {
            *retval = pthread_mutex_unlock(&(game->game_mutex));
            if (*retval)
            {

                errbuff("Error: pthread_mutex_unlock() in mechanics() while() after switch(game->state) returned: %d\n", *retval);
            }
        }
    }
    *retval = pthread_mutex_unlock(&(game->game_mutex));
    if (*retval)
    {

        errbuff("Error: pthread_mutex_unlock() in mechanics() returned: %d\n", *retval);
    }
    return (void *)retval;
}

int game_loop(WINDOW *win, int level)
{
    game_o_t game = {};
    int newmaxlines, newmaxcols;
    int retval = 0;
    int input_signaled = 0, draw_signaled = 0, mechanics_signaled = 0;
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
                srand(time(NULL));
                // Set next game state
                game.state = BULLET_READY;
                game.draw_state = GAME_INIT;
                game.input_state = GAME_INIT;
                game.mechanics_state = GAME_INIT;
                game.result = GAME_QUIT;
                game.input_signaled = 0;
                game.mechanics_signaled = 0;
                game.draw_signaled = 0;
                game.shots = 0;
                game.score = 0;
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

                        errbuff("Error: phtread_attr_init(tmpattr) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_attr_setdetachstate(&tmpattr, PTHREAD_CREATE_JOINABLE);
                    if (retval)
                    {

                        errbuff("Error: phtread_attr_setdetachstate(tmpattr) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_cond_init(&(game.draw_cv), NULL);
                    if (retval)
                    {

                        errbuff("Error: phtread_cond_init(draw_cv) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_cond_init(&(game.input_cv), NULL);
                    if (retval)
                    {

                        errbuff("Error: phtread_cond_init(input_cv) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_cond_init(&(game.mechanics_cv), NULL);
                    if (retval)
                    {

                        errbuff("Error: phtread_cond_init(mechanics_cv) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_mutex_init(&(game.game_mutex), NULL);
                    if (retval)
                    {

                        errbuff("Error: pthread_mutex_init(game_mutex) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_create(&(game.draw_thread), &tmpattr, draw, (void *)(&game));
                    if (retval)
                    {

                        errbuff("Error: pthread_create(draw_thread) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_create(&(game.mechanics_thread), &tmpattr, mechanics, (void *)(&game));
                    if (retval)
                    {

                        errbuff("Error: pthread_create(mechanics_thread) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_create(&(game.input_thread), &tmpattr, input, (void *)(&game));
                    if (retval)
                    {

                        errbuff("Error: pthread_create(input_thread) returned: %d\n", retval);

                        return retval;
                    }
                    retval = pthread_attr_destroy(&tmpattr);
                    if (retval)
                    {

                        errbuff("Error: pthread_attr_destroy(tmpattr) returned: %d\n", retval);

                        return retval;
                    }
                }
            }
            else
            {
                errbuff("Failed to load assets. loadAssetsFromFile() returned: %d\n", retval);
                game.state = GAME_ERROR;
            }
            break;
        case BULLET_READY:
        {
            // signal to draw thread to draw the initialized game once it's ready
            if (!draw_signaled && game.draw_state == BULLET_READY)
            {
                pthread_cond_signal(&(game.draw_cv));
                if (game.draw_signaled)
                    draw_signaled = 1;
            }
            // then signal to the input thread to start queueing inputs from the user
            if (draw_signaled && !input_signaled && game.input_state == BULLET_READY)
            {
                pthread_cond_signal(&(game.input_cv));
                if (game.input_signaled)
                    input_signaled = 1;
            }
        }
        break;
        case BULLET_FIRED:
            // wait for all threads to reach this stage then signal to them the game state changed and to continue
            if (game.input_state == BULLET_FIRED && game.draw_state == BULLET_FIRED && game.mechanics_state == BULLET_FIRED && !mechanics_signaled)
            {
                pthread_cond_signal(&(game.mechanics_cv));
                if (game.mechanics_signaled)
                    mechanics_signaled = 1;
            }
            break;
        case BULLET_HIT:
            input_signaled = 0, draw_signaled = 0, mechanics_signaled = 0;
            break;
        default:
            game.state = GAME_END;
            wgetch(win);
            if (game.input_state == BULLET_FIRED && game.draw_state == BULLET_FIRED && game.mechanics_state == BULLET_FIRED)
            {
                pthread_cond_signal(&(game.input_cv));
                pthread_cond_signal(&(game.draw_cv));
                pthread_cond_signal(&(game.mechanics_cv));
            }
            break;
        }
        getmaxyx(stdscr, newmaxlines, newmaxcols);
        // redraw the window if the terminal changed size
        if ((newmaxlines != game.wattr.maxlines || newmaxcols != game.wattr.maxcols))
        {
            pthread_mutex_lock(&(game.game_mutex));
            game.wattr.maxlines = newmaxlines;
            game.wattr.maxcols = newmaxcols;
            wclear(win);
            wresize(win, game.wattr.lines, game.wattr.cols);
            mvwin(win, (game.wattr.maxlines / 2) - (game.wattr.lines / 2), (game.wattr.maxcols / 2) - (game.wattr.cols / 2));
            box(win, 0, 0);
            wrefresh(win);
            pthread_mutex_unlock(&(game.game_mutex));
        }
    }
    int *threadret;
    // TODO: Take care of post game stuff (Score, shots, free dynamic vars(sprites (DONE), Targets(DONE),etc) etc)
    if (game.state == GAME_ERROR)
    {
        // pthread_cancel(game.input_thread);
        // pthread_cancel(game.draw_thread);
        // pthread_cancel(game.mechanics_thread);
    }
    else if (game.state == GAME_END)
    {
        // signal to any threads still waiting on a condition variable signal
        pthread_cond_signal(&(game.draw_cv));
        pthread_cond_signal(&(game.input_cv));
        pthread_cond_signal(&(game.mechanics_cv));
        // join all threads to main thread
        pthread_join(game.draw_thread, (void **)&threadret);

        errbuff("draw_thread joined and returned: %d\n", *threadret);

        pthread_join(game.input_thread, (void **)&threadret);

        errbuff("input_thread joined and returned: %d\n", *threadret);

        pthread_join(game.mechanics_thread, (void **)&threadret);

        errbuff("mechanics_thread joined and returned: %d\n", *threadret);

        // destroy the remaining condition variables
        pthread_cond_destroy(&(game.draw_cv));
        pthread_cond_destroy(&(game.input_cv));
        pthread_cond_destroy(&(game.mechanics_cv));
        // destroy the game mutex
        pthread_mutex_destroy(&(game.game_mutex));
        // clear the window and refresh
        wclear(win);
        wrefresh(win);
        if (game.result != GAME_QUIT)
        {
            // Print score here
            box(win, 0, 0);
            mvwprintw(win, 5, 1, "Score: %d", game.score);
            mvwprintw(win, 6, 1, "Number of shots: %d", game.shots);
            mvwprintw(win, 8, 1, "Press Q to exit to menu...");
            wrefresh(win);
            nodelay(win, FALSE);
            int c = wgetch(win);
            while ((c != (int)'Q') && (c != (int)'q'))
            {
                c = wgetch(win);
            }
            wclear(win);
            wrefresh(win);
        }
    }
    spriteUnloader(&(game.assets.arrow));
    spriteUnloader(&(game.assets.bubble));
    spriteUnloader(&(game.assets.layout));
    targetUnloader(&(game.targets));
    clear();
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

void selectLevelscreen(WINDOW *win, int wincols, int winlines, int maxlines, int maxcols)
{
    //create window
    if (has_colors())
    {
        if (maxlines < winlines || maxcols < wincols)
        {
            addstr("Terminal too small\n");
            refresh();
            getch();
        }
        else
        {
            if (start_color() == OK)
            {
                win = newwin(winlines, wincols, (maxlines / 2) - (winlines / 2) - (winlines % 2), (maxcols / 2) - (wincols / 2) - (wincols % 2));
                keypad(win, true);

                char menu[5][50] = {"1-LEVEL_1", "2-LEVEL_2", "3-LEVEL_3", "4-LEVEL_4", "5-BACK"};
                int key, ref = 1;
                int highlight = 0;

                while (key != 10 || highlight != 4)
                {
                    if (ref)
                    {
                        mvwin(win, (maxlines / 2) - (winlines / 2) - (winlines % 2), (maxcols / 2) - (wincols / 2) - (wincols % 2));
                        wresize(win, winlines, wincols);
                        box(win, 0, 0);

                        mvwprintw(win, 4, 17, "===================================");
                        mvwprintw(win, 5, 17, "||   PLEASE SELECT GAME LEVEL    ||");
                        mvwprintw(win, 6, 17, "===================================");
                        ref = 0;
                    }
                    for (int i = 0; i < 5; i++)
                    {
                        if (i == highlight)
                        {
                            wattron(win, A_REVERSE);
                        }
                        mvwprintw(win, i + 9, 15, menu[i]);
                        wattroff(win, A_REVERSE);
                    }
                    wrefresh(win);
                    key = wgetch(win);
                    switch (key)
                    {
                    case KEY_UP:
                        if (highlight > 0)
                            highlight--;
                        break;
                    case KEY_DOWN:
                        if (highlight < 4)
                            highlight++;
                        break;
                    case 10:
                        if (highlight == 0)
                        {
                            errbuff("game_loop returned: %d\n", game_loop(win, 1));
                            ref = 1;
                        }
                        else if (highlight == 1)
                        {
                            errbuff("game_loop returned: %d\n", game_loop(win, 2));
                            ref = 1;
                        }
                        else if (highlight == 2)
                        {
                            errbuff("game_loop returned: %d\n", game_loop(win, 3));
                            ref = 1;
                        }
                        else if (highlight == 3)
                        {
                            errbuff("game_loop returned: %d\n", game_loop(win, 4));
                            ref = 1;
                        }
                        wrefresh(win);
                        break;
                    }
                }
            }
            else
            {
                clear();
                addstr("Cannot start colours\n");
                refresh();
            }
        }
    }
    else
    {
        addstr("Terminal not colour capable\n");
        refresh();
    }
    wclear(win);
}
