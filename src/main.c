#include "lib.h"
errorbuffer_t errbuffer; // Global variable for buffering printf
pthread_mutex_t errbuff_mutex;
int main()
{
    srand(time(NULL));
    setlocale(LC_ALL, "");
    int retval = pthread_mutex_init(&(errbuff_mutex), NULL);
    if (retval)
    {
        fprintf(stderr, "Error: pthread_mutex_init(errbuff_mutex) in main() returned: %d\n", retval);
    }
    else
    {
        initscr();
        noecho();
        cbreak();
        curs_set(0);

        int wincols = 70, winlines = 30;
        int maxlines, maxcols;
        WINDOW *win;
        // set x and y max value
        getmaxyx(stdscr, maxlines, maxcols);

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

                    char menu[3][50] = {"1-PLAY", "2-SELECT_LEVEL", "3-QUIT"};
                    int key, ref = 1;
                    int highlight = 0;

                    while (key != 10 || highlight != 2)
                    {
                        if (ref)
                        {
                            mvwin(win, (maxlines / 2) - (winlines / 2) - (winlines % 2), (maxcols / 2) - (wincols / 2) - (wincols % 2));
                            wresize(win, winlines, wincols);
                            box(win, 0, 0);

                            mvwprintw(win, 4, 17, "===================================");
                            mvwprintw(win, 5, 17, "|| WELCOME TO CLI BUBBLE SHOOTER ||");
                            mvwprintw(win, 6, 17, "===================================");
                            ref = 0;
                        }
                        for (int i = 0; i < 3; i++)
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
                            if (highlight < 2)
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
                                selectLevelscreen(win, wincols, winlines, maxlines, maxcols);
                                ref = 1;
                            }
                            wrefresh(win);
                            break;
                        }
                    }
                    delwin(win);
                    refresh();
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
        clear();
        endwin();

        pthread_mutex_destroy(&(errbuff_mutex));
        if (errbuffer.buffer != NULL)
        {
            fputs(errbuffer.buffer, stderr);
            free(errbuffer.buffer);
        }
    }
    perror("last errno");
    pthread_exit(NULL);
}